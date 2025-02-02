#define RXD2 16
#define TXD2 17
//------for SD CARD------
int a = 0, b = 0, d = 20;
#include "FS.h"
#include "SD.h"
#include "SPI.h"
int i = 0;
long int c;
//--------for Battery-----------
int batteryLevel;
//--------for Fuel-----------
int fuelLevel;
//---------For LCD-----------
#include <LiquidCrystal.h>
LiquidCrystal lcd(21, 22, 25, 26, 27, 14);
//---------For GPS----------
String check, response, sabr, timee, sms;
String currentdate,currentdate1;
String Firstdirectory;
int indi, indj;
String data1; //main captured String
String latitude, longitude, spe, datetime;
int ind1; // , locations
int ind2;
int ind3;
int ind4, ind5, ind6, ind7;
String year, month, day, hour, minute , sec;
const unsigned long gpsinterval = 20000;      // check Location every 20 sec
const unsigned long smsinterval = 7000;       // check Unread Sms every 7 sec
const unsigned long speedinterval = 3000;     // check Speed every 3 sec
const unsigned long fuelinterval = 5000;      // check Fuel every 5 sec
const unsigned long batteryinterval = 50000; // check battery health every 50 sec

unsigned long currenttime;
//--------------------------
unsigned long batteryprevioustime = 0;
unsigned long fuelprevioustime = 0;
unsigned long gpsprevioustime = 0;
unsigned long smsprevioustime = 0;
unsigned long speedprevioustime = 0;
void setup()
{
  lcd.begin(20,4);
  lcd.clear();
  lcd.print("init....");

  // Open serial communications and wait for port to open:
  Serial.begin(9600);
  while (!Serial) {
    ; // wait for serial port to connect.
  }
  lcd.noDisplay();
  pinMode(4, INPUT);
  pinMode(2, OUTPUT);
  pinMode(33, INPUT);
  pinMode(32, OUTPUT);//power key
  
  Serial2.begin(9600, SERIAL_8N1, RXD2, TXD2);
  while (!Serial2) {
    ; // wait for serial2 port to connect.
  }
  delay(500);
  lcd.display();
  Serial2.write("AT\r");
  delay(200);
  boolean y=Serial2.find("OK");
  if (y==true)
  {
  Serial.println("Module Power is already On");
  Serial.print(Serial2.readString());
  }
  else{

  
    digitalWrite(32,HIGH);
    delay(1500);
  digitalWrite(32,LOW);
  Serial.print("im turning on the power");
  lcd.noDisplay();
  delay(2500);
  lcd.display();
  delay(2500);
    }
  //Serial2.print("AT+CLTS=1\r");
  //  delay(100);
  //Serial2.print("AT&W\r");
  // Serial2.print("AT+cgpspwr=1\r");
  //  delay(200);
  //Serial2.print("AT\r");
  // delay(200);
  // Serial2.print("ATE0\r");
  //delay(200);
  // Serial2.print("ATE1\r");
  // delay(200);
  Serial.print("AT+CMGF=0\r");
  delay(200);
  //Serial.print("AT+CMGD=1,4\r");
  //delay(100);
  lcdinit();   //initializing lcd
  
  if (!SD.begin()) {
    Serial.println("Card Mount Failed");
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Card Mount Failed");
    delay(3000);
    lcd.clear();
    return;
  }
  uint8_t cardType = SD.cardType();

  if (cardType == CARD_NONE) {
    Serial.println("No SD card attached");
    return;
  }
  timegps();
  lcd.clear();
  checksdspace();
  delay(5000);
  lcd.clear();
  checkbatterylevel();
  delay(500);
  
  
  
  //Serial.print("AT+CNMI=0,0,0,0,0\r");
  String number1 = "+923162234550";
  sms = " MODULE IS RESTARTED ";
  // Serial.print("AT+CMGF=0\r");
  // sendmsg(sms, number1);

  // sms="https://www.google.com/maps/?q="+latitude +","+longitude+",14z";
//  sendmsg(sms, number1);
  Serial.println("Setup done ");
}

