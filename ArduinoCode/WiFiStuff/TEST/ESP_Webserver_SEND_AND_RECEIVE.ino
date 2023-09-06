/*********
  Rui Santos
  Complete project details at http://randomnerdtutorials.com
*********/

// Load Wi-Fi library
#include <ESP8266WiFi.h>

// Replace with your network credentials
const char *ssid = "INSERT_SSID_HERE";
const char *password = "INSERT_PASSWORD_HERE";

// THIS IS THE PORT FOR THIS ESP! When you send it requests you have to include the port!
WiFiServer server(1234);

// Variable to store the HTTP request
string header;

// Add any variables that you might need here!
String serverName = "http://10.0.0.225:1234";
int stateOfOtherLED = 0; // 0 means off
int allowOtherTick = 0;  // 0 means dont allow other ESP to tick
// Set timer to 5 seconds (5000)
unsigned long timerDelay = 1000;

// Current time
unsigned long currentTime = millis();
// Previous time
unsigned long previousTime = 0;
// Define timeout time in milliseconds (example: 2000ms = 2s)
const long timeoutTime = 2000;

void setup()
{
    Serial.begin(115200);
    // Initialize the built-in LED
    digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level

    // Connect to Wi-Fi network with SSID and password
    Serial.print("Connecting to ");
    Serial.println(ssid);
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED)
    {
        delay(500);
        Serial.print(".");
    }
    // Print local IP address and start web server
    Serial.println("");
    Serial.println("WiFi connected.");
    Serial.println("IP address: ");
    Serial.println(WiFi.localIP());
    server.begin();
}

void loop()
{
    WiFiClient rcvClient = server.available(); // Listen for incoming clients

    if (rcvClient)
    {                                  // If a new client connects,
        Serial.println("New Client."); // print a message out in the serial port
        String currentLine = "";       // make a String to hold incoming data from the client
        currentTime = millis();
        previousTime = currentTime;
        while (rcvClient.connected() && currentTime - previousTime <= timeoutTime)
        { // loop while the client's connected
            currentTime = millis();
            if (rcvClient.available())
            {                              // if there's bytes to read from the client,
                char c = rcvClient.read(); // read a byte, then
                Serial.write(c);           // print it out the serial monitor
                header += c;
                if (c == '\n')
                { // if the byte is a newline character
                    // if the current line is blank, you got two newline characters in a row.
                    // that's the end of the client HTTP request, so send a response:
                    if (currentLine.length() == 0)
                    {
                        // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
                        // and a content-type so the client knows what's coming, then a blank line:
                        rcvClient.println("HTTP/1.1 200 OK");
                        rcvClient.println("Content-type:text/html");
                        rcvClient.println("Connection: close");
                        rcvClient.println();

                        // turns the GPIOs on and off
                        if (header.indexOf("GET /?led=on") >= 0)
                        {
                            Serial.println("LED On");
                            digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
                            allowOtherTick = 1;
                        }
                        else if (header.indexOf("GET /?led=off") >= 0)
                        {
                            Serial.println("LED off");
                            digitalWrite(LED_BUILTIN, HIGH); // Turn the LED off by making the voltage HIGH
                            allowOtherTick = 0;
                        }

                        // The HTTP response ends with another blank line
                        rcvClient.println();
                        // Break out of the while loop
                        break;
                    }
                    else
                    { // if you got a newline, then clear currentLine
                        currentLine = "";
                    }
                }
                else if (c != '\r')
                {                     // if you got anything else but a carriage return character,
                    currentLine += c; // add it to the end of the currentLine
                }
            }
        }
        // Clear the header variable
        header = "";
        // Close the connection
        rcvClient.stop();
        Serial.println("Client disconnected.");
        Serial.println("");
    }
    else if (allowOtherTick)
    {
        // Send an HTTP GET request depending on timerDelay
        if ((millis() - lastTime) > timerDelay)
        {
            // Check WiFi connection status
            if (WiFi.status() == WL_CONNECTED)
            {
                WiFiClient sendClient;
                HTTPClient http;

                // If its 0, turn it on
                if (stateOfOtherLED == 0)
                {
                    String serverPath = serverName + "?led=on";

                    // Your Domain name with URL path or IP address with path
                    http.begin(sendClient, serverPath.c_str());
                    stateOfOtherLED = 1;
                }
                else
                {
                    String serverPath = serverName + "?led=off";

                    // Your Domain name with URL path or IP address with path
                    http.begin(sendClient, serverPath.c_str());
                    stateOfOtherLED = 0;
                }

                // Send HTTP GET request
                int httpResponseCode = http.GET();

                if (httpResponseCode > 0)
                {
                    Serial.print("HTTP Response code: ");
                    Serial.println(httpResponseCode);
                    String payload = http.getString();
                    Serial.println(payload);
                }
                else
                {
                    Serial.print("Error code: ");
                    Serial.println(httpResponseCode);
                }
                // Free resources
                http.end();
            }
            else
            {
                Serial.println("WiFi Disconnected");
            }
            lastTime = millis();
        }
    }
}
