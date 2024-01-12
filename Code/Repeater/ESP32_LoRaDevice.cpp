#define ENABLE_RSSI true
#include <WiFi.h>
#include "Arduino.h"
#include "LoRa_E22.h"
#include <TinyGPSPlus.h>
#include <SoftwareSerial.h>
#include "DHT.h"
#include <SPI.h> 
#include <Wire.h> 
#include <Adafruit_GFX.h> 
#include <Adafruit_SSD1306.h> 
#include <string>
#include <Keypad.h>
#include <ThreeWire.h>  
#include <RtcDS1302.h>
 
ThreeWire myWire(4,5,2); // IO, SCLK, CE  //对应接到ESP32的IO号
RtcDS1302<ThreeWire> Rtc(myWire);

#define SCREEN_WIDTH 128 // OLED 寬度像素
#define SCREEN_HEIGHT 64 // OLED 高度像素
 // 引用Keypad程式庫
#define DHTPIN 14    // Digital pin connected to the DHT sensor
#define SOSB 15
#define R 27
#define G 26
#define B 25
#define buzzer 33
#define func1 32
#define func2 35
#define func3 34
#define func4 39
#define DHTTYPE DHT22   // DHT 22  (AM2302), AM2321
DHT dht(DHTPIN, DHTTYPE);

#define OLED_RESET -1
// ---------- esp32 pins --------------
LoRa_E22 e22ttl(&Serial2, 18, 23, 19); //  RX AUX M0 M1

//LoRa_E22 e22ttl(&Serial2, 22, 4, 18, 21, 19, UART_BPS_RATE_9600); //  esp32 RX <-- e22 TX, esp32 TX --> e22 RX AUX M0 M1
// -------------------------------------

#define SERIAL_BAUD 9600

HardwareSerial mserial(1);//声明串口1

const char *ssid = "LoRa-ESP32(Addison)";
const char *password = "00000000";

WiFiServer server;

IPAddress local_IP(192, 168, 1, 1);  
IPAddress gateway(192, 168, 1, 1);
IPAddress subnet(255, 255, 255, 0);
TinyGPSPlus gps;
String name = "Addison";

Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

short delayTime = 1000; //使用者輸入延遲時間

// HardwareSerial gps_serial(2); // RX, TX pin

