//TR.BIDS.lib.h
//Under the MIT License
//Copyright 2020 Tetsu Otter
#ifndef TR_BIDS_LIB_H
#define TR_BIDS_LIB_H

#ifndef AS_MAX
#define AS_MAX 100
#endif

#ifndef CMD_LEN_MAX
#define CMD_LEN_MAX 128
#endif

typedef void (*AS_OnDataGot)(int, double);
typedef struct ASAction
{
  char type;
  int data_num;
  AS_OnDataGot action;
} ASAction;

class BIDS
{
private:
  bool isEnable = false;
  ASAction ASActions[AS_MAX];
  int Actions_count = 0;
  Stream *UsingSerial;

  const int HeaderSize = 2;

  void ASActoinsSet(ASAction *asacts, const char type, const int data_num, AS_OnDataGot act);
  void ASActoinsSet(ASAction *asactDst, ASAction *asactSrc);

public:
  char LastCMD[CMD_LEN_MAX];

  BIDS(Stream *ser);
  ~BIDS();
  bool AddAutoSend(char type, int data_num, AS_OnDataGot act);
  bool AddAutoSend(ASAction asa);
  bool RmvAutoSend(char type, int data_num, AS_OnDataGot act);
  bool RmvAutoSend(char type, int data_num);
  bool RmvAutoSend(ASAction asa);
  bool ASDataCheck(bool *NonASCMDGot);

  int CmdSender(const char *cmd, char *ret, int retlen);
  bool CmdSender(const char *cmd, int *ret);
  bool CmdSender(const char *cmd, double *ret);
  int CmdSenderI(const char *cmd);
  double CmdSenderF(const char *cmd);

  bool IsEnable();

  char *cmdCAGet(char *ca, const int ca_len, const char cmdType, const char data_type, const int data_num);
  char *cmdCAGet(char *ca, const int ca_len, const char cmdType, const int data_num);
  bool HeaderCheck(char *ca, const int ca_len, const char cmdType = 0, const char data_type = 0);
  bool HeaderCheck(char *ca, const int ca_len, const char cmdType, const int data_num);
  bool HeaderCheck(char *ca, const int ca_len, const char cmdType, const char data_type, const int data_num);
};

#endif