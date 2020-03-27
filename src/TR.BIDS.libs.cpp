//TR.BIDS.lib.cpp
//Under the MIT License
//Copyright 2020 Tetsu Otter

#define TIMEOUT_NUM 10000

#include <Arduino.h>
#include <stdlib.h>
#include <string.h>
#include "UFunc.h"
#include "TR.BIDS.libs.h"

BIDS::BIDS(Stream *ser)
{
  UsingSerial = ser;

  isEnable = true;
}

BIDS::~BIDS()
{
  isEnable = false;
}

void BIDS::ASActoinsSet(ASAction *asacts, const char type, const int data_num, AS_OnDataGot act)
{
  (*asacts).type = type;
  (*asacts).data_num = data_num;
  (*asacts).action = act;
}
void BIDS::ASActoinsSet(ASAction *asactDst, ASAction *asactSrc)
{
  ASActoinsSet(asactDst, (*asactSrc).type, (*asactSrc).data_num, (*asactSrc).action);
  ASActoinsSet(asactSrc, 0, 0, NULL);
}

bool BIDS::AddAutoSend(char type, int data_num, AS_OnDataGot act)
{
  if (act == NULL)
    return false;
  if (Actions_count >= AS_MAX)
    return false;
  bool IsSameDataAlready = false;
  for (int i = 0; i < Actions_count; i++)
  {
    if (ASActions[i].type == type && ASActions[i].data_num == data_num)
    {
      IsSameDataAlready = true;
      if (ASActions[i].action == act)
        return false;
    }
  }

  char ca[CMD_LEN_MAX];
  if (IsSameDataAlready || CmdSenderI(cmdCAGet(ca, CMD_LEN_MAX, 'A', type, data_num)) != 0)
    return false;

  ASActoinsSet(&ASActions[Actions_count++], type, data_num, act);

  return true;
}
bool BIDS::AddAutoSend(ASAction asa)
{
  return AddAutoSend(asa.type, asa.data_num, asa.action);
}

bool BIDS::RmvAutoSend(char type, int data_num, AS_OnDataGot act = NULL)
{
  if (Actions_count <= 0)
    return false;

  int ActNotSameSkipCount = 0;
  bool IsFound = false;

  for (int i = 0; i < Actions_count; i++)
  {
    if (ASActions[i].type == type && ASActions[i].data_num == data_num)
    {
      if (act != NULL && ASActions[i].action != act) //Action Check Mode && Action is not same
      {
        ActNotSameSkipCount++;
        continue;
      }

      IsFound = true;

      if (i == (Actions_count - 1)) //Means that this is the Last Element
      {
        ASActoinsSet(&ASActions[i], 0, 0, NULL);
        return true;
      }
      for (int j = i; j < Actions_count - 1; j++)
        ASActoinsSet(&ASActions[j], &ASActions[j + 1]);
      Actions_count--;
    }
  }
  char ca[CMD_LEN_MAX];
  if (IsFound && ActNotSameSkipCount == 0)
    CmdSenderI(cmdCAGet(ca, CMD_LEN_MAX, 'D', type, data_num));

  return IsFound;
}
bool BIDS::RmvAutoSend(ASAction asa)
{
  return RmvAutoSend(asa.type, asa.data_num, asa.action);
}

bool BIDS::ASDataCheck(bool *NonASCMDGot)
{
  bool b = false;
  bool *nonASCMDGot = NonASCMDGot == NULL ? &b : NonASCMDGot;

  *nonASCMDGot = false;

  if ((*UsingSerial).available() <= 0)
    return false;

  ZeroFill(LastCMD, CMD_LEN_MAX);

  int len = (*UsingSerial).readBytesUntil('\n', LastCMD, CMD_LEN_MAX);

  if (len <= 0)
    return false;

  *nonASCMDGot = true;

  if (!HeaderCheck(LastCMD, CMD_LEN_MAX, 'I'))
    return false;

  char *cp = NULL;
  int dnum = (int)strtol(&LastCMD[4], NULL, 10);

  for (int i = 0; i < len; i++)
    if (LastCMD[i] == 'X')
      cp = &LastCMD[i + 1];
  if (cp == NULL)
    return false;

  int valI = (int)strtol(cp, NULL, 10);
  double valF = strtod(cp, NULL);
  for (int i = 0; i < Actions_count; i++)
  {
    if (LastCMD[3] == ASActions[i].type && dnum == ASActions[i].data_num)
    {
      ASActions[i].action(valI, valF);
      *nonASCMDGot = false;
    }
  }

  return !(*nonASCMDGot);
}