void testdrawstyles(void) {

  display.clearDisplay();
  display.setTextSize(3);          // 設定文字大小
  display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
  display.setCursor(0,0);          // 設定起始座標
  display.print("LoRa\n  ESP32");     // 要顯示的字串


  display.display();               // 要有這行才會把文字顯示出來
  delay(1000);
}
unsigned long previousMillis = 0;
unsigned long previousMillis2 = 0;
const long interval = 2000;
unsigned long soscheck = 0;
int screen = 0;
float dist = 0.0;
String x= "0";
String y= "0";
String atti = "0";
String whour = "0";
String wmin = "0";
String wsec = "0";
String wsat = "0";
String wlat = "0";
String wlng = "0";
bool sure = false;
bool gpsok = false;
unsigned long lastupdate = 0;
int autohelp = 0;
void Sound(int a){
    if(a==0){ //notify status
        digitalWrite(buzzer, HIGH);
        delay(120);
        digitalWrite(buzzer, LOW);
        delay(60);
        digitalWrite(buzzer, HIGH);
        delay(120);
        digitalWrite(buzzer, LOW);

    }
    else if(a==1){ //good status
        digitalWrite(buzzer, HIGH);
        delay(300);
        digitalWrite(buzzer, LOW);
    }
    else if(a==2){ //bad status
        digitalWrite(buzzer, HIGH);
        delay(100);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(100);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(100);
        digitalWrite(buzzer, LOW);
        delay(50);
    }
    else if(a==3){ //SOS
        //morse code beep
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(120);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(120);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(120);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);
        delay(50);
        digitalWrite(buzzer, HIGH);
        delay(50);
        digitalWrite(buzzer, LOW);

    }
}
int timeupdate(){
    
    
//    String sdate = String(gps.date.value());
//    String stime = String(gps.time.value());
//    String smin = String(gps.time.minute());
//    String ssec = String(gps.time.second());
//    if(millis()-lastupdate>30000){
//      lastupdate = millis();
//    if (smin != String("0") and ssec != String("0")) {
//      
//      RtcDateTime compiled = RtcDateTime(__sdate__, __stime__); 
//      printDateTime(compiled);  //打印时间
//        Serial.println("Correcting the time...");
//        if (Rtc.GetIsWriteProtected())    //检查是不是写保护了
//    {
//        Serial.println("RTC was write protected, enabling writing now");
//        Rtc.SetIsWriteProtected(false);
//    }
// 
//    if (!Rtc.GetIsRunning())  //检查是不是可读
//    {
//        Serial.println("RTC was not actively running, starting now");
//        Rtc.SetIsRunning(true);
//    }
// 
//    
//    else if (now == compiled) 
//    {
//        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
//    }
//    }
//    else {
//      Serial.println("no change due to GPS!");
//    }
//
//    }
//    else{
//        Serial.println("no change due to time!");
//    }
//Serial.println("ok");
}
int gotsos(String rev){
    Sound(3);
    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);
    display.clearDisplay();
    display.setTextSize(2);          // 設定文字大小
    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
    display.setCursor(0,0);          // 設定起始座標
    display.print("SOME ONE NEED\nHELP! SOS!\nCAL 112or911"); 
    delay(2000);    
    display.clearDisplay();
    display.setTextSize(2);          // 設定文字大小
    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
    display.setCursor(0,0);          // 設定起始座標
    display.print(rev.substring(0,30));
    delay(3000);
    display.clearDisplay();
    display.setTextSize(2);          // 設定文字大小
    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
    display.setCursor(0,0);          // 設定起始座標
    display.print(rev.substring(30,60));
    delay(3000);
    display.clearDisplay();
    display.setTextSize(2);          // 設定文字大小
    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
    display.setCursor(0,0);          // 設定起始座標
    display.print(rev.substring(60,90));
    delay(3000);
    Sound(3);
    digitalWrite(R, LOW);




  display.display();               // 要有這行才會把文字顯示出來
  delay(1000);


}
int SOS(){
    autohelp +=1;

    digitalWrite(R, HIGH);
    digitalWrite(G, LOW);
    digitalWrite(B, LOW);

    if(autohelp < 5){
      
    int ti = 0;
    display.display();
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    RtcDateTime now = Rtc.GetDateTime();
    String currenttime = String(now.Hour())+":"+String(now.Minute())+":"+String(now.Second()); 
    String x=(String(gps.location.lat(),6));
    String y=(String(gps.location.lng(),6));
    String sat = (String(gps.satellites.value()));
    String whour = (String(gps.time.hour()));
    String wmin = (String(gps.time.minute()));
    String wsec = String(gps.time.second());
    String wlat = (String(gps.location.lat(),6));
    String wlng = (String(gps.location.lng(),6));
    String wsat = (String(gps.satellites.value()));
    String atti = (String(gps.altitude.meters()));
    String input = String("!")+String(name)+":SOS!!!緊急求救/板載GPS:經度:"+String(wlat)+",緯度:"+String(wlng)+",接收到衛星數:"+String(wsat)+",海拔:"+String(atti)+",溫度:"+String(t)+",濕度:"+String(h)+"T:"+String(currenttime) ; 
    ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
    delay(500);
    ResponseStatus rp = e22ttl.sendBroadcastFixedMessage(70, input);
    delay(500);
    ResponseStatus re = e22ttl.sendBroadcastFixedMessage(70, input);
    Serial.println("已發送:"+String(input));
    display.clearDisplay();
    display.setTextSize(3);          // 設定文字大小
    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
    display.setCursor(0,0);          // 設定起始座標
    display.print("SOS \n  SENT!");     // 要顯示的字串
    display.display();               // 要有這行才會把文字顯示出來
    Sound(3);
    // if(String(rs.getResponseDescription()) == "success"){
    // display.clearDisplay();
    // display.setTextSize(3);          // 設定文字大小
    // display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
    // display.setCursor(0,0);          // 設定起始座標
    // display.print("SOS \n  SENT");     // 要顯示的字串
    // display.display();               // 要有這行才會把文字顯示出來
    // Sound(3);

    // }
    // else if(String(rs.getResponseDescription()) != "success" ){
    //     while(String(rs.getResponseDescription()) != "success" or ti <= 5){
    //         ti +=1;
    //         display.clearDisplay();
    //         display.setTextSize(3);          // 設定文字大小
    //         display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
    //         display.setCursor(0,0);          // 設定起始座標
    //         display.print("SOS RE\ntime: "+String(ti));     // 要顯示的字串
    //         display.display();               // 要有這行才會把文字顯示出來
    //         delay(1000);
    //         rs = e22ttl.sendBroadcastFixedMessage(70, input);
    //         Serial.println("已發送:"+String(input));
    //     }
        // display.clearDisplay();
        // display.setTextSize(3);          // 設定文字大小
        // display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
        // display.setCursor(0,0);          // 設定起始座標
        // display.print("SOS \n  SENT"); 
        // display.display();               // 要有這行才會把文字顯示出來
        // Sound(3);
        // }
    }
    else if(autohelp >= 5){
        unsigned long lastTime = 0;
        int mode_oled =0;
        digitalWrite(R, HIGH);
        digitalWrite(buzzer, HIGH); //鎖死 buzzer 跟 LED
        RtcDateTime now22 = Rtc.GetDateTime();
        String START = String(now22.Year())+"/"+String(now22.Month())+"/"+String(now22.Day())+","+String(now22.Hour())+":"+String(now22.Minute())+":"+String(now22.Second());
        while(1){
            
            if (millis() - lastTime > 2000) {
                lastTime = millis();
                if(mode_oled == 0){
                    
                    mode_oled = 1;
                    display.clearDisplay();
                    display.setTextSize(3);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                    display.print("AUTO \n SOS!!!"); 
                    display.display(); 

                    float h = dht.readHumidity();
                    // Read temperature as Celsius (the default)
                    float t = dht.readTemperature();
                    RtcDateTime now = Rtc.GetDateTime();
                    String currenttime = String(now.Hour())+":"+String(now.Minute())+":"+String(now.Second());
                    String input = String("V!!!")+ "{" +String(name)+","+ String(currenttime)+ "},{"+String(wlat)+ ","+String(wlng)+","+String(wsat)+","+String(atti)+"},{"+String(h)+","+String(t)+"},{START AT:"+String(START)+"}"; 
                    ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
                    
                }
                else if(mode_oled == 1){
                    mode_oled = 2;
                    display.clearDisplay();
                    display.setTextSize(2);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                    display.print("lng:"+ String(wlat)+"\nlat:"+String(wlng)+"\nSat:"+String(wsat)+" Alt:"+String(atti)); 
                    display.display();

                }
                else if(mode_oled == 2){
                    mode_oled = 0;
                  
                    display.clearDisplay();
                    display.setTextSize(3);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                      float h = dht.readHumidity();
                    // Read temperature as Celsius (the default)
                    float t = dht.readTemperature();
                    display.print(String(t)+"'C\n"+String(h)+"%"); 
                    display.display(); 
                }
            
                     
            }
            
        }
    }
        digitalWrite(R, LOW);
                
}
int msg_mode = 0;
int qui = 0;
void setup()
{
    pinMode(func1, INPUT);
    // pinMode(func2, INPUT);
    // pinMode(func3, INPUT);
    // pinMode(func4, INPUT);
    mserial.begin(SERIAL_BAUD,SERIAL_8N1,12,13);
    Serial.begin(115200);
    Serial.println();
    dht.begin();
  
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // 一般1306 OLED的位址都是0x3C
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
    }

    display.clearDisplay(); // 清除畫面
    testdrawstyles();    // 測試文字
    delay(1000);

    
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(local_IP,gateway,subnet);
    WiFi.softAP(ssid, password);
    Serial.print("AP IP address: ");
    Serial.println(WiFi.softAPIP());
    /*
    WiFi.mode(WIFI_STA);
    WiFi.setSleep(false);
    WiFi.begin(ssid, password);

    
    while (WiFi.status() != WL_CONNECTED)  
    {
        delay(500);
        Serial.print(".");
    }
    Serial.println("Connected");
    Serial.print("IP Address:");
    Serial.println(WiFi.localIP());
*/

    server.begin(1);  
    e22ttl.begin();
    ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, name);
  // Check If there is some problem of succesfully send
    Serial.println("發送狀態:"+rs.getResponseDescription());
    Rtc.Begin();  //用管脚声明来初始化RTC
 
   
 
 
    if (!Rtc.GetIsRunning())  //检查是不是可读
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
 
    RtcDateTime now = Rtc.GetDateTime(); 

    pinMode(R, OUTPUT);
    pinMode(G, OUTPUT);
    digitalWrite(G, HIGH);
    delay(300);
    digitalWrite(G, LOW);
    delay(100);
    digitalWrite(G, HIGH);
    delay(300);
    digitalWrite(G, LOW);
    delay(100);
    digitalWrite(G, HIGH);
    delay(300);
    digitalWrite(G, LOW);
 
    pinMode(B, OUTPUT);
    pinMode(buzzer, OUTPUT);
    pinMode(SOSB, INPUT);
    Sound(0);
    Serial.begin(9600);
    
    Serial.print("compiled: ");
    Serial.print(__DATE__); //打印系统日期
    Serial.println(__TIME__); //打印系统时间
 
    RtcDateTime compiled = RtcDateTime(__DATE__,__TIME__); //将系统时间声明一个时间结构
    Serial.println(compiled);  //打印时间
    Serial.println();
 
    if (!Rtc.IsDateTimeValid())   //读RTC时间，如果没读到，就是电池没电了
    {
        // Common Causes:
        //    1) first time you ran and the device wasn't running yet
        //    2) the battery on the device is low or even missing
 
        Serial.println("RTC lost confidence in the DateTime!");
        Rtc.SetDateTime(compiled);    //设置系统时间为当前时间
    }
 
    if (Rtc.GetIsWriteProtected())    //检查是不是写保护了
    {
        Serial.println("RTC was write protected, enabling writing now");
        Rtc.SetIsWriteProtected(false);
    }
 
    if (!Rtc.GetIsRunning())  //检查是不是可读
    {
        Serial.println("RTC was not actively running, starting now");
        Rtc.SetIsRunning(true);
    }
 
