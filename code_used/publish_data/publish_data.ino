
#include <ESP8266WiFi.h>
#include "Adafruit_MQTT.h"
#include "Adafruit_MQTT_Client.h"
#define S1 D7
#define S2 D5

  int val1 = 0;
  int val2 = 0;


/************************* WiFi Access Point *********************************/

#define WLAN_SSID       "sa43"
#define WLAN_PASS       "12345678"

/************************* Adafruit.io Setup *********************************/

#define AIO_SERVER      "io.adafruit.com"
#define AIO_SERVERPORT  1883                   // use 8883 for SSL
#define AIO_USERNAME    "palyam"
#define AIO_KEY         "6abc294d34b4453fa73862d3b75bfca2"    

/************ Global State (you don't need to change this!) ******************/

// Create an ESP8266 WiFiClient class to connect to the MQTT server.
WiFiClient client;
// or... use WiFiFlientSecure for SSL
//WiFiClientSecure client;

// Setup the MQTT client class by passing in the WiFi client and MQTT server and login details.
Adafruit_MQTT_Client mqtt(&client, AIO_SERVER, AIO_SERVERPORT, AIO_USERNAME, AIO_KEY);

/****************************** Feeds ***************************************/

// Setup a feed called 'photocell' for publishing.
// Notice MQTT paths for AIO follow the form: <username>/feeds/<feedname>
Adafruit_MQTT_Subscribe onoff = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/auto1");


Adafruit_MQTT_Publish fan = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/rel1");
Adafruit_MQTT_Publish light1 = Adafruit_MQTT_Publish(&mqtt, AIO_USERNAME "/feeds/rel2");
// Setup a feed called 'onoff' for subscribing to changes.
//Adafruit_MQTT_Subscribe onoffbutton = Adafruit_MQTT_Subscribe(&mqtt, AIO_USERNAME "/feeds/onoff");

/*************************** Sketch Code ************************************/

// Bug workaround for Arduino 1.6.6, it seems to need a function declaration
// for some reason (only affects ESP8266, likely an arduino-builder bug).
void MQTT_connect();




void setup() {
  Serial.begin(115200);
  delay(10);

  pinMode(S1,INPUT); 
  pinMode(S2,INPUT);  
  pinMode(5,OUTPUT);
 

  Serial.println(F("Adafruit MQTT demo"));

  // Connect to WiFi access point.
  Serial.println(); Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WLAN_SSID);

  WiFi.begin(WLAN_SSID, WLAN_PASS);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();

  Serial.println("WiFi connected");
  Serial.println("IP address: "); Serial.println(WiFi.localIP());

  // Setup MQTT subscription for onoff feed.
  //mqtt.subscribe(&onoffbutton);
    mqtt.subscribe(&onoff);


  pinMode(A0,INPUT);
  pinMode(D8,INPUT);

  pinMode(D8,LOW);

}

uint32_t x=0;

void loop() {
  // Ensure the connection to the MQTT server is alive (this will make the first
  // connection and automatically reconnect when disconnected).  See the MQTT_connect
  // function definition further below.
  MQTT_connect();
  

val1 = analogRead(A0);
val2 = digitalRead(D8);


delay(2000);


if(val1 > 1000){
  // Now we can publish stuff!
  Serial.print(F("\nSending Sensor's Value "));

  digitalWrite(S1,0);
  Serial.print("light1 ");Serial.println(digitalRead(S1));
  Serial.print("...");
  int Value = digitalRead(S1);
  if (! light1.publish(Value)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
   delay(5000);
 
}
if(val2 == 1){
  // Now we can publish stuff!
  Serial.print(F("\nSending Sensor's Value "));

  digitalWrite(S2,0);
  Serial.print("fan ");Serial.println(digitalRead(S2));
  Serial.print("...");
  int Value = digitalRead(S2);
  if (! fan.publish(Value)) {
    Serial.println(F("Failed"));
  } else {
    Serial.println(F("OK!"));
  }
   delay(5000);
 
}

 Adafruit_MQTT_Subscribe *subscription;
  while ((subscription = mqtt.readSubscription(5000))) {
    if (subscription == &onoff) {
      Serial.print(F("Got: "));
      Serial.println((char *)onoff.lastread);
      uint16_t state = atoi((char *)onoff.lastread);
        digitalWrite(5,state);

        delay(2000);
    }
}
}

// Function to connect and reconnect as necessary to the MQTT server.
// Should be called in the loop function and it will take care if connecting.
void MQTT_connect() {
  int8_t ret;

  // Stop if already connected.
  if (mqtt.connected()) {
    return;
  }

  Serial.print("Connecting to MQTT... ");

  uint8_t retries = 3;
  while ((ret = mqtt.connect()) != 0) { // connect will return 0 for connected
       Serial.println(mqtt.connectErrorString(ret));
       Serial.println("Retrying MQTT connection in 5 seconds...");
       mqtt.disconnect();
       delay(5000);  // wait 5 seconds
       retries--;
       if (retries == 0) {
         // basically die and wait for WDT to reset me
         while (1);
       }
  }
  Serial.println("MQTT Connected!");
}
