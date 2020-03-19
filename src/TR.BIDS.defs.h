//TR.BIDS.defs.h
//under the CC0 License
//written by Tetsu Otter
#ifndef TR_BIDS_DEFS_H
#define TR_BIDS_DEFS_H

//Command Type Character define
#define CMD_VERSION 'V'      //Version Check Command
#define CMD_INFOREQ 'I'      //Info Request Command
#define CMD_AUTOSEND_ADD 'A' //AS Add Command
#define CMD_AUTOSEND_DEL 'D' //AS Delete Command
#define CMD_ERROR 'E'        //Error Notification

//Data Type Symbol Character define
#define DTYPE_ELAPD 'E'   //Elapse Data Type Symbpl Character
#define DTYPE_DOOR 'D'    //Door State Data Type Symbol Character
#define DTYPE_HANDPOS 'H' //Handle Position Data Type Symbol Character
#define DTYPE_CONSTD 'C'  //Constant Data Type Symbol Character

//Data Type : E (ElapD)
#define DNUM_ELAPD_DISTANCE 0   //(double) Current Distance [m]
#define DNUM_ELAPD_SPEED 1      //(float) Current Speed [km/h]
#define DNUM_ELAPD_TIME 2       //(int) Current Time Count from 0:00 [ms]
#define DNUM_ELAPD_BC_PRES 3    //(float) BC Pressure [kPa]
#define DNUM_ELAPD_MR_PRES 4    //(float) MR Pressure [kPa]
#define DNUM_ELAPD_ER_PRES 5    //(float) ER Pressure [kPa]
#define DNUM_ELAPD_BP_PRES 6    //(float) BP Pressure [kPa]
#define DNUM_ELAPD_SAP_PRES 7   //(float) SAP Pressure [kPa]
#define DNUM_ELAPD_CURRENT 8    //(float) Electric Current [A]
#define DNUM_ELAPD_VOLTAGE 9    //(float) Electric Voltage [V]
#define DNUM_ELAPD_TIME_HOUR 10 //(int)  Hour Part of current time
#define DNUM_ELAPD_TIME_MIN 11  //(int)  Minute Part of current time
#define DNUM_ELAPD_TIME_SEC 12  //(int)  Second Part of current time
#define DNUM_ELAPD_TIME_MSEC 13 //(int)  Milli-Second Part of current time

//Data Type : H (HandPos)
#define DNUM_HANDPOS_BRAKE 0    //(int) The position of brake handle
#define DNUM_HANDPOS_POWER 1    //(int) The position of power handle
#define DNUM_HANDPOS_REVERSER 2 //(int) The position of reverser handle
#define DNUM_HANDPOS_CONSTSPD 3 //(int) The state of const-speed button

//Data Type : C (Const-Data)
#define DNUM_CONSTD_BRAKE_COUNT 0 //(int) Position Count of Brake Handle
#define DNUM_CONSTD_POWER_COUNT 1 //(int) Position Count of Brake Handle
#define DNUM_CONSTD_ATSC_POS 2    //(int) ATS Check Position Data
#define DNUM_CONSTD_B67_POS 3     //(int) B67 Position Data
#define DNUM_CONSTD_CAR_COUNT 4   //(int) Car Count Data

#endif