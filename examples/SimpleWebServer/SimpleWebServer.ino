/*
  Web Server

  A simple web server that shows the time since NodeMCU began running the current program..
  using ENC2J60 Ethernet module.
  Open a any webbrowser 192.168.1.200 and see the time ticking.

  created 01 Jun 2019
  srivatsaks  DevIoT.in
  

*/


//#include <SPI.h>
//#include <Ethernet.h>
#include <UIPEthernet_edtd.h>
// MAC address from Ethernet shield sticker under board
byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};
IPAddress ip(192, 168, 1, 200); // IP address, may need to change depending on network
EthernetServer server(80);      // create a server at port 80
EthernetClient client;
// Variable to store the HTTP request
String header;
void setup()
{
  // disable Ethernet chip
  pinMode(14, OUTPUT);
  pinMode(15, OUTPUT);
  pinMode(5, OUTPUT);
  digitalWrite(5, LOW);
  Serial.begin(115200); // for debugging
  digitalWrite(5, HIGH);
  Serial.println(F("Ethernet Test"));
  Ethernet.begin(mac, ip); // initialize Ethernet device
  server.begin();          // start to listen for clients
  Serial.print("server is at ");
  Serial.println(Ethernet.localIP());
}

void loop()
{
  client = server.available(); // try to get client
  if (client > 0)
  {
    // Serial.println("New Client."); // print a message out in the serial port
    String currentLine = "";       // make a String to hold incoming data from the client
    while (client.connected())
    { // loop while the client's connected
      if (client.available())
      {                         // if there's bytes to read from the client,
        char c = client.read(); // read a byte, then
        // Serial.write(c);     // print it out the serial monitor
        header += c;
        if (c == '\n')
        { // if the byte is a newline character
          // if the current line is blank, you got two newline characters in a row.
          // that's the end of the client HTTP request, so send a response:
          if (currentLine.length() == 0)
          {
            // HTTP headers always start with a response code (e.g. HTTP/1.1 200 OK)
            // and a content-type so the client knows what's coming, then a blank line:
            client.println("HTTP/1.1 200 OK");
            client.println("Content-type:text/html");
            client.println("Connection: close");
            client.println();

            long t = millis() / 1000;
            word h = t / 3600;
            byte m = (t / 60) % 60;
            byte s = t % 60;

            client.println( "<!DOCTYPE html><html>\r\n"
                            "<meta http-equiv='refresh' content='1'/>\r\n"
                            "<head><style>html { font-family: Helvetica; display: inline-block; margin: 0px auto; text-align: center;}</style></head>\r\n"
                            "<title>Test server</title>\r\n"
                            "<body><h1>ESP8266 Web Server</h1>\r\n"
                            "<h1>\r\n");
            
            client.println(h / 10);
            client.println(h % 10);
            client.println(":");
            client.println(m / 10);
            client.println(m % 10);
            client.println(":");
            client.println(s / 10);
            client.println(s % 10);
            client.println("</body></html>");
            client.println();
            // Break out of the while loop
            break;
          }
          else
          { // if you got a newline, then clear currentLine
            currentLine = "";
          }
        }
        else if (c != '\r')
        {                   // if you got anything else but a carriage return character,
          currentLine += c; // add it to the end of the currentLine
        }
      }
    }
    // Clear the header variable
    header = "";
    // Close the connection
    client.stop();
    // Serial.println("Client disconnected.");
    // Serial.println("");
  }
}
