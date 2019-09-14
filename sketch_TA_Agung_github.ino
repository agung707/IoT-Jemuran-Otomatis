#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <DHT.h>
#include <Servo.h>



Servo myservo;

#define DHTTYPE DHT11  

const int ldr = A0;

const char* ssid = "VIRUZ";
const char* password = "08121996";

const char* mqtt_server = "192.168.43.39";

WiFiClient espClient;
PubSubClient client(espClient);

const int DHTPin = D8;

DHT dht(DHTPin, DHTTYPE);

long now = millis();
long lastMeasure = 0;
long lastMsg= 0;
char msg[50];
int value = 0;

void setup() {
  myservo.attach(D4);
  myservo.write(0);
  dht.begin();
  Serial.begin(115200);
  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

}
void setup_wifi() {
  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.print("WiFi connected - ESP IP address: ");
  Serial.println(WiFi.localIP());
}

 void callback(char* topic, byte* payload, unsigned int length) {
  String string;
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
    string+=((char)payload[i]);
  }
  Serial.println(string);
 
  if ((char)payload[0] == '1') {
     Serial.println("masuk");
    myservo.write(180);
  } 
 
 if ((char)payload[0] == '0') {
    Serial.println("keluar");
    myservo.write(0); 
  }
  Serial.println();
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");  
      client.subscribe("servo");
    } else {
      Serial.print("failed, rc= ");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(3000);
    }
  }
}

void loop() {
  if(!client.connected()) {
    reconnect();
  }
  
  if(!client.loop())
    client.connect("ESP8266Client");

  now = millis();
  if (now - lastMeasure > 3000) {
    lastMeasure = now;
    int nilai = analogRead(ldr);   
    float h = dht.readHumidity();
    float t = dht.readTemperature();
  
    if (isnan(h) || isnan(t)||isnan(nilai)) {
      Serial.println("Failed to read from sensor!");
      return;
    }

    static char temperatureTemp[7];
    dtostrf(t, 6, 2, temperatureTemp);
     
    static char humidityTemp[7];
    dtostrf(h, 6, 2, humidityTemp);

    static char itensitasCahaya[7];
    dtostrf(nilai,6,2,itensitasCahaya);

    client.publish("suhu", temperatureTemp);
    client.publish("kelembapan", humidityTemp);
    client.publish("cahaya", itensitasCahaya);

    Serial.print("Kelembapan: ");
    Serial.print(h);
    Serial.print(" %\t Suhu: ");
    Serial.print(t);
    Serial.print(" *C ");
    Serial.print ("Cahaya = ");
    Serial.print (nilai);
    Serial.println ("V");
  }
} 
