#include <ESP8266WiFi.h>
#include <DHT.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x3F, 16, 2);

byte celsius[8] = {
  0b11000,
  0b11000,
  0b00000,
  0b00110,
  0b01001,
  0b01000,
  0b01001,
  0b00110
};

int oldmsec = 0;
int condition = 0;

float humi;
float temp;

const char* ssid = "Wemos";
const char* password = "12345678";

#define DHTPIN 13
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);


//създаване на обект server с порт 3000
WiFiServer server(3000);

void setup() {
  lcd.begin(16, 2);
  lcd.init();
  lcd.backlight();
  lcd.createChar(0, celsius);

  Serial.begin(115200);
  delay(10);
  pinMode(0, OUTPUT);  //D3 на ESP8266 D1 mini
  pinMode(2, OUTPUT);  //D4
  pinMode(12, OUTPUT); //D6
  pinMode(14, OUTPUT); //D5
  digitalWrite(0, HIGH);      // Високото активно ниво върху модула с релета се инвертира от транзистора и релето е неактивирано.
  digitalWrite(2, HIGH);
  digitalWrite(12, HIGH);
  digitalWrite(14, HIGH);

  // Свързване към мрежа
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  // Стартиране на сървър
  server.begin();
  Serial.println("Server started");

  // Визуализиране на IP
  Serial.print("Use this URL to connect: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");

}

void loop() {

  humi = dht.readHumidity();
  temp = dht.readTemperature();

  LcdLoopPrint();

  // проверка за свързан клиент
  WiFiClient client = server.available();
  if (!client) {
    return;
  }

  // изчакване на клиент
  Serial.println("new client");
  while (!client.available()) {
    delay(1);
  }

  // прочитане на първи ред на заявката
  String request = client.readStringUntil('\r');
  Serial.println(request);
  client.flush();

  // проверка на заявка

  if (request.indexOf("/light1on") > 0)  {
    digitalWrite(14, LOW);
  }

  if (request.indexOf("/light1off") > 0)  {
    digitalWrite(14, HIGH);
  }

  if (request.indexOf("/light2on") > 0)  {
    digitalWrite(12, LOW);
  }

  if (request.indexOf("/light2off") > 0)  {
    digitalWrite(12, HIGH);
  }

  if (request.indexOf("/light3on") > 0)  {
    digitalWrite(0, LOW);
  }

  if (request.indexOf("/light3off") > 0)  {
    digitalWrite(0, HIGH);
  }

  if (request.indexOf("/light4on") > 0)  {
    digitalWrite(2, LOW);
  }

  if (request.indexOf("/light4off") > 0)  {
    digitalWrite(2, HIGH);
  }

  // Визуализиране на информацията в HTML страница
  client.println("HTTP/1.1 200 OK");
  client.println("Content-Type: text/html");
  client.println(""); //  do not forget this one
  client.println("<!DOCTYPE HTML>");
  client.println("<html>");
  client.println("<head>");
  client.println("<meta charset='UTF-8'>");
  client.println("<meta name='viewport' content='width=device-width'/>");
  client.println("<meta name='apple-mobile-web-app-capable' content='yes' />");
  client.println("<meta name='apple-mobile-web-app-status-bar-style' content='black-translucent' />");
  client.println("</head>");
  client.println("<body bgcolor = \"#2E9AFE\">");
  client.println("<hr/><hr>");
  client.println("<h4><center> Контрол на електрически устройства с ESP8266 </center></h4>");
  client.println("<hr/><hr>");
  //client.println("<br><br>");
  client.println("<br><br>");
  client.println("<center>");
  client.println("ПОРТ 1");
  client.println("<a href=\"/light1on\"\"><button>Вкл. </button></a>");
  client.println("<a href=\"/light1off\"\"><button>Изкл. </button></a><br />");
  client.println("</center>");
  client.println("<br><br>");
  client.println("<center>");
  client.println("ПОРТ 2");
  client.println("<a href=\"/light2on\"\"><button>Вкл. </button></a>");
  client.println("<a href=\"/light2off\"\"><button>Изкл. </button></a><br />");
  client.println("</center>");
  client.println("<br><br>");
  client.println("<center>");
  client.println("ПОРТ 3");
  client.println("<a href=\"/light3on\"\"><button>Вкл. </button></a>");
  client.println("<a href=\"/light3off\"\"><button>Изкл. </button></a><br />");
  client.println("</center>");
  client.println("<br><br>");
  client.println("<center>");
  client.println("ПОРТ 4");
  client.println("<a href=\"/light4on\"\"><button>Вкл. </button></a>");
  client.println("<a href=\"/light4off\"\"><button>Изкл. </button></a><br />");
  client.println("</center>");
  client.println("<br><br>");
  client.println("<center>");
  client.println("<table border=\"5\">");
  client.println("<tr>");

  if (digitalRead(14))
  {
    client.print("<td>Порт 1 е ИЗКЛ.</td>");
  }
  else
  {
    client.print("<td>Порт 1 е ВКЛ.</td>");
  }

  client.println("<br />");

  if (digitalRead(12))
  {
    client.print("<td>Порт 2 е ИЗКЛ.</td>");
  }
  else
  {
    client.print("<td>Порт 2 е ВКЛ.</td>");
  }
  client.println("</tr>");
  client.println("<tr>");

  if (digitalRead(0)) {
    client.print("<td>Порт 3 е ИЗКЛ.</td>");
  }

  else
  {
    client.print("<td>Порт 3 е ВКЛ.</td>");
  }

  if (digitalRead(2)) {
    client.print("<td>Порт 4 е ИЗКЛ.</td>");
  }

  else
  {
    client.print("<td>Порт 4 е ВКЛ.</td>");
  }
  client.println("</tr>");
  client.println("</table>");
  client.println("</center>");

  client.println("<center>");
  //client.println("<br><br>");
  client.println("<table border=\"5\">");
  client.println("<tr>");
  client.print("<h4><center>Данни от сензора DHT22</center></h4>");
  client.println("</tr>");
  client.println("<tr>");
  client.print("<td>Температура (°C)</td>");
  client.print("<td>Влажност (%)</td>");
  client.println("</tr>");
  client.println("<tr>");
  client.print("<td>");
  client.print((float)temp, 2);
  client.print("</td>");
  client.print("<td>");
  client.print((float)humi, 2);
  client.print("</td>");
  client.println("</tr>");
  client.println("</table>");
  client.println("</center>");
  client.println("</html>");

  delay(1);
  Serial.println("Client disonnected");
  Serial.println("");

}

