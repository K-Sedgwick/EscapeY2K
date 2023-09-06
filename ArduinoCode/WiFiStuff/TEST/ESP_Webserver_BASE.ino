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

// Add any variables that you might need here!

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
    pinMode(LED_BUILTIN, OUTPUT);  // Initialize the LED_BUILTIN pin as an output

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
                        rcvClient.println("Content-type: application/json");
                        rcvClient.println("Content-Length: 20"); //THIS NEEDS TO BE DYNAMIC
                        rcvClient.println("Access-Control-Allow-Origin: *");
                        rcvClient.println();

                        // PUT YOUR LOGIC HERE!
                        // Change the strings after GET to what youre hoping the URL will have in it
                        if (header.indexOf("GET /?led=on") >= 0)
                        {
                            Serial.println("LED On");
                            ledStateOfOtherESP = "on";
                            digitalWrite(LED_BUILTIN, LOW); // Turn the LED on (Note that LOW is the voltage level
                        }

                        // The HTTP response ends with another blank line
                        rcvClient.println("{'success':'true'}"); //CONTENT LENGTH DYNAMIC BASED ON HOW LONG THIS IS (IN CHARACTERS) + 1
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
    else
    {
        // ENTER LOGIC FOR OTHER PROCESSES THAT YOU WANT THE ESP TO DO HERE
        // FOR EXAMPLE, SENDING AN HTTP GET REQUEST
        // Send an HTTP GET request depending on timerDelay
        if ((millis() - lastTime) > timerDelay)
        {
            // Check WiFi connection status
            if (WiFi.status() == WL_CONNECTED)
            {
                WiFiClient sendClient;
                HTTPClient http;
                String serverPath = INSERT_URL_TO_ACCESS_HERE;

                // Your Domain name with URL path or IP address with path
                http.begin(sendClient, serverPath.c_str());

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
