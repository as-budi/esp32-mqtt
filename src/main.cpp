#include <WiFi.h>
#include <PubSubClient.h>

const int ledPin = 2;
const int button = 0;
bool buttonDown = false;
bool ledOn = false;

// WiFi
const char *ssid = "C1-9"; // Enter your WiFi name
const char *password = "zxcvbnm9";  // Enter WiFi password

// MQTT Broker
const char *mqtt_broker = "broker.emqx.io";
const char *topic_1 = "esp32/button_0";
const char *topic_2 = "esp32/led_2";
const char *mqtt_username = "emqx";
const char *mqtt_password = "public";
const int mqtt_port = 1883;

WiFiClient espClient;
PubSubClient client(espClient);

void callback(char *topic, byte *payload, unsigned int length) {
     String messageTemp;

     Serial.print("Message arrived in topic: ");
     Serial.println(topic);
     Serial.print("Message:");
     for (int i = 0; i < length; i++) {
          Serial.print((char) payload[i]);
          messageTemp += (char)payload[i];
     }
     Serial.println();
     Serial.println("-----------------------");

     if (String(topic) == "esp32/led_2") {
          Serial.print("Changing output to ");
          if(messageTemp == "on"){
               Serial.println("on");
               digitalWrite(ledPin, HIGH);
          }
          else if(messageTemp == "off"){
               Serial.println("off");
               digitalWrite(ledPin, LOW);
          }
     }
}
void wifi_setup(){
     // connecting to a WiFi network
     WiFi.begin(ssid, password);
     while (WiFi.status() != WL_CONNECTED) {
          delay(500);
          Serial.println("Connecting to WiFi..");
     }
     Serial.println("Connected to the WiFi network");
}

void connect_to_mqqt_broker(){
     //connecting to a mqtt broker
     while (!client.connected()) {
          String client_id = "esp32-client-";
          client_id += String(WiFi.macAddress());
          Serial.printf("The client %s connects to the public mqtt broker\n", client_id.c_str());
          if (client.connect(client_id.c_str(), mqtt_username, mqtt_password)) {
               Serial.println("Public emqx mqtt broker connected");
          }
          else {
               Serial.print("failed with state ");
               Serial.print(client.state());
               delay(2000);
          }
     }
}

void button_check(){
     if (digitalRead(0)){
          // detect the transition from low to high
          if (!buttonDown){
               buttonDown = true;
               // toggle the LED state
               ledOn = !ledOn;
               // digitalWrite(ledPin, ledOn);
               // send a message to everyone else
               if (ledOn){
                    client.publish(topic_1, "on");
               }
               else{
                    client.publish(topic_1, "off");
               }
          }
          // delay to avoid bouncing
          delay(100);
     }
     else{
          // reset the button state
          buttonDown = false;
     }
}

void setup() {
     pinMode(ledPin, OUTPUT);
     pinMode(button, INPUT_PULLUP);
     Serial.begin(115200);
     wifi_setup();
     client.setServer(mqtt_broker, mqtt_port);
     client.setCallback(callback);
     connect_to_mqqt_broker();
     
     // publish and subscribe
     client.publish(topic_1, "Hi EMQX I'm ESP32 ^^");
     client.subscribe(topic_2);
}

void loop() {
     if (!client.connected()) {
          connect_to_mqqt_broker();
     }
     client.loop();
     button_check();
}
