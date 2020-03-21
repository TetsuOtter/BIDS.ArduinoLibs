# BIDS Communication Library for Arduino
Serial Interfaceを通じたBIDS中継ソフトとの通信を補助します。  
BIDS.ArduinoSketchesから移動したものです。

---
# examples
[スケッチ例に関しては、こちらをご覧ください。](./examples/README.md)

---
# TR.BIDS.libs.h
通信を補助する機能を定義しています。実装はTR.BIDS.libs.cppにあります。

## Constant Values
publicなもののみを扱います。privateなものはソースコードを直接参照してください。
### AS_MAX
初期値 : 100  
internal ASListのキャパシティを指定します。本ヘッダを読み込む前にdefineすることにより、任意の値に変更することが可能…だと思われます。

### CMD_LEN_MAX
初期値 : 128  
受信データの許容最大長を指定します。本ヘッダを読み込む前にdefineすることにより、任意の値に変更することが可能…だと思われます。

## Variables
publicなもののみを扱います。privateなものはソースコードを直接参照してください。
### char LastCMD[CMD_LEN_MAX]
ASDataCheckにて受信した生データを格納します。

## functions
publicなもののみを扱います。privateなものはソースコードを直接参照してください。

### BIDS(Stream *)
BIDSクラスを初期化します。引数には、予め通信が開始された状態のシリアルインターフェース(正確にはStreamクラス)へのポインタを渡します。  
通信が開始されていない状態での使用は想定されていません。

### bool AddAutoSend(char, int, AS_OnDataGot)
ライブラリ内部のAutoSend管理リストに、引数で指定したデータタイプ/番号および受信時動作を設定します。  
internal ASListに追加後、必要があればexternal ASListにAutoSendの要求を出します。

|Header|Command Type|Data Type|Data Number|Footer|
|-|-|-|-|-|
|TR|A|E|10|¥n|

上記Data Typeは必ず1文字の英字となり、これは第一引数で指定します。
上記Data Numberは正の整数であり、これは第二引数で指定します。

AutoSendでBIDS_Server等からデータが送られてきたときに、実行する処理を第三引数に指定します。  
ここで指定する処理はint型の第一引数およびdouble型の第二引数を受けるようにする必要があります。

例として、速度情報をAutoSendにより受信するプログラムを、以下に示します。なお、各種初期化処理は省略しています。

~~~
BIDS bids = BIDS(&Serial);
void setup()
{  
  bids.AddAutoSend('E', 1, func);
}
void func(int i, double d)
{
  //i : Got-data in int
  //d : Got-data in double
  //(any process)
}
~~~

返り値は、internal / external問わずASListに設定を追加できたかどうかです。

ASAction構造体で追加要素指定をすることも可能ですが、これに関しては引数を展開して再実行する実装をとっておりますので、説明は省略させていただきます。

### bool RmvAutoSend(char, int, AS_OnDataGot)
引数で指定された情報についてAutoSendの停止要求を出します。
第三引数にNULLを指定した場合、該当するデータ情報を持つすべてのASActionを削除します。

第一引数に、検索条件とするData Typeを指定します。これは省略できません。  
第二引数に、検索条件とするData Numberを指定します。これは省略できません。  
第三引数に、検索条件とする関数ポインタを指定します。これを省略あるいはNULL指定をした場合、条件なしとして検索が実行されます。

internal ASListより検索条件をもとに該当するASSettingを探し出し、該当するものが存在した場合はその要素を削除し、前に詰めたうえで次の合致要素を検索します。  
関数ポインタが条件として指定されていた場合、該当したASSetting以外に同じデータを要求するものが存在した場合はexternal ASListに対して削除要求を出しません。逆に、internal ASListの指定されたデータを要求するASSettingがすべて削除された場合に限り、external ASListに対して削除要求を出します。

返り値は、internal/external問わず削除に成功したかどうかです。

ASAction構造体で追加要素指定をすることも可能ですが、これに関しては引数を展開して再実行する実装をとっておりますので、説明は省略させていただきます。

### bool ASDataCheck(bool *)
バッファにデータが来ていないか確認し、来ているデータがASListに登録されているものであった場合には関連付けられた関数を実行します。

引数にはbool型変数へのポインタを指定し、これには「受信したデータがASListに登録されていないものであったか」が記録されます。

返り値は何かしらのデータの受信に成功したかどうかです。

なお、受信したデータはLastCMDに記録されます。

### int CmdSender(const char *, char *, int)
指定されたコマンドを送信し、帰ってきたデータを指定された場所に記録します。

第一引数には、送信するコマンドを指定します。  
第二引数には、受信したデータを記録する場所へのポインタを指定します。  
第三引数には、第二引数で指定した場所のキャパシティを指定します。

返り値は、読み取ったデータのサイズです。なお、何らかのエラーが発生した場合は負の数が返されます。

### bool CmdSender(const char *, int/double *)
指定されたコマンドを送信し、帰ってきたデータより情報を読み取り、指定された場所に記録します。

第一引数には、送信するコマンドを指定します。  
第二引数には、受信したデータを記録する場所へのポインタを指定します。

返り値は、処理に成功したかどうかです。

### int CmdSenderI / double CmdSenderF (const char *)
指定されたコマンドを送信し、帰ってきたデータより情報を読み取ります。

第一引数には、送信するコマンドを指定します。

返り値は、読み取った結果得られた数値です。

### bool IsEnable(void)
本クラスが有効状態にあるかどうかを返します。基本trueのみが返ります。

### char *cmdCAGet(char *, const int, const char, const char, const int)
引数で指定した情報をもとに、BIDS規格のコマンドを構成します。

