# BIDS Communication Library for Arduino
Serial Interfaceを通じたBIDS中継ソフトとの通信を補助します。  
BIDS.ArduinoSketchesから移動したものです。

## functions
### BIDS(*Stream)
BIDSクラスを初期化します。引数には、予め通信が開始された状態のシリアルインターフェース(正確にはStreamクラス)へのポインタを渡します。  
通信が開始されていない状態での使用は想定されていません。

### AddAutoSend(char, int, AS_OnDataGot)
ライブラリ内部のAutoSend管理リストに、引数で指定したデータタイプ/番号および受信時動作を設定します。

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

### RmvAutoSend(char, int, AS_OnDataGot)
引数で指定された情報についてAutoSendの停止要求を出します。停止要求が受理された場合、内部AutoSend管理リストより該当する情報を削除します。

返り値は、削除に成功したかどうかです。

以下準備中
