#include <ESP8266WiFi.h>
#include <string>

// Network Details
// Change these to your network settings
const char* wifiSSID = "BYU-WiFi";
const char* wifiPSK  = "";

// The port the web server will run on
WiFiServer server(80);

// Tell the program which LED is on which pins
int green = D6;
int yellow = D7; 
int red = D8;

// Variables to keep track of which LEDs are on
bool green_on = 0;
bool yellow_on = 0; 
bool red_on = 0;
bool auto_on = 0;

// Timer variable to make the non-blocking loop for the auto function
unsigned long autoTimer = 0;

void setup() {
  Serial.begin(9600);
  delay(10);
  Serial.println("Initializing ...");

  // Initialize the pins
  pinMode(green, OUTPUT);
  digitalWrite(green, LOW);
  pinMode(yellow, OUTPUT);
  digitalWrite(yellow, LOW);
  pinMode(red, OUTPUT);
  digitalWrite(red, LOW);

  // ** Connect to WiFi network - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
  Serial.print("Connecting to ");
  Serial.println(wifiSSID);

  // Connect to the WiFi
  WiFi.begin(wifiSSID, wifiPSK);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");

  server.begin();
  Serial.println("Server started");

  Serial.print("Control the Stoplight at: ");
  Serial.print("http://");
  Serial.print(WiFi.localIP());
  Serial.println("/");
} // ** End Adapted Code - This is the end of the code that was adapted from www.esp8266learning.com