void loop() // run over and over
{
  currenttime = millis();
  if (currenttime > 3000000000) {
    Serial.println("restart");
    ESP.restart();
  }
  if (currenttime - gpsprevioustime >= gpsinterval) {
    gps();
    timegps();
    writetoSD();
    gpsprevioustime = currenttime;
  }

  if (currenttime - smsprevioustime >= smsinterval) {
    Serial.print("AT+CMGF=1\r"); //Entering in Text Mode
    delay(100);
    Serial.print("AT+CMGL=\"REC UNREAD\"\r");//checking unread msg
    delay(100);
    while (Serial2.available()) {


      String msg = Serial2.readString(); //reading and storing string
      Serial.println(msg);
      int indsms = msg.indexOf('"'); //fetching required data from string
      int indsms2 = msg.indexOf('"', indsms + 1);
      // Serial.println(msg);
      String smsg = msg.substring(indsms2); //required data is obtained
      //Serial.println("smsg=" + smsg);
      delay(100);
      if (smsg.length() >= 15) //found new sms
      {
        // delay(200);
        Serial.println("im in next if" );
        int indres = smsg.indexOf(':');
        Serial.print("indres= ");
        Serial.print(indres);
        Serial.println(smsg);
        //delay(200);
        if (indres == 45 || indres == 49 || indres == 9) {
          //delay(200);

          Serial.print("Separating number and msg from String ");
          Serial.print("coma1= ");
          int comma1 = smsg.indexOf(',');
          Serial.println(comma1);
          int comma2 = smsg.indexOf(',', comma1 + 1);
          Serial.print("coma2= ");
          Serial.println(comma2);
          int comma3 = smsg.indexOf(',', comma2 + 1);
          Serial.print("coma3= ");
          Serial.println(comma3);
          int comma4 = smsg.indexOf(',', comma3 + 1);
          Serial.print("coma4= ");
          Serial.println(comma4);
          int comma5 = smsg.indexOf(',', comma4 + 1);
          Serial.print("coma5= ");
          Serial.println(comma5);
          int comma6 = smsg.indexOf('"', comma5 + 1);
          Serial.print("invertedcoma=  ");
          Serial.println(comma6);
          //   int comma7 = smsg.indexOf('"', comma6+1);
          //  Serial.print("invertedcoma= ");
          //Serial.println(comma7);
          String number = smsg.substring(comma2 + 2, comma3 - 1); //number is obtained
          // Serial.println("number" + number);
          Serial.println(number);
          if (number == "+923109279602" || number == "+923162234550" || number == "+923078913602" || number == "+923145293271" || number == "+923115469049") //checking registered number
          {
            String messagerec = smsg.substring(comma6 + 2, comma6 + 5); //recieving message

            Serial.println(smsg.substring(comma6 + 2, comma6 + 3));
            lcd.clear();
            lcd.print(msg);
            long messager = messagerec.toInt(); //reading message in integer form
            Serial.print("i recieved ");
            Serial.print(messagerec);
            Serial.print(" Now Converted to ");
            Serial.print(messager);
            if (messager == 1)             //Location
            {
              // https://www.google.com/maps/@8.2630696,77.3022699,14z
              sms = "https://www.google.com/maps?q=" + latitude + "," + longitude ;
              Serial.println(smsg);
              sendmsg(sms, number);
            }
            else if (messager == 2)             //SPEED
            {
              sms = "Current Speed of your Vehicle is =" + spe;
              sendmsg(sms, number);
            }
            else if (messager == 3)          //Fuel
            {

              sms =  "fuel ";
              sendmsg(sms, number);
            }
            else if (messager == 4)         //SD Space
            {
              checksdspace();
              sms = "Space ";
              sendmsg(sms, number);
            }
            else if (messager == 5)         //battery Health
            {
              checkbatterylevel();
              sms =  "fuel ";;
              sendmsg(sms, number);
            }
            else if (messager == 6)        //ALL Detail
            {
              sms = "All details will share to You soon";
              sendmsg(sms, number);
            }
            else if (messager == 7)        //Module restart
            {

              sms = "Restared Module in Respoce of Sms";
              sendmsg(sms, number);
              Serial.print("AT+CMGF=0\r");
              delay(100);
              ESP.restart();

            }

            else
            {
              sms = "Sir:\n   Please reply with\n1 for Location\n2 for Current Speed\n3 for Fuel\n4 for SD space\n5 for Battery Health and\n6 for all\n7 for restart Your module ";
              sendmsg(sms, number);
            }
          }
          smsprevioustime = currenttime;
          Serial.print("AT+CMGF=0\r");
          delay(100);
          Serial.print("AT+CMGD=1,4\r");
          delay(100);
        }
      }

    }
    smsprevioustime = currenttime;
  }


  //if(currenttime-speedprevioustime>=speedinterval){
  //
  // speedprevioustime=currenttime;
  //}


  if (currenttime - fuelprevioustime >= fuelinterval) {

    fuelprevioustime = currenttime;
    checkfuellevel();
  }

  if (currenttime - batteryprevioustime >= batteryinterval) {

    batteryprevioustime = currenttime;
    checkbatterylevel();
  }

lcd.setCursor(11,3);
  lcd.print(timee);
lcd.setCursor(0,3);
  lcd.print(currentdate1);
delay(500);
}

