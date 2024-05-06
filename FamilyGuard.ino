#include <ESP8266WiFi.h>
#include <SPI.h>
#include <MFRC522.h>


char ssid[] = "SeuSSID";     // SSID da sua rede WiFi
char pass[] = "SuaSenha";    // Senha da sua rede WiFi


IPAddress server(10,140,4,188);  // Endereço IP do servidor
WiFiClient client;


#define SS_PIN D8
#define RST_PIN D0
MFRC522 rfid(SS_PIN, RST_PIN);


void setup() {
  Serial.begin(9600);
  pinMode(2, OUTPUT); // Pin D2 como saída
  digitalWrite(2, LOW); // Inicializa D2 em LOW


  SPI.begin(); // Inicia o barramento SPI
  rfid.PCD_Init(); // Inicia o MFRC522
  delay(100); // Aguarda a inicialização
  connectWiFi(); // Conecta-se à rede WiFi
}


void loop() {
  if (rfid.PICC_IsNewCardPresent() && rfid.PICC_ReadCardSerial()) {
    Serial.println("Cartão detectado!");
    sendMessageToServer("Ativo");
    delay(1000); // Aguarda um segundo antes de tentar ler outro cartão
  }
}


void connectWiFi() {
  Serial.print("Conectando ao WiFi...");
  WiFi.begin(ssid, pass);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.print(".");
  }
  Serial.println("\nConectado ao WiFi!");
}


void sendMessageToServer(String message) {
  if (client.connect(server, 8080)) {
    Serial.println("Conectado ao servidor!");
    client.println(message);
    client.stop();
  } else {
    Serial.println("Falha ao conectar ao servidor!");
  }
}