void loop() { // ** Create a web server - Adapted from http://www.esp8266learning.com/wemos-webserver-example.php
  WiFiClient client = server.available(); // Create a new client object for available connections
  if (client) {                           // If a client is connected, wait until it sends some data
    while (!client.available()) {         // If the client hasn't sent info, wait for it
      delay(10);
    }

    String request = client.readStringUntil('\r');  // read the first line of the request
    Serial.println(request);              // DEBUG CODE - Echo the request to the Serial Monitor for debug
    client.flush();                       // Wait until the buffers are clear

    // Turn on the green light
    if (request.indexOf("/green") != -1) {
      auto_on = 0;
      digitalWrite(green, HIGH); green_on = 1;
      digitalWrite(yellow, LOW); yellow_on = 0;
      digitalWrite(red, LOW);  red_on = 0;
    }

    // Turn on the yellow light
    if (request.indexOf("/yellow") != -1) {
      auto_on = 0;
      digitalWrite(green, LOW); green_on = 0;
      digitalWrite(yellow, HIGH); yellow_on = 1;
      digitalWrite(red, LOW); red_on = 0;
    }

    // Turn on the red light
    if (request.indexOf("/red") != -1) {
      auto_on = 0;
      digitalWrite(green, LOW); green_on = 0;
      digitalWrite(yellow, LOW); yellow_on = 0;
      digitalWrite(red, HIGH); red_on = 1;
    }

    // Turn off the lights
    if (request.indexOf("/off") != -1) {
      auto_on = 0;
      digitalWrite(green, LOW); green_on = 0;
      digitalWrite(yellow, LOW); yellow_on = 0;
      digitalWrite(red, LOW); red_on = 0;
    }

    // Cycle through all the lights
    if (request.indexOf("/auto") != -1) {
      green_on = 0; yellow_on = 0; red_on = 0;
      auto_on = 1;
    }

    // ** End Adapted Code - This is the end of the code that was adapted from www.esp8266learning.com

    // Return the response
    client.println("HTTP/1.1 200 OK");
    client.println("Content-Type: text/html");
    client.println("");
    client.println("<!DOCTYPE HTML>");
    client.println("<html>");
    client.println("<head>");

    // The concept and code of the sliding button on this page came from https://www.w3schools.com/howto/howto_css_switch.asp
    client.println("<meta name='viewport' content='width=device-width, initial-scale=1'>");
    client.println("<style>");
    client.println("html {text-align: center;}");
    client.println(".switch { position: relative; display: inline-block; width: 60px; height: 34px;}");
    client.println(".switch input {display:none;}");
    client.println(".slider { position: absolute; cursor: pointer; top: 0; left: 0; right: 0; bottom: 0; background-color: #ccc; -webkit-transition: .4s; transition: .4s;}");
    client.println(".slider:before {position: absolute; content: \"\"; height: 26px; width: 26px; left: 4px; bottom: 4px; background-color: white; -webkit-transition: .4s;transition: .4s;}");
    client.println("input:checked + .slider {background-color: #2196F3;}");
    client.println("input:focus + .slider {box-shadow: 0 0 1px #2196F3;}");
    client.println("input:checked + .slider:before {-webkit-transform: translateX(26px); -ms-transform: translateX(26px); transform: translateX(26px);}");
    client.println(".slider.round { border-radius: 34px;}");
    client.println(".slider.round:before { border-radius: 50%;}");
    client.println("</style>");
    client.println("</head>");
    client.println("<body>");

    client.println("<h2>Stoplight</h2>");

    // This section modifies the page HTML based on the current state of the LED
    // The Green Button
    client.println("<h3>Green</h3>");
    client.println("<label class='switch'>");
    client.print("  <input type=\"checkbox\" ");
    if (green_on) {
      client.print("checked ");
    }
    client.print("onchange=\"window.location.href='/");
    if (green_on) {
      client.print("off");
    } else {
      client.print("green");
    }
    client.println("'\">");
    client.println("  <span class=\"slider round\"></span>");
    client.println("</label>");

    // The Yellow Button
    client.println("<h3>Yellow</h3>");
    client.println("<label class='switch'>");
    client.print("  <input type=\"checkbox\" ");
    if (yellow_on) {
      client.print("checked ");
    }
    client.print("onchange=\"window.location.href='/");
    if (yellow_on) {
      client.print("off");
    } else {
      client.print("yellow");
    }
    client.println("'\">");
    client.println("  <span class=\"slider round\"></span>");
    client.println("</label>");

    // The Red Button
    client.println("<h3>Red</h3>");
    client.println("<label class='switch'>");
    client.print("  <input type=\"checkbox\" ");
    if (red_on) {
      client.print("checked ");
    }
    client.print("onchange=\"window.location.href='/");
    if (red_on) {
      client.print("off");
    } else {
      client.print("red");
    }
    client.println("'\">");
    client.println("  <span class=\"slider round\"></span>");
    client.println("</label>");

    // The Automatic Button                                              
    client.println("<h3>Automatic</h3>");
    client.println("<label class='switch'>");
    client.print("  <input type=\"checkbox\" ");
    if (auto_on) {
      client.print("checked ");
    }
    client.print("onchange=\"window.location.href='/");
    if (auto_on) {
      client.print("off");
    } else {
      client.print("auto");
    }
    client.println("'\">");
    client.println("  <span class=\"slider round\"></span>");
    client.println("</label>");


    client.println("</body>");
    client.println("</html>");
  }

  // The non-blocking automatic function swtiches every second
  // Got this idea from Todd Barrett - thanks Todd!
  if (auto_on && (autoTimer < millis())){
    autoTimer = millis() + 1000;
    if (!red_on && !green_on && !yellow_on){
      red_on = 1;
    }

    if (red_on){
      digitalWrite(green, HIGH);
      digitalWrite(yellow, LOW);
      digitalWrite(red, LOW);
      green_on = 1; red_on = 0;
    }

    else if (green_on){
      digitalWrite(green, LOW);
      digitalWrite(yellow, HIGH);
      digitalWrite(red, LOW);
      yellow_on = 1; green_on = 0;
    }

    else if (yellow_on){
      digitalWrite(green, LOW);
      digitalWrite(yellow, LOW);
      digitalWrite(red, HIGH);
      red_on = 1; yellow_on = 0;
    }
  }

// Save power
  delay(100);
}