//    RtcDateTime now = Rtc.GetDateTime();  //得到RTC的时间
    if (now < compiled) //如果读出的时间早于系统时间，则重新设置RTC时间
    {
        Serial.println("RTC is older than compile time!  (Updating DateTime)");
        Rtc.SetDateTime(compiled);
    }
    else if (now > compiled) //如果读出的时间晚于系统时间，则重新设置RTC时间
    {
        Serial.println("RTC is newer than compile time. (this is expected)");
    }
    else if (now == compiled) 
    {
        Serial.println("RTC is the same as compile time! (not expected but all is fine)");
    }
}

void loop()
{    
    digitalWrite(G, HIGH);
    String x=(String(gps.location.lat(),6));
    String y=(String(gps.location.lng(),6));
    String sat = (String(gps.satellites.value()));
    String whour = (String(gps.time.hour()));
    String wmin = (String(gps.time.minute()));
    String wsec = String(gps.time.second());
    String wsat = String(gps.satellites.value());
    String wlat = String(gps.location.lat(),6);
    String wlng = String(gps.location.lng(),6);
    String atti = String(gps.altitude.meters());
    String speed = String(gps.speed.kmph());
    String course = String(gps.course.deg());
    String date = String(gps.date.value());
    
    
    // Serial.print(x);
    // Serial.print(",");
    // Serial.print(y);
    // Serial.print(",");
    // Serial.print(wsat);
    // Serial.print(",");
    // Serial.print(whour);
    // Serial.print(":");
    // Serial.print(wmin);
    // Serial.print(":");
    // Serial.print(wsec);


    timeupdate();
    float h = dht.readHumidity();
    // Read temperature as Celsius (the default)
    float t = dht.readTemperature();
    String input;
    WiFiClient client = server.available();
    //SOS button INDEPENDENCE
    if(digitalRead(SOSB)==HIGH){
                if(sure == true and millis()-soscheck>=5000){
                    
                    if(digitalRead(SOSB) == HIGH){
                        SOS();
                        sure = false;
                    }
                    else{
                        sure = false;
                         
                    }
                }
                else if(sure == true and millis()-soscheck<5000){
                    sure = true;
                    display.clearDisplay();
                    display.setTextSize(3);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                    display.print("SOS MODE\npress5s");     // 要顯示的字串
                    display.display(); 
                    display.clearDisplay();
                    
                    
                }
                else{
                    sure = true;
                    soscheck = millis();
                }
               
                

            
            }
    
        unsigned long currentMillis = millis();
                                if (currentMillis - previousMillis >= interval) {
                                    previousMillis = currentMillis;
                                    if(screen == 0){
                                    screen = 1;
                                    display.clearDisplay();
                                    display.setTextSize(4);          // 設定文字大小
                                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                    display.setCursor(0,0);          // 設定起始座標
                                    RtcDateTime now2 = Rtc.GetDateTime();
                                    display.print(String(now2.Hour())+":"+String(now2.Minute())+":"+String(now2.Second()));     // 要顯示的字串
                                    //String(whour)+" : "+String(wmin)+"  sat:"+String(wsat)+"   GPS:" +String(b)+ "Pack:"+ String(count)+" "+String(t)+"C "
                                    display.display(); 
                                }
                                else if(screen == 1){
                                    screen = 2;
                                    display.clearDisplay();
                                    display.setTextSize(3);          // 設定文字大小
                                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                    display.setCursor(0,0);          // 設定起始座標
                                    display.print("CNT:" +String("OFF")+"\nSat:"+String(wsat));     // 要顯示的字串
                                    display.display(); 
                                }
                                else if(screen == 2){
                                        screen = 3;
                                        display.clearDisplay();
                                        display.setTextSize(2);          // 設定文字大小
                                        display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                        display.setCursor(0,0);          // 設定起始座標
                                        display.print("lng:"+ String(wlat)+"\nlat:"+String(wlng));     // 要顯示的字串
                                        display.display(); 
                                    }
                                else if(screen ==3){
                                        screen = 4;
                                        display.clearDisplay();
                                        display.setTextSize(3);          // 設定文字大小
                                        display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                        display.setCursor(0,0);          // 設定起始座標
                                        display.print(String(t)+"'C\n"+String(h)+"%");     // 要顯示的字串
                                        display.display(); 
                                    }
                                else if(screen == 4){
                                        screen = 0;
                                        display.clearDisplay();
                                        display.setTextSize(2);          // 設定文字大小
                                        display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                        display.setCursor(0,0);          // 設定起始座標
                                        //方位角度換算英文符號
//                                        String dir = "";
//                                        if(course >= 0.0 && course < 22.5){
//                                            dir = "N";
//                                        }
//                                        else if(course >= 22.5 && course < 67.5){
//                                            dir = "NE";
//                                        }
//                                        else if(course >= 67.5 && course < 112.5){
//                                            dir = "E";
//                                        }
//                                        else if(course >= 112.5 && course < 157.5){
//                                            dir = "SE";
//                                        }
//                                        else if(course >= 157.5 && course < 202.5){
//                                            dir = "S";
//                                        }
//                                        else if(course >= 202.5 && course < 247.5){
//                                            dir = "SW";
//                                        }
//                                        else if(course >= 247.5 && course < 292.5){
//                                            dir = "W";
//                                        }
//                                        else if(course >= 292.5 && course < 337.5){
//                                            dir = "NW";
//                                        }
//                                        else if(course >= 337.5 && course < 360.0){
//                                            dir = "N";
//                                        }
                                        
                                        display.print("Speed:"+String(speed)+" KM\n"+"Course:"+String(course)+" deg\n"+String(date));     // 要顯示的字串
                                        display.display();

                                }
                                

    }
    if (e22ttl.available()>1) {
            

            ResponseContainer rc = e22ttl.receiveMessage();
            Serial.println(rc.data);
            String chw = String(rc.data).substring(3,rc.data.length());
            if(String(chw[0])==String("#")){
                Sound(0);
                digitalWrite(B, HIGH);
                display.clearDisplay();
                display.setTextSize(3);          // 設定文字大小
                display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                display.setCursor(0,0);          // 設定起始座標
                display.print("New Msg");     // 要顯示的字串
                display.display();
                delay(800);
                display.clearDisplay();
                display.setTextSize(3);          // 設定文字大小
                display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                display.setCursor(0,0);          // 設定起始座標
                display.print(String(rc.data).substring(4,rc.data.length()));     // 要顯示的字串
                display.display();
                delay(2000);
                digitalWrite(B, LOW);
            }
            else if(String(chw[0])==String("!")){
                gotsos(chw);
            }
            }

        
        if(digitalRead(func1)){
            digitalWrite(R, LOW);
            digitalWrite(G, LOW);
            digitalWrite(B, LOW);
            Sound(0);
            digitalWrite(B, HIGH);
            display.clearDisplay();
            display.setTextSize(3);          // 設定文字大小
            display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
            display.setCursor(0,0);          // 設定起始座標
            display.print("Quick\nMsgMODE");     // 要顯示的字串
            display.display();
            delay(1000);
            unsigned long lastTime2 = 0;
            qui = 0;
            digitalWrite(B,HIGH);
            msg_mode = 0;
            while(digitalRead(func1)!=HIGH ){
                
        
                RtcDateTime now22 = Rtc.GetDateTime();
                String START = String(now22.Year())+"/"+String(now22.Month())+"/"+String(now22.Day())+","+String(now22.Hour())+":"+String(now22.Minute())+":"+String(now22.Second());
       
                if (millis() - lastTime2 > 2000) {
                lastTime2 = millis();
                if(msg_mode == 3){
                    msg_mode = 0;
                    qui+=1;
                    display.clearDisplay();
                    display.setTextSize(3);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                    display.print("DATA\n    SMS");     // 要顯示的字串
                    display.display();
            
                    
                }
                else if(msg_mode == 0){
                    msg_mode = 1;
                    qui+=1;
                    display.clearDisplay();
                    display.setTextSize(3);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                    display.print("CALL ME\n    SMS");     // 要顯示的字串
                    display.display();

                }
                else if(msg_mode == 1){
                    msg_mode = 2;
                    qui+=1;
                    display.clearDisplay();
                    display.setTextSize(3);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                      float h = dht.readHumidity();
                    // Read temperature as Celsius (the default)
                    float t = dht.readTemperature();
                    display.print("FINE\n    SMS"); 
                    display.display(); 
                }
                else if(msg_mode == 2){
                    msg_mode = 3;
                    qui+=1;
                    display.clearDisplay();
                    display.setTextSize(3);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                    display.print("OK\n    SMS");     // 要顯示的字串
                    display.display();
                }
            
                     
            }
            }
            
            }
            if(msg_mode == 0){
            display.clearDisplay();
            display.setTextSize(2);          // 設定文字大小
            display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
            display.setCursor(0,0);          // 設定起始座標
            display.print("DATA\nSMS sent!");     // 要顯示的字串
            display.display();
            delay(500);
            float h = dht.readHumidity();
              // Read temperature as Celsius (the default)
            float t = dht.readTemperature();
            RtcDateTime now = Rtc.GetDateTime();
            String currenttime = String(now.Hour())+":"+String(now.Minute())+":"+String(now.Second()); 
            input ="#"+String(name)+":DATA/板載GPS:經度:"+String(wlat)+",緯度:"+String(wlng)+",接收到衛星數:"+String(wsat)+",海拔:"+String(atti)+",溫度:"+String(t)+",濕度:"+String(h)+"T:"+String(currenttime) ; 
            ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
            Serial.println("已發送:"+String(input));
            delay(2000);  
            digitalWrite(B, LOW);  
        }
        else if(msg_mode == 1){
            digitalWrite(R, LOW);
            digitalWrite(G, LOW);
            digitalWrite(B, LOW);
            Sound(0);
            digitalWrite(B, HIGH);
            display.clearDisplay();
            display.setTextSize(2);          // 設定文字大小
            display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
            display.setCursor(0,0);          // 設定起始座標
            display.print("CALL ME\nSMS sent!");     // 要顯示的字串
            display.display();
            delay(500);
            RtcDateTime now = Rtc.GetDateTime();
            String currenttime = String(now.Hour())+":"+String(now.Minute())+":"+String(now.Second()); 
            input ="#"+String(name)+":CALL ME"+" T:"+String(currenttime) ; 
            ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
            Serial.println("已發送:"+String(input));
            delay(2000);  
            digitalWrite(B, LOW);  
        }
        else if(msg_mode == 2){
            digitalWrite(R, LOW);
            digitalWrite(G, LOW);
            digitalWrite(B, LOW);
            Sound(0);
            digitalWrite(B, HIGH);
            display.clearDisplay();
            display.setTextSize(2);          // 設定文字大小
            display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
            display.setCursor(0,0);          // 設定起始座標
            display.print("FINE\nSMS sent!");     // 要顯示的字串
            display.display();
            delay(500);
            RtcDateTime now = Rtc.GetDateTime();
            String currenttime = String(now.Hour())+":"+String(now.Minute())+":"+String(now.Second()); 
            input ="#"+String(name)+":FINE"+" T:"+String(currenttime) ; 
            ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
            Serial.println("已發送:"+String(input));
            delay(2000);  
            digitalWrite(B, LOW);  
        }
        else if(msg_mode == 3){
            digitalWrite(R, LOW);
            digitalWrite(G, LOW);
            digitalWrite(B, LOW);
            Sound(0);
            digitalWrite(B, HIGH);
            display.clearDisplay();
            display.setTextSize(2);          // 設定文字大小
            display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
            display.setCursor(0,0); 
            display.print("OK\nSMS sent!");     // 要顯示的字串
            display.display();
            delay(500);
            RtcDateTime now = Rtc.GetDateTime();
            String currenttime = String(now.Hour())+":"+String(now.Minute())+":"+String(now.Second());  
            input ="#"+String(name)+":OK"+" T:"+String(currenttime) ; 
            ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
            Serial.println("已發送:"+String(input));
            delay(2000);  
            digitalWrite(B, LOW);          

    
        }
        msg_mode =-1;
    

    if (client)
    {   
        digitalWrite(G, HIGH);
        digitalWrite(B, HIGH);
        Sound(1);
        sure = false;
        Serial.println("==connection built==");
        while (client.connected())
        {
            timeupdate();
            if(Serial.available()){
            String a = Serial.readString();
              client.print("server: " + a); 
              Serial.println("server: " + a);
            }

            if(digitalRead(SOSB)==HIGH){
                if(sure == true and millis()-soscheck>=5000){
                    
                    if(digitalRead(SOSB) == HIGH){
                        SOS();
                        sure = false;
                    }
                    else{
                        sure = false;
                        
                    }
                }
                else if(sure == true and millis()-soscheck<5000){
                    sure = true;
                    display.clearDisplay();
                    display.setTextSize(3);          // 設定文字大小
                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                    display.setCursor(0,0);          // 設定起始座標
                    display.print("SOS MODE\npress5s");     // 要顯示的字串
                    display.display(); 
                    display.clearDisplay();
                    
                    
                }
                else{
                    sure = true;
                    soscheck = millis();
                }
               
                

            
            }

            if (gps.encode(mserial.read())) {

                if (gps.location.isValid()) {
                    wlat = String(gps.location.lat());
                    wlng = String(gps.location.lng());
                    whour = String(gps.time.hour());
                    wmin = String(gps.time.minute());
                    wsat = String(gps.satellites.value());
                    gpsok = true;
                }
            }
            if(client.available()) 
            {
                RtcDateTime now = Rtc.GetDateTime();
                String currenttime = String(now.Hour())+":"+String(now.Minute())+":"+String(now.Second());
                String c = client.readString(); 
               
                   
               if(c[0]=='&'){
                  client.print("location sent"); 
                    input = "#"+String(name)+":PHONE:"+c.substring(1,c.length())+"/板載GPS:經度:"+String(wlat)+",緯度:"+String(wlng)+",接收到衛星數:"+String(wsat)+",海拔:"+String(atti)+" T:"+String(currenttime) ; 
                    ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
                    Serial.println("已發送:"+input);
                    Sound(1);
                }else if(c[0]=='!'){
                  client.print("SOS sent DATA:"+c.substring(1,c.length()));
                    // input = String("!!!")+c.substring(1,c.length())+",{"+name+","+String(currenttime)+"},{"+String(wlat)+","+String(wlng)+","+String(wsat)+","+String(atti)+"},{"+String(h)+","+String(t)+"}"; 
                    // ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
                    // Sound(3);
                    SOS();
                    // Serial.println("已發送:"+input);
                }else if(c[0]=='?'){
                    String temp = "Temperature:"+String(t)+"°C , humidity:"+String(h)+"%";
                    client.print(temp);
                    Sound(1);
                }else{
                    input = "#"+String(name)+":"+String(c)+" T:"+String(currenttime) ; 
                    ResponseStatus rs = e22ttl.sendBroadcastFixedMessage(70, input);
                  
                   
                    Sound(1);
                    
                    client.print("user: "+c.substring(0,c.length()));
                    Serial.println("已發送:"+input);
                    
                }
        
                Serial.println("user: " + c); 
            }
            if (e22ttl.available()>1) {

            ResponseContainer rc = e22ttl.receiveMessage();
            
            String ch = String(rc.data).substring(3,rc.data.length());
            Serial.println(ch.substring(4,rc.data.length()));
            
            if(String(ch[0])==String("#")){
                client.print(ch.substring(4,rc.data.length()));
                Sound(0);
                display.clearDisplay();
                display.setTextSize(3);          // 設定文字大小
                display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                display.setCursor(0,0);          // 設定起始座標
                display.print("New Msg");     // 要顯示的字串
                display.display();
                delay(800);
                display.clearDisplay();
                display.setTextSize(3);          // 設定文字大小
                display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                display.setCursor(0,0);          // 設定起始座標
                display.print("SHOW ON\n*PHONE*");     // 要顯示的字串
                display.display();
                delay(2000);
                

            }
            else if(String(ch[0])==String("!")){
                client.print(ch.substring(4,rc.data.length()));
                client.print("請幫助此SOS呼叫!!! 撥打*112*or*911*並提交接收到資料已進行救援!");
                client.print("PLEASE HELP THIS SOS CALL!!! CALL *112*or*911*AND SUBMIT RECEIVED DATA TO RESCUE!");
                client.print("或請聯繫最近可以提供救援服務的單位!!!(嘗試撥打112,911或衛星電話!)");
                gotsos(ch);
                
            }
            }
            unsigned long currentMillis1 = millis();
                                if (currentMillis1 - previousMillis2 >= interval) {
                                    previousMillis2 = currentMillis1;
                                    if(screen == 0){
                                    screen = 1;
                                    display.clearDisplay();
                                    display.setTextSize(4);          // 設定文字大小
                                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                    display.setCursor(0,0);          // 設定起始座標
                                    RtcDateTime now2 = Rtc.GetDateTime();
                                    display.print(String(now2.Hour())+":"+String(now2.Minute())+":"+String(now2.Second()));     // 要顯示的字串
                                    //String(whour)+" : "+String(wmin)+"  sat:"+String(wsat)+"   GPS:" +String(b)+ "Pack:"+ String(count)+" "+String(t)+"C "
                                    display.display(); 
                                }
                                else if(screen == 1){
                                    screen = 2;
                                    display.clearDisplay();
                                    display.setTextSize(3);          // 設定文字大小
                                    display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                    display.setCursor(0,0);          // 設定起始座標
                                    display.print("CNT:" +String("ON")+"\nSat:"+String(wsat));     // 要顯示的字串
                                    display.display(); 
                                }
                                else if(screen == 2){
                                        screen = 3;
                                        display.clearDisplay();
                                        display.setTextSize(2);          // 設定文字大小
                                        display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                        display.setCursor(0,0);          // 設定起始座標
                                        display.print("lng:"+ String(wlat)+"\nlat:"+String(wlng));     // 要顯示的字串
                                        display.display(); 
                                    }
                                else if(screen ==3){
                                        screen = 4;
                                        display.clearDisplay();
                                        display.setTextSize(3);          // 設定文字大小
                                        display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                        display.setCursor(0,0);          // 設定起始座標
                                        display.print(String(t)+"'C\n"+String(h)+"%");     // 要顯示的字串
                                        display.display(); 
                                    }
                                else if(screen == 4){
                                        screen = 0;
                                        display.clearDisplay();
                                        display.setTextSize(2);          // 設定文字大小
                                        display.setTextColor(1);         // 1:OLED預設的顏色(這個會依該OLED的顏色來決定)
                                        display.setCursor(0,0);          // 設定起始座標
                                        //方位角度換算英文符號
//                                        String dir = "";
//                                        if(course < 22.5 && course >= 0.0 ){
//                                            dir = "N";
//                                        }
//                                        else if(course >= 22.5 && course < 67.5){
//                                            dir = "NE";
//                                        }
//                                        else if(course >= 67.5 && course < 112.5){
//                                            dir = "E";
//                                        }
//                                        else if(course >= 112.5 && course < 157.5){
//                                            dir = "SE";
//                                        }
//                                        else if(course >= 157.5 && course < 202.5){
//                                            dir = "S";
//                                        }
//                                        else if(course >= 202.5 && course < 247.5){
//                                            dir = "SW";
//                                        }
//                                        else if(course >= 247.5 && course < 292.5){
//                                            dir = "W";
//                                        }
//                                        else if(course >= 292.5 && course < 337.5){
//                                            dir = "NW";
//                                        }
//                                        else {
//                                            dir = "N";
//                                        }
                                        
                                        display.print("Speed:"+String(speed)+" KM\n"+"Course:"+String(course)+" deg\n");     // 要顯示的字串
                                        display.display();

                                }
                                }
        }
        client.stop(); 
        Serial.println("==disconnect==");
        Sound(1);
        digitalWrite(G, LOW);
        digitalWrite(B, LOW);
        }
        
    digitalWrite(G, LOW);
    }
        
        