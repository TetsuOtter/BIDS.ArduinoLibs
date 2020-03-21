//TR.BIDS.lib.cpp
//Under the MIT License
//Copyright 2020 Tetsu Otter

#define TIMEOUT_NUM 10000

#include <Arduino.h>
#include <math.h>
#include <stdlib.h>
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
  if (Actions_count >= Actions_MAX)
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
  if (!IsSameDataAlready || CmdSenderI(("TRA" + String(type) + String(data_num)).c_str()) != 0)
    return false;

  ASActoinsSet(&ASActions[Actions_count++], type, data_num, act);

  return true;
}
bool BIDS::AddAutoSend(ASAction asa)
{
  return AddAutoSend(asa.type, asa.data_num, asa.action);
}

bool BIDS::RmvAutoSend(char type, int data_num)
{
  RmvAutoSend(type, data_num, NULL);
}
bool BIDS::RmvAutoSend(char type, int data_num, AS_OnDataGot act)
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
  if (IsFound && ActNotSameSkipCount == 0)
    CmdSenderI(("TRD" + String(type) + String(data_num)).c_str());
  return IsFound;
}
bool BIDS::RmvAutoSend(ASAction asa)
{
  return RmvAutoSend(asa.type, asa.data_num, asa.action);
}

bool BIDS::ASDataCheck(bool *NonASCMDGot)
{
  *NonASCMDGot = false;
  if ((*UsingSerial).available() <= 0)
    return false;

  ZeroFill(LastCMD, LastCMD_len);

  int len = (*UsingSerial).readBytesUntil('\n', LastCMD, LastCMD_len);

  if (len <= 0)
    return false;

  if (len <= 4 || LastCMD[0] != 'T' || LastCMD[1] != 'R' || LastCMD[2] != 'I')
  {
    *NonASCMDGot = true;
    return false;
  }

  int data_start_pos = 0;
  for (int i = 4; i < len; i++)
  {
    if (LastCMD[i] == 'X')
    {
      data_start_pos = i + 1;
      break;
    }
  }
  if (data_start_pos == 0)
  {
    *NonASCMDGot = true;
    return false;
  }

  int dnum = atoi(&LastCMD[4]);
  int valI = atoi(&LastCMD[data_start_pos]);
  double valF = atof(&LastCMD[data_start_pos]);
  bool Done = false;
  for (int i = 0; i < Actions_count; i++)
  {
    if (LastCMD[3] == ASActions[i].type && dnum == ASActions[i].data_num)
    {
      ASActions[i].action(valI, valF);
      Done = true;
    }
  }

  return Done;
}

int BIDS::CmdSender(const char *cmd, char *ret, int retlen)
{
  (*UsingSerial).println(cmd);
  int w = 0;
  while ((*UsingSerial).available() <= 0)
  {
    if (w >= TIMEOUT_NUM)
      return 0;
    delay(1);
    w++;
  }
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
  int len = 0;
  char charr[100];
  len = CmdSender(cmd, charr, 100);
  if (len <= 3 || charr[0] != 'T' || charr[1] != 'R')
    return false;

  *ret = 0;
  for (int i = 0; i < len; i++)
  {
    if (charr[i] == 'X')
    {
      *ret = atoi(&charr[i + 1]);
      return true;
    }
    else if (cmd[i] != charr[i])
      return false;
  }
}
bool BIDS::CmdSender(const char *cmd, double *ret)
{
  int len = 0;
  char charr[100];
  len = CmdSender(cmd, charr, 100);
  if (len <= 3 || charr[0] != 'T' || charr[1] != 'R')
    return false;

  *ret = 0;
  for (int i = 0; i < len; i++)
  {
    if (charr[i] == 'X')
    {
      *ret = atof(&charr[i + 1]);
      return true;
    }
    else if (cmd[i] != charr[i])
      return false;
  }
}
int BIDS::CmdSenderI(const char *cmd)
{
  int ret;
  CmdSender(cmd, &ret);
  return ret;
}
double BIDS::CmdSenderF(const char *cmd)
{
  double ret;
  CmdSender(cmd, &ret);
  return ret;
}
bool BIDS::IsEnable()
{
  return isEnable;
}
