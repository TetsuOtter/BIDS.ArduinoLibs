//TR.BIDS.lib.h
//Under the MIT License
//Copyright 2020 Tetsu Otter
#ifndef TR_BIDS_LIB_H
#define TR_BIDS_LIB_H

#ifndef AS_MAX
//internal AutoSend List Capacity
#define AS_MAX 100
#endif

#ifndef CMD_LEN_MAX
//Command length limit that this library can be handled
#define CMD_LEN_MAX 128
#endif

//the format of the AutoSend Actions function to be executed when associated data is received
typedef void (*AS_OnDataGot)(int, double);

typedef struct ASAction //AutoSend Action-associate structure
{
  char type;           //Data Type Character
  int data_num;        //Data Number
  AS_OnDataGot action; //Action that invoked when the specified data is receieved
} ASAction;

//BIDS Communication Support Class
class BIDS
{
private:
  bool isEnable = false;      //Is Enabled
  ASAction ASActions[AS_MAX]; //AutoSend Actions Record
  int Actions_count = 0;      //AutoSend Actions List Counter
  Stream *UsingSerial;        //Using Communication Interface

  const int HeaderSize = 2; //Size of BIDS String-Command's Header

  //Write the arg data to the location, specified in the arg
  void ASActoinsSet(ASAction *asacts, const char type, const int data_num, AS_OnDataGot act);
  void ASActoinsSet(ASAction *asactDst, ASAction *asactSrc);

public:
  char LastCMD[CMD_LEN_MAX]; //The string that is got in ASDataCheck function

  //Initialize the BIDS class
  //"ser" : Communication Interface
  BIDS(Stream *ser);
  //BIDS class destructor
  ~BIDS();

  //Request AutoSend Setting Add to internal (and external if needed) ASList
  //"type" : Requesting Data Type
  //"data_num" : Requesting Data Number
  //"act" : Address of the function that to be executed when receiving the specified data
  //returns : whether the request accepted
  bool AddAutoSend(char type, int data_num, AS_OnDataGot act = NULL);
  //Request AutoSend Setting Add to internal (and external if needed) ASList
  //"asa" : Requesting ASSetting
  //returns : whether the request accepted
  bool AddAutoSend(ASAction asa);

  //Request Removing ASSetting from internal (and external if needed) ASList
  //"type" : Requesting Data Type
  //"data_num" : Requesting Data Number
  //"act" : Address of the function that you set to be executed when receiving the specified data
  //returns : whether the request accepted
  bool RmvAutoSend(char type, int data_num, AS_OnDataGot act = NULL);
  //Request Removing ASSetting from internal (and external if needed) ASList
  //"asa" : Requesting ASSetting
  //returns : whether the request accepted
  bool RmvAutoSend(ASAction asa);

  //Check that data is coming to the buffer
  //"NonASCMDGot" : whether the got data is not in the internal ASList
  //returns : whether data reading successed
  bool ASDataCheck(bool *NonASCMDGot);

  //Send the specified command and get the responce
  //"cmd" : The command string you want to send
  //"ret" : The place to write the received command string
  //"retlen" : writeable length of "ret"
  //returns : The length of got command
  int CmdSender(const char *cmd, char *ret, int retlen);
  //Send the specified command and get the responce data in int
  //"cmd" : The command string you want to send
  //"ret" : The place to write the received data
  //returns : whether data reading successed
  bool CmdSender(const char *cmd, int *ret);
  //Send the specified command and get the responce data in double
  //"cmd" : The command string you want to send
  //"ret" : The place to write the received data
  //returns : whether data reading successed
  bool CmdSender(const char *cmd, double *ret);
  //Send the specified command and get the responce data in int
  //"cmd" : The command string you want to send
  //returns : Got data
  int CmdSenderI(const char *cmd);
  //Send the specified command and get the responce data in double
  //"cmd" : The command string you want to send
  //returns : Got Data
  double CmdSenderF(const char *cmd);

  //Get whether this class is opened
  //returns : Is Enabled
  bool IsEnable();

  //Constructs the command based on the specifying in arguments
  //"ca" : The palce to write the constructed command
  //"ca_len" : writeable length of "ca"
  //"cmdType" : Command Type Character
  //"data_type" : Data Type Character
  //"data_num" : Data Number
  //returns : If successed, returns "ca".  If not, returns NULL
  char *cmdCAGet(char *ca, const int ca_len, const char cmdType, const char data_type, const int data_num);
  //Constructs the command based on the specifying in arguments
  //"ca" : The palce to write the constructed command
  //"ca_len" : writeable length of "ca"
  //"cmdType" : Command Type Character
  //"data_num" : Data Number
  //returns : If successed, returns "ca".  If not, returns NULL
  char *cmdCAGet(char *ca, const int ca_len, const char cmdType, const int data_num);

  //Check the given string structure conforms to the BIDS standard.
  //"ca" : Command string to check
  //"ca_len" : Command string length (except for NULL char)
  //"cmdType" : Command Type Character to be used as condition
  //"data_type" : Data Type Character to be used as condition
  //returns : whether given command is same as given data info
  bool HeaderCheck(char *ca, const int ca_len, const char cmdType = 0, const char data_type = 0);
  //Check the given string structure conforms to the BIDS standard.
  //"ca" : Command string to check
  //"ca_len" : Command string length (except for NULL char)
  //"cmdType" : Command Type Character to be used as condition
  //"data_num" : Data Number to be used as condition
  //returns : whether given command is same as given data info
  bool HeaderCheck(char *ca, const int ca_len, const char cmdType, const int data_num);
  //Check the given string structure conforms to the BIDS standard.
  //"ca" : Command string to check
  //"ca_len" : Command string length (except for NULL char)
  //"cmdType" : Command Type Character to be used as condition
  //"data_type" : Data Type Character to be used as condition
  //"data_num" : Data Number to be used as condition
  //returns : whether given command is same as given data info
  bool HeaderCheck(char *ca, const int ca_len, const char cmdType, const char data_type, const int data_num);
};

#endif