void sendmsg(String sms, String snumber) {
  Serial.println(" im in Now ");
  Serial.println(snumber);
  Serial2.print("AT+CMGF=1\r");
  delay(100);
  Serial2.print("AT+CMGS=\"");
  Serial2.print(snumber);
  Serial2.print("\"\r");
  delay(100);
  Serial2.println(sms);
  delay(100);
  Serial.print((char)26);
  delay(1000);
  Serial2.print("AT+CMGF=0\r");
  delay(100);
  Serial.print("The Mesage : ");
  Serial.print(sms);
  Serial.print(": IS Send To Phone Number : ");
  Serial.print(snumber);

}

//-----------------------------------------------------------------------------------------------------------------------
String gpsressponce() {
  //this function checks what sim808 transmit to Data Aquisition Interface
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Checking Responce");
  //Serial.print("Checking Responce");
  if (Serial2.available() > 0) {
    response = Serial2.readString();
    indi = response.indexOf(':');
    indj = response.indexOf('\n', indi);
    check = response.substring(indi + 2, indj - 1);
    Serial.println(check);
  }
  delay(2000);
  return check;
}
//-----------------------------------------------------------------------------------------------------------------------
void gps()
{
  // this function Sends GPS commands to Sim808 Module
  Serial.println("Sending gps commands from DAQ to Sim808 Module");
  Serial.print("at+cgnspwr?\r"); //check the power
  sabr = gpsressponce();
  lcd.setCursor(0, 0);
  lcd.print("                    ");
  lcd.setCursor(0, 0);
  lcd.print(check);

  if (gpsressponce() == "1") {
    Serial.println("power is on");
    delay(50);
  }
  else {

    Serial.print("at+cgnspwr=1\r");//if power is off,it will make it ON
    Serial.println("now i turned the power on");
    delay(100);
  }
//  Serial.print("AT+CGPSSTATUS?\r");//check gps signal
  delay(1000);
//  while (gpsressponce() !="Location 3D Fix") { //wait untill recieved the gps signal 
//    lcd.clear();
//    lcd.setCursor(0, 0);
//    lcd.print("location not fixed");
//    delay(1000);
//    Serial.print("AT+CGPSSTATUS?\r");
//  }
  lcd.setCursor(0, 1);
  lcd.print("               ");
  lcd.setCursor(0, 0);
  lcd.print("location fixed");
  Serial2.print("at+cgnsinf\r");//recieving coordinates

  latlong();//separating coordinates from NMEA sentence
  lcd.setCursor(0, 1);
  lcd.print(" congratulations ");
  delay(1000);
  lcd.clear();
}
//---------------------------------------------------------------------------------------------------------------------
void latlong() {
  Serial.println("Separating GPS Coordinates");
  Serial.println("Lat,Long Separating");
  Serial.println();
  Serial.print("captured String is : ");
  data1 = Serial2.readString();
  Serial.println(data1); //prints string to serial port out

  ind1 = data1.indexOf(',');  //finds location of first ,

  ind2 = data1.indexOf(',', ind1 + 1 ); //finds location of second ,

  ind3 = data1.indexOf(',', ind2 + 1 ); //finds location of third ,
  datetime = data1.substring(ind2 + 1, ind3);
  ind4 = data1.indexOf(',', ind3 + 1 ); //finds location of fourth ,
  latitude = data1.substring(ind3 + 1, ind4);
  ind5 = data1.indexOf(',', ind4 + 1 ); //finds location of fifth ,
  longitude = data1.substring(ind4 + 1, ind5);
  ind6 = data1.indexOf(',', ind5 + 1 ); //finds location of sixth ,
  ind7 = data1.indexOf(',', ind6 + 1 ); //finds location of seventh ,
  spe = data1.substring(ind6 + 1, ind7);
  Serial.print("datetime = ");
  Serial.println(datetime);
  Serial.print("lat = ");
  Serial.println(latitude);
  Serial.print("long ");
  Serial.println(longitude);
  Serial.print("speed = ");
  Serial.println(spe);
  Serial.println();
  Serial.println();
  year = datetime.substring(0, 4);
  month = datetime.substring(4, 6);
  day = datetime.substring(6, 8);
  hour = datetime.substring(8, 10);
  minute = datetime.substring(10, 12);
  sec = datetime.substring(12, 14);
  Serial.print("date = ");
  currentdate1 = day + "-" + month + "-" + year;
 
 

  Serial.println(currentdate);
  currentdate = day + "" + month + "" + year;
  Serial.println(currentdate);
  timee = hour + ":" + minute + ":" + sec;

  data1 = "";
  
}