int BIDS::CmdSender(const char *cmd, char *ret, int retlen)
{
  if (cmd == NULL)
    return -1;

  int w = 0;

  w = (*UsingSerial).println(cmd); //Serial Send
  if (ret == NULL || retlen <= 0)  //Only Sending Mode
    return -w;

  for (w = 0; (*UsingSerial).available() <= 0; w++) //Serial ReadWait
    if (w >= TIMEOUT_NUM)                           //Timeout Check
      return -1;
    else
      delay(1);

  int len = (*UsingSerial).available();
  if (len >= retlen)
    len = retlen;
  ZeroFill(ret, retlen);
  for (int i = 0; i < len; i++)
    ret[i] = (*UsingSerial).read();
  return len;
}

bool BIDS::CmdSender(const char *cmd, int *ret)
{
  if (cmd == NULL || ret == NULL)
    return false;

  int len = 0;
  char charr[CMD_LEN_MAX];
  len = CmdSender(cmd, charr, CMD_LEN_MAX);
  if (!HeaderCheck(charr, len))
    return false;

  *ret = 0;
  for (int i = 0; i < len; i++)
  {
    if (charr[i] == 'X')
    {
      *ret = (int)strtol(&charr[i + 1], NULL, 10);
      return true;
    }
    else if (cmd[i] != charr[i])
      return false;
  }
}
bool BIDS::CmdSender(const char *cmd, double *ret)
{
  if (cmd == NULL || ret == NULL)
    return false;

  int len = 0;
  char charr[CMD_LEN_MAX];

  len = CmdSender(cmd, charr, CMD_LEN_MAX);

  if (!HeaderCheck(charr, len))
    return false;

  *ret = 0.f;
  for (int i = 0; i < len; i++)
  {
    if (charr[i] == 'X')
    {
      *ret = strtod(&charr[i + 1], NULL);
      return true;
    }
    else if (cmd[i] != charr[i])
      return false;
  }
}
int BIDS::CmdSenderI(const char *cmd)
{
  if (cmd == NULL)
    return 0;

  int ret;
  CmdSender(cmd, &ret);
  return ret;
}
double BIDS::CmdSenderF(const char *cmd)
{
  if (cmd == NULL)
    return 0;

  double ret;
  CmdSender(cmd, &ret);
  return ret;
}
bool BIDS::IsEnable()
{
  return isEnable;
}

char *BIDS::cmdCAGet(char *ca, const int ca_len, const char cmdType, const int data_num)
{
  return cmdCAGet(ca, ca_len, cmdType, 0, data_num);
}
char *BIDS::cmdCAGet(char *ca, const int ca_len, const char cmdType, const char data_type, const int data_num)
{
  if (ca == NULL || ca_len <= 3)
    return NULL;

  if (data_type != 0)
    snprintf(ca, ca_len, "TR%c%c%d", cmdType, data_type, data_num);
  else
    snprintf(ca, ca_len, "TR%c%d", cmdType, data_num);

  return ca;
}

bool BIDS::HeaderCheck(char *ca, const int ca_len, const char cmdType = 0, const char data_type = 0)
{
  if (ca == NULL || ca_len <= 2)
    return false;

  char c[] = "TR\0\0\0";
  int len_min = HeaderSize;
  if (cmdType != 0) //needed cmdType check
  {
    if (data_type == 0) //not needed data_type check
    {
      len_min = HeaderSize + 1;
      c[HeaderSize] = cmdType;
    }
    else //needed data_type check
    {
      len_min = HeaderSize + 2;
      c[HeaderSize] = cmdType;
      c[HeaderSize + 1] = data_type;
    }
  }

  if (ca_len <= len_min)
    return false;

  return strncmp(ca, c, len_min) == 0;
}
bool BIDS::HeaderCheck(char *ca, const int ca_len, const char cmdType, const int data_num)
{
  return HeaderCheck(ca, ca_len, cmdType, 0, data_num);
}
bool BIDS::HeaderCheck(char *ca, const int ca_len, const char cmdType, const char data_type, const int data_num)
{
  if (ca == NULL || ca_len <= (HeaderSize + 3))
    return false;

  int ca_dnum = !data_num;
  int nPos = HeaderSize + 2;

  if (!HeaderCheck(ca, ca_len, cmdType, data_type))
    return false;

  if (data_type == 0)
    nPos--;

  ca_dnum = (int)strtol(&ca[nPos], NULL, 10);

  return data_num == ca_dnum;
}