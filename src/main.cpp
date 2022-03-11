#include <OneWire.h>
#include <DallasTemperature.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char *ssid = "BabyAncestor";
const char *password = "bb2020$!@";
const char *mqtt_server = "192.168.20.130";

WiFiClient espClient;
PubSubClient client(espClient);
unsigned long lastMsg = 0;
#define MSG_BUFFER_SIZE (50)
char msg[MSG_BUFFER_SIZE];

// Data wire is plugged into port 2 on the Arduino
#define ONE_WIRE_BUS D3
#define Moisture D5

#define LED_ONE D7
#define LED_TWO D6
float C;
int value;
// Setup a oneWire instance to communicate with any OneWire devices (not just Maxim/Dallas temperature ICs)
OneWire oneWire(ONE_WIRE_BUS);

// Pass our oneWire reference to Dallas Temperature.
DallasTemperature sensors(&oneWire);
void setup_wifi()
{

  delay(10);
  // We start by connecting to a WiFi network
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }

  randomSeed(micros());

  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}

void callback(char *topic, byte *payload, unsigned int length)
{
  Serial.print("Message arrived [");
  Serial.print(topic);
  Serial.print("] ");
}

void reconnect()
{
  // Loop until we're reconnected
  while (!client.connected())
  {
    Serial.print("Attempting MQTT connection...");
    // Create a random client ID
    String clientId = "garden_";
    clientId += String(random(0xffff), HEX);
    // Attempt to connect
    if (client.connect(clientId.c_str()))
    {
      Serial.println("connected");
      // Once connected, publish an announcement...
      Serial.println("PUBLISH DAYA");
    }
    else
    {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      // Wait 5 seconds before retrying
      delay(5000);
    }
  }
}

void setup(void)
{

  Serial.begin(115200);
  sensors.begin();
  sensors.requestTemperatures();
  sensors.setResolution(9);
  C = sensors.getTempCByIndex(0);
  Serial.println("WAKEUP LOADING");
  pinMode(Moisture, INPUT);
  pinMode(LED_ONE, OUTPUT);
  pinMode(LED_TWO, OUTPUT);

  setup_wifi();
  client.setServer(mqtt_server, 1883);
  client.setCallback(callback);

  Serial.println(C);
  Serial.println("******************************");
  value = analogRead(Moisture);
  Serial.println(value);
  Serial.println("******************************");
  while (!client.connected())
  {
    reconnect();
  }
  client.publish("status/garden_14CFER",String("\"data\" : { \"temp\": \"" + String(value) + "\" , \"moisture\" : \""
                     + String(C) + "\"}").c_str());
  delay(1000);
  ESP.deepSleep(6e7);
}
void loop(void)
{
}