//--------------------------------------------------------------------------------------------------------------------
void checkfuellevel()
{
  analogRead(33);
  Serial.print(" fuel reading = ");
  Serial.print(analogRead(33));
  fuelLevel = map(analogRead(33), 0.0f, 770.0f, 0, 100);
  if (fuelLevel > 90)
  {
    fuelLevel = 100;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else if (fuelLevel > 90)
  {
    fuelLevel = 90;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else if (fuelLevel > 80)
  {
    fuelLevel = 80;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else if (fuelLevel > 70)
  {
    fuelLevel = 70;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else if (fuelLevel > 60)
  {
    fuelLevel = 60;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else if (fuelLevel > 50)
  {
    fuelLevel = 50;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else if (fuelLevel > 40)
  {
    fuelLevel = 40;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else if (fuelLevel > 30)
  {
    fuelLevel = 30;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else if (fuelLevel > 20)
  {
    fuelLevel = 20;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
  else
  {
    fuelLevel = 0;
    Serial.print(" fuel percentage = ");
    Serial.print(fuelLevel);
  }
    lcd.setCursor(0, 2);
  lcd.print("FuelR=");
  lcd.setCursor(6,2);
  lcd.print(analogRead(33));
  lcd.setCursor(10, 2);
  lcd.print("fuelL=");
  lcd.setCursor(17, 2);
  lcd.print(fuelLevel);


}
void checkbatterylevel()
{
  
  batteryLevel = map(analogRead(4), 0.0f, 4095.0f, 0, 100);
  lcd.setCursor(0, 1);
  lcd.print("Bat R=");
  lcd.setCursor(6,1);
  lcd.print(analogRead(4));
  lcd.setCursor(10, 1);
  lcd.print("Bat L=");
  lcd.setCursor(16, 1);
  lcd.print(batteryLevel);
  // delay(500);
  if (batteryLevel < 70)
  {
    digitalWrite(2, HIGH);
  }
  else if (batteryLevel > 95)
  {
    digitalWrite(2, LOW);
  }
  else
  {
  }

}

//-----------------------------------------------------------------------------------------------------------------------
void readFile(fs::FS &fs, const char * path) {
  Serial.printf("Reading file: %s\n", path);

  File file = fs.open(path);
  if (!file) {
    Serial.println("Failed to open file for reading");
    return;
  }

  Serial.print("Read from file: ");
  while (file.available()) {
    Serial.write(file.read());
  }
  file.close();
}
//-----------------------------------------------------------------------------------------------------------------------
void lcdinit()
{
  
  lcd.clear();
  lcd.setCursor(8, 0);
  lcd.print("N");
  delay(d+80);
  lcd.setCursor(9, 0);
  lcd.print("R");
  delay(d+80);
  lcd.setCursor(10, 0);
  lcd.print("T");
  delay(d+80);
  lcd.setCursor(11, 0);
  lcd.print("C");
  delay(d+80);
  lcd.noDisplay();
  delay(400);
  lcd.display();
  delay(400);
  lcd.noDisplay();
  delay(400);
  lcd.display();
  delay(400);
  lcd.noDisplay();
  delay(400);
  lcd.display();
  delay(600);
  
  lcd.setCursor(7, 1);
  lcd.print("W");
  delay(d);
  lcd.setCursor(8, 1);
  lcd.print("E");
  delay(d);
  lcd.setCursor(9, 1);
  lcd.print("L");
  delay(d);
  lcd.setCursor(10, 1);
  lcd.print("C");
  delay(d);
  lcd.setCursor(11, 1);
  lcd.print("O");
  delay(d);
  lcd.setCursor(12, 1);
  lcd.print("M");
  delay(d);
  lcd.setCursor(13, 1);
  lcd.print("E");
  delay(d);
  
  lcd.setCursor(9, 2);
  lcd.print("T");
  delay(d);
  lcd.setCursor(10, 2);
  lcd.print("o");
  delay(d);
  
  lcd.setCursor(6, 3);
  lcd.print("B");
  delay(d);
  lcd.setCursor(7, 3);
  lcd.print("L");
  delay(d);
  lcd.setCursor(8, 3);
  lcd.print("A");
  delay(d);
  lcd.setCursor(9, 3);
  lcd.print("C");
  delay(d);
  lcd.setCursor(10, 3);
  lcd.print("K");
  delay(d);
  lcd.setCursor(11, 3);
  lcd.print(" ");
  delay(d);
  lcd.setCursor(12, 3);
  lcd.print("B");
  delay(d);
  lcd.setCursor(13, 3);
  lcd.print("O");
  delay(d);
  lcd.setCursor(14, 3);
  lcd.print("X");
  delay(4000);
  lcd.clear();
  


}

//----------------------------------------------------------------------------------------------------------------------
void writeFile(fs::FS &fs, String path, String message) {
  Serial.printf("Writing file: %s\n", path);

  File file = fs.open(path, FILE_WRITE);
  if (!file) {
    Serial.println("Failed to open file for writing");
    return;
  }
  if (file.print(message)) {
    Serial.println("File written");
  } else {
    Serial.println("Write failed");
  }
  file.close();
}
//-----------------------------------------------------------------------------------------------------------------------
void renameFile(fs::FS &fs, const char * path1, const char * path2) {
  Serial.printf("Renaming file %s to %s\n", path1, path2);
  if (fs.rename(path1, path2)) {
    Serial.println("File renamed");
  } else {
    Serial.println("Rename failed");
  }
}
//-----------------------------------------------------------------------------------------------------------------------
void deleteFile(fs::FS &fs, const char * path) {
  Serial.printf("Deleting file: %s\n", path);
  if (fs.remove(path)) {
    Serial.println("File deleted");
  } else {
    Serial.println("Delete failed");
  }
}
//-----------------------------------------------------------------------------------------------------------------------
void checksdspace()
{
  a = SD.totalBytes() / (1024 * 1024);
  b = SD.usedBytes() / (1024 * 1024);
  if (((b / a) * 100) > 80) {
    FirstDir(SD, "/", 0);
    removeDir(SD, Firstdirectory);
  }
  int d1 = d + 20;
  lcd.setCursor(0, 0);
  lcd.print("Total S= ");

  lcd.print(a);
  lcd.print("Mbs");
  delay(d1);

  lcd.setCursor(0, 1);
  lcd.print("Used= ");
  lcd.print(b);
  lcd.print("Mbs");
}
//---------------------------------------------------------------------------------------------------------------------
void createDir(fs::FS &fs, String path) {
  Serial.printf("Creating Dir: %s\n", path);
  if (fs.mkdir(path)) {
    Serial.println("Dir created");
  } else {
    Serial.println("mkdir failed");
  }
}
//---------------------------------------------------------------------------------------------------------------------
void FirstDir(fs::FS &fs, const char * dirname, uint8_t levels) {
  File root = fs.open(dirname);
  if (!root) {
    Serial.println("Failed to open directory");
    return;
  }
  if (!root.isDirectory()) {
    Serial.println("Not a directory");
    return;
  }
  File file = root.openNextFile();
  if (file.isDirectory()) {
    Serial.println( file.name());
    Firstdirectory = file.name();
    if (levels) {
      FirstDir(fs, file.name(), levels - 1);
    }
  }
  file = root.openNextFile();
}
//---------------------------------------------------------------------------------------------------------------------
void removeDir(fs::FS &fs, String path) {
  Serial.printf("Removing Dir: %s\n", path);
  if (fs.rmdir(path)) {
    Serial.println("Dir removed");
  } else {
    Serial.println("rmdir failed");
  }
}
//---------------------------------------------------------------------------------------------------------------------
void appendFile(fs::FS &fs, String path, String message) {
  Serial.printf("Appending to file: %s\n", path);
  File file = fs.open(path, FILE_APPEND);
  if (!file) {
    Serial.println("Failed to open file for appending");
    return;
  }
  if (file.print(message)) {
    Serial.println("Message appended");
  } else {
    Serial.println("Append failed");
  }
  file.close();
}
//-----------------------------------------------------------------------------------------------------------------------
void timegps() {

  Serial.print("AT+CCLK?\r");
  String d = Serial2.readString();
  timee = d.substring(28, 36);
  Serial.println(timee);

}
//---------------------------------------------------------------------------------------------------------------------
void writetoSD() {
  if (SD.exists("/" + currentdate) == false) {
    Serial.print("directory ceated");
    createDir(SD, "/" + currentdate);
    writeFile(SD, "/" + currentdate + "/data.txt", timee + "," + spe + "," + latitude + "," + longitude + "\n");
    writeFile(SD, "/" + currentdate + "/gps.txt",timee + "," +latitude + "," + longitude + "\n");
  }
  else {
    appendFile(SD, "/" + currentdate + "/data.txt", timee + "," + spe + "," + latitude + "," + longitude + "\n" );
    appendFile(SD, "/" + currentdate + "/gps.txt", timee + ","+latitude + "," + longitude + "\n");
    Serial.println("append file");
  }
}
//---------------------------------------------------------------------------------------------------------------------
//  writeFile(SD, "/hi.txt", "/Mujeeb ");
//    readFile(SD, "/hi.txt");
//    renameFile(SD, "/hi.txt", "/fo.txt");
//    readFile(SD, "/foo.txt");
