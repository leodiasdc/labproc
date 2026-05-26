#include <WiFi.h>

const char* ssid = "iPhone";
const char* password = "07092005";


// Set web server port number to 80
WiFiServer server(80);

const int PIN_LED1 = 23;
const int PIN_LED2 = 24;
const int PIN_LED3 = 25;
const int PIN_LED4 = 26;

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi network with SSID and password
  Serial.print("Connecting to ");
  Serial.println(ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
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
void loop(){
    WiFiClient client = server.available();   // Listen for incoming clients
    if (client) {
    currentTime = millis();
    previousTime = currentTime;
    Serial.println("New Client.");          // print a message out in the serial port
    String currentLine = "";                // make a String to hold incoming data from the client
    while (client.connected() && currentTime - previousTime <= timeoutTime) {  // loop while the client's connected
      currentTime = millis();
      if (client.available()) {             // if there's bytes to read from the client,
        char c = client.read();             // read a byte, then
        Serial.write(c);                    // print it out the serial monitor
        header += c;
        if (c == '\n') {                    // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0) {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            // Gerar HTML
            client.println("<!DOCTYPE html>");
            client.println("<html>");
            client.println("<head>");
            client.println("<title>Calculadora</title>");
            client.println("<style>");
            client.println("body { font-family: Arial; text-align: center; margin-top: 50px; }");
            client.println("input { padding: 10px; margin: 10px; font-size: 16px; }");
            client.println("button { padding: 10px 20px; margin: 10px; font-size: 16px; cursor: pointer; }");
            client.println("</style>");
            client.println("</head>");
            client.println("<body>");
            client.println("<h1>Calculadora</h1>");
            client.println("<form action=\"/\" method=\"GET\">");
            client.println("Número 1: <input type=\"number\" name=\"num1\" required><br>");
            client.println("Número 2: <input type=\"number\" name=\"num2\" required><br>");
            client.println("<button type=\"submit\" name=\"op\" value=\"soma\">Somar</button>");
            client.println("<button type=\"submit\" name=\"op\" value=\"subtracao\">Subtrair</button>");
            client.println("</form>");
            client.println("</body>");
            client.println("</html>");
          }
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    Serial.println("Client disconnected.");
    Serial.println("");
}

