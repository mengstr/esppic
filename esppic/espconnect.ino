
void ConnectToWifi() {
//  String ssid="";
//  String pw = "";
//  for (int i = 0; i < 32; i++) ssid += char(EEPROM.read(i));
//  for (int i = 32; i < 64; i++) pw += char(EEPROM.read(i));

  Serial.println("Connecting to wifi using default info");
  WiFi.mode(WIFI_STA);
  WiFi.begin();
  int timeout=60;
  while(timeout-- && WiFi.status()!=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status()==WL_CONNECTED) {
    Serial.print("Successfully connected. IP=");
    Serial.println(WiFi.localIP());
    return;
  }
  Serial.print("Starting soft-AP for configuration... ");
  WiFi.mode(WIFI_AP);
  Serial.println(WiFi.softAP("ESPPIC") ? "Ready" : "Failed!");
  // Start up a webserver for configuration if ssid/password
  server.on("/",HTTP_POST,[]() {
        String content = "<!DOCTYPE HTML><html><pre>ESP8266 Setup\n-------------\n";
        content += "Storing credentials and rebooting.\nPlease re-connect to your usual WiFi network.</pre>";
        content += "</html>";
        server.send(200, "text/html", content);  

        Serial.printf("ssid=%s\n",server.arg("ssid").c_str());
        Serial.printf("pw=%s\n",server.arg("pw").c_str());
        ETS_UART_INTR_DISABLE();
        WiFi.disconnect();
        ETS_UART_INTR_ENABLE();
        WiFi.mode(WIFI_STA);
        WiFi.begin(server.arg("ssid").c_str(),server.arg("pw").c_str());

  int timeout=60;
  while(timeout-- && WiFi.status()!=WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println();
  if (WiFi.status()==WL_CONNECTED) {
    Serial.print("Successfully connected. IP=");
    Serial.println(WiFi.localIP());
    return;
  }
  Serial.println("FAIL FAIL FAIL!");

//        delay(100);
//        WiFi.mode(WIFI_STA);
//        ETS_UART_INTR_DISABLE();
//        WiFi.disconnect();
//        ETS_UART_INTR_ENABLE();
//        delay(2000);
//        ESP.restart();
//        delay(2000);
  });
  server.on("/", []() {
        String content = "<!DOCTYPE HTML><html><pre>ESP8266 Setup\n-------------\n";
        content += "<form method='POST' action=''>SSID:    <input name='ssid' value='Abstract85_2.4G' />\nPassword:<input name='pw' value='0863636870' />\n<input type='submit' /></form></pre>";
        content += "</html>";
        server.send(200, "text/html", content);  
  });
}