void LcdLoopPrint() {

  int msec = millis();
  if (msec - oldmsec >= 3000)
  {
    if (condition == 0) {
      lcd.clear();
      PrintWifiStatus();
      condition = 1;
    }
    else if (condition == 1)
    {
      lcd.clear();
      PortStatus();
      condition = 2;
    }
    else if (condition == 2)
    {
      lcd.clear();
      PrintDHTdata();
      condition = 0;
    }
    oldmsec = msec;
  }

}

void PortStatus() {
  lcd.setCursor(0, 0);
  if (digitalRead(14) == LOW) {

    lcd.print("Ch1:ON");
  }
  else {
    lcd.print("Ch1:OFF");
  }

  lcd.setCursor(8, 0);
  if (digitalRead(12) == LOW) {

    lcd.print("Ch2:ON");
  }
  else {
    lcd.print("Ch2:OFF");
  }

  lcd.setCursor(0, 1);
  if (digitalRead(0) == LOW) {

    lcd.print("Ch3:ON");
  }
  else {
    lcd.print("Ch3:OFF");
  }

  lcd.setCursor(8, 1);
  if (digitalRead(2) == LOW) {

    lcd.print("Ch4:ON");
  }
  else {
    lcd.print("Ch4:OFF");
  }
}

void PrintDHTdata() {
  lcd.setCursor(0, 0);
  lcd.print("Temperature: ");
  lcd.print((int)temp);
  lcd.write(byte (0));

  lcd.setCursor(0, 4);
  lcd.print("Humidity: ");
  lcd.print((int)humi);
  lcd.print("%");
}

void PrintWifiStatus() {
  lcd.setCursor(0, 0);
  lcd.print(ssid);
  lcd.setCursor(0, 1);
  lcd.print("IP");
  lcd.print(WiFi.localIP());
}