第一引数には、コマンドを書き込む場所を指定します。  
第二引数には、第一引数で指定した場所のキャパシティを指定します。  
第三引数には、Command Type Characterを指定します。  
第四引数には、Data Type Characterを指定します。  
第五引数には、Data Numberを指定します。

返り値は、コマンドの構成に成功した場合は第一引数に指定したポインタです。失敗した場合はNULLが返されます。

### char *cmdCAGet(char *, const int, const char, const int)
引数で指定した情報をもとに、BIDS規格のコマンドを構成します。

第一引数には、コマンドを書き込む場所を指定します。  
第二引数には、第一引数で指定した場所のキャパシティを指定します。  
第三引数には、Command Type Characterを指定します。  
第四引数には、Data Numberを指定します。

返り値は、コマンドの構成に成功した場合は第一引数に指定したポインタです。失敗した場合はNULLが返されます。

### bool HeaderCheck(char *, const int, const char, const char)
与えられた文字列がBIDS規格において指定した条件に合致するものであるかどうかを確認します。

第一引数には、チェックする文字列が格納された場所へのポインタを指定します。  
第二引数には、チェックする文字列の長さを指定します。  
第三引数には、BIDS規格におけるCommand Type Characterにあたるものを指定します。この引数は省略可能であり、省略あるいはNULL文字指定にて、条件指定なしとして扱われます。  
第四引数には、BIDS規格におけるData Type Characterにあたるものを指定します。この引数は省略可能であり、省略あるいはNULL文字指定にて、条件指定なしとして扱われます。但し、第三引数にNULL文字を指定した場合、この引数での指定は無視され常に条件なしとして扱われます。

返り値は、与えられた文字列が条件に一致するものであるかどうかです。

### bool HeaderCheck(char *, const int, const char, const int)
与えられた文字列がBIDS規格において指定した条件に合致するものであるかどうかを確認します。

第一引数には、チェックする文字列が格納された場所へのポインタを指定します。  
第二引数には、チェックする文字列の長さを指定します。  
第三引数には、BIDS規格におけるCommand Type Characterにあたるものを指定します。  
第四引数には、BIDS規格におけるData Numberにあたるものを指定します。

返り値は、与えられた文字列が条件に一致するものであるかどうかです。

### bool HeaderCheck(char *, const int, const char, const char, const int)
与えられた文字列がBIDS規格において指定した条件に合致するものであるかどうかを確認します。

第一引数には、チェックする文字列が格納された場所へのポインタを指定します。  
第二引数には、チェックする文字列の長さを指定します。  
第三引数には、BIDS規格におけるCommand Type Characterにあたるものを指定します。  
第四引数には、BIDS規格におけるData Type Characterにあたるものを指定します。  
第五引数には、BIDS規格におけるData Numberにあたるものを指定します。

返り値は、与えられた文字列が条件に一致するものであるかどうかです。

---
# TR.BIDS.defs.h
コマンドを構成する際に便利になるよう、情報と数値等を関連付けています。

## Command Type Characters
|定義名|コマンド種類|
|-|-|
|CMD_VERSION|バージョン情報|
|CMD_INFOREQ|情報送信要求|
|CMD_AUTOSEND_ADD|AutoSend Listへの追加要求|
|CMD_AUTOSEND_DEL|AutoSend Listからの消去要求|
|CMD_ERROR|エラー発生を通知|

## Data Type Symbol Characters
|定義名|データ種類|
|-|-|
|DTYPE_ELAPD|Elapse Data|
|DTYPE_DOOR|ドア状態|
|DTYPE_HANDPOS|ハンドル位置情報|
|DTYPE_CONSTD|車両毎固定値情報|

## Data Numbers
|定義名|データ種類|単位|
|-|-|-|
|DNUM_ELAPD_DISTANCE|現在の車両位置情報|m|
|DNUM_ELAPD_SPEED|現在の車両速度|km/h|
|DNUM_ELAPD_TIME|BVE内での0時からの経過時間|ms|
|DNUM_ELAPD_BC_PRES|BC圧力|kPa|
|DNUM_ELAPD_MR_PRES|MR圧力|kPa|
|DNUM_ELAPD_ER_PRES|ER圧力|kPa|
|DNUM_ELAPD_BP_PERS|BP圧力|kPa|
|DNUM_ELAPD_SAP_PRES|SAP圧力|kPa|
|DNUM_ELAPD_CURRENT|電流値|A|
|DNUM_ELAPD_VOLTAGE|架線電圧(BIDS未実装)|V|
|DNUM_ELAPD_TIME_HOUR|内部時刻の「時」部分||
|DNUM_ELAPD_TIME_MIN|内部時刻の「分」部分||
|DNUM_ELAPD_TIME_SEC|内部時刻の「秒」部分||
|DNUM_ELAPD_TIME_MSEC|内部時刻の「ミリ秒」部分||
|DNUM_HANDPOS_BRAKE|ブレーキハンドル位置情報||
|DNUM_HANDPOS_POWER|力行ハンドル位置情報||
|DNUM_HANDPOS_REVERSER|レバーサー位置情報||
|DNUM_HANDPOS_CONSTSPD|定速ボタン状態(BIDS未実装)||
|DNUM_CONSTD_BRAKE_COUNT|ブレーキハンドルの位置設定可能段数||
|DNUM_CONSTD_POWER_COUNT|力行ハンドルの位置設定可能段数||
|DNUM_CONSTD_ATSC_POS|ATS確認に必要な最低段数||
|DNUM_CONSTD_B67_POS|67度に相当するブレーキハンドル位置||
|DNUM_CONSTD_CAR_COUNT|編成車両数|両|