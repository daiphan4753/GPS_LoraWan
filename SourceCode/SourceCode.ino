#include <TinyGPS++.h>
#include <SoftwareSerial.h>

#define TX 9
#define RX 8
const int btn = 7;
TinyGPSPlus gps;
SoftwareSerial mySerial(RX, TX);
unsigned long time1 = 0;
String temp = "$$";
String tempSOS = "$SOS";
String tempInp = "";
String inputs = "";

void setup() {
  Serial.begin(9600);
  mySerial.begin(9600);
  pinMode(btn, INPUT);
}
void loop() {
  if (Serial.available() > 0) {

    if (gps.encode(Serial.read())) {
      // lấy tọa độ GPS
      String strLat = String(gps.location.lat(), 6);
      String strLng = String(gps.location.lng(), 6);
      // lấy giờ : phút : giây
      int hourr = gps.time.hour() + 7;
      String strHour = "";
      if (gps.time.isValid()) {
        if (hourr < 10) {
          strHour = "0" + String(hourr);
        } else {
          strHour = String(hourr);
        }

        int minuted = gps.time.minute();
        String strMinute = "";
        if (minuted < 10) {
          strMinute = "0" + String(minuted);
        } else {
          strMinute = String(minuted);
        }

        int seconded = gps.time.second();
        String strSecond = "";
        if (seconded < 10) {
          strSecond = "0" + String(seconded);
        } else {
          strSecond = String(seconded);
        }
        // lấy ngày tháng năm
        //        int dayy = gps.date.day();
        //        String strDay = "";
        //        if (dayy < 10) {
        //          strDay = "0" + String(dayy);
        //        } else {
        //          strDay = String(dayy);
        //        }
        //
        //        int monthh = gps.date.month();
        //        String strMonth = "";
        //        if (monthh < 10) {
        //          strMonth = "0" + String(monthh);
        //        } else {
        //          strMonth = String(monthh);
        //        }
        //
        //        int yearr = gps.date.year();
        //        String strYear = "";
        //        if (yearr < 10) {
        //          strYear = "0" + String(yearr);
        //        } else {
        //          strYear = String(yearr);
        //        }
        // nút SOS gửi lập tức bản tin khi nhấn
        if (digitalRead(btn) == 1) {
          String sos = "$SOS;NANO;" + strLat + ";" + strLng + ";" + strHour + ":" + strMinute + ":" + strSecond + "#";
          mySerial.println(sos);
          delay(500);
        }


        // gửi đi bản tin của nó với chu kì 120s
        if ((unsigned long) (millis() - time1) > 120000) {
          inputs = "$$NANO;" + strLat + ";" + strLng + ";" + strHour + ":" + strMinute + ":" + strSecond + "#";
          //String inputs = "$$KHN1;10.843012;106.654110;14:04:25#";
          // biển số tàu cá gồm 9 kí tự: "BD 12345 TS"
          // thay biển số tạo ra chuỗi mới dài 43 kí tự
          if (inputs != tempInp) {
            mySerial.println(inputs);
            Serial.println("inputs " + inputs);
            tempInp = inputs;
            Serial.println("tempInp " + tempInp);
          }
          time1 = millis();
        }
      }
      // đọc mySerial lấy data từ Slave khác
      if (mySerial.available()  > 0 ) {
        String inputs_ = mySerial.readStringUntil('\r\n');
        if (inputs_ != inputs && inputs_ != tempInp && inputs_.length() > 30 && inputs_.length() < 45) {
          int len = inputs.length() + 1;
          int lenTempInp = tempInp.length() + 1;
          char arr[len];
          char arrTempInp[lenTempInp];
          inputs.toCharArray(arr, len);
          tempInp.toCharArray(arrTempInp, lenTempInp);
          int lenInp = inputs_.length() + 1;
          int lenTemp = temp.length() + 1;
          int lenTempSOS = tempSOS.length() + 1;
          char arrInp[lenInp];
          char arrTemp[lenTemp];
          char arrTempSOS[lenTempSOS];
          inputs_.toCharArray(arrInp, lenInp);
          temp.toCharArray(arrTemp, lenTemp);
          tempSOS.toCharArray(arrTempSOS, lenTempSOS);
          if ( strncmp(arrInp, arrTemp, 33) != 0 && strncmp(arrInp, arr, 33) != 0 && strncmp(arrInp, arrTempInp, 33) != 0) {
            if ((strncmp(arrInp, arrTemp, 2)) == 0 ) {
              mySerial.println(String(inputs_));
              Serial.println("inputs_ " + inputs_);
              temp = String(inputs_);
              Serial.println("temp " + temp);
            } if ((strncmp(arrInp, arrTempSOS, 4)) == 0) {
              mySerial.println(inputs_);
              Serial.println(inputs_);
            }
          }
        }
      }
    }
  }
}
