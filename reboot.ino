/**
This sketch performs the following actions;

1) Connects to local wifi
2) Every 20 minutes, it pings 'google' (and if no reply - then 'cloudflare') and if it gets
   a response, the ESP will go to deep sleep mode for 20 mins, then repeat.
3) If no reply is received from either, the sketch enters a loop, rechecking google &
   cloudflare, for a further 4 times, 1 minute apart.
4) If no reply is received, the relay will remove power to the router for 5
   seconds causing a router reboot.
5) ESP will then go into deep sleep for 5 minutes to allow the router to
   stabilise.
   
https://www.losant.com/blog/making-the-esp8266-low-powered-with-deep-sleep
**/

#include <ESP8266Ping.h>
#include <ESP8266WiFi.h>

// WiFi credentials.
const char* WIFI_SSID = "myssid";
const char* WIFI_PASS = "mypassword";

#define MINUTES (60*1000)
long now;
long startTime = millis();
unsigned long wifiConnectStart;
const IPAddress google (8, 8, 8, 8);
const IPAddress cloudflare (1, 1, 1, 1);
long rssi;
int count = 0;
long lastPingTime = 0;
// pin 2 is the LED pin
// pin 15 = D8
#define relayPin 15

WiFiClientSecure wifiClient;

void connect() {

  // Connect to Wifi.
  Serial.println();
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(WIFI_SSID);

  WiFi.begin(WIFI_SSID, WIFI_PASS);

  // WiFi fix: https://github.com/esp8266/Arduino/issues/2186
  WiFi.persistent(false);
  WiFi.mode(WIFI_OFF);
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASS);

  wifiConnectStart = millis();

  while (WiFi.status() != WL_CONNECTED) {
    // Check to see if
    if (WiFi.status() == WL_CONNECT_FAILED) {
      Serial.println("Failed to connect to WiFi. Please verify credentials: ");
      delay(10000);
    }

    delay(500);
    Serial.print(".");
    // Only try for 15 seconds.
    if (millis() - wifiConnectStart > 15000) {
      Serial.println("Failed to connect to WiFi");
      return;
    }
  }

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.print("IP address: ");
  Serial.println(WiFi.localIP());
  // Print the wifi signal strength
  rssi = WiFi.RSSI();
  Serial.print("RSSI:");
  Serial.println(rssi);
 }

void setup() {
  Serial.begin(115200);
  Serial.setTimeout(2000);
  pinMode(relayPin, OUTPUT);
  digitalWrite(relayPin, LOW);
  connect();
}

void loop() {
// Check for internet access
  if((millis() - lastPingTime >= (1*MINUTES)) && (count < 5)){
  lastPingTime = millis(); 
  if (Ping.ping(google))
  {
    Serial.println("reply received OK from google");
    Serial.println("Going into deep sleep for 20 minutes");
    ESP.deepSleep(1.2e+9); // 1.2e+9 is 20 minutes
  }
  else if (Ping.ping(cloudflare))
  {
    Serial.println("reply received OK from cloudflare");
    Serial.println("Going into deep sleep for 20 minutes");
    ESP.deepSleep(1.2e+9); // 1.2e+9 is 20 minutes
    //If no reply from either site, iterate the loop a further 4 times and then reboot router
     } else {
       count++;
       Serial.print("no reply #");
       Serial.println(count);
    if (count >= 5) {
      Serial.print("No ping results - rebooting router ");
      //Reboot code
        digitalWrite(relayPin, HIGH);
        delay(5000);
        digitalWrite(relayPin, LOW);
        Serial.println("...and going to sleep whilst router reboots");
        ESP.deepSleep(3e+8); // 5 minutes
   }
  }
 }  
}
