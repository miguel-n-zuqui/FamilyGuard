#include <ESP8266WiFi.h> 
#include <PubSubClient.h>

char ssid[] = "Rodolfo_Roteador";   //seu ssid
char pass[] = "rodolfoinstrutor";  //seu password

const char* BROKER_MQTT = "192.168.43.126"; //URL do broker MQTT que se deseja utilizar
int BROKER_PORT = 1883; // Porta do Broker MQTT
#define ID_MQTT "esp8266_do_rodolfo"     //id mqtt, este deve ser único no broker

//Variáveis e objetos globais
WiFiClient espClient; // Cria o objeto espClient
PubSubClient MQTT(espClient); // Instancia o Cliente MQTT passando o objeto espClient

void setup() 
{
  pinMode(2,OUTPUT);
  digitalWrite(2,1); 
  
  Serial.begin(9600);

  MQTT.setServer(BROKER_MQTT, BROKER_PORT);   //informa qual broker e porta deve ser conectado
  MQTT.setCallback(mqtt_callback);            //função chamada quando qualquer informação de um dos tópicos subescritos chega
}

void loop() 
{
  //verifica a conexão com o roteador e tenta conectar
  if (WiFi.status() != WL_CONNECTED)
  {    
    Serial.print("\nCONECTANDO AO ROTEADOR WIFI");
    WiFi.begin(ssid, pass); // Conecta na rede WI-FI
    while(WiFi.status() != WL_CONNECTED)
    {
      delay(1000);
      Serial.print(".");
    }
    Serial.print("\nCONECTADO AO ROTEADOR!");
  }

  //enquanto estiver conectado ao roteador WIFI
  while(WiFi.status() == WL_CONNECTED)
  {
    if (!MQTT.connected())//se o broker estiver desconectado
    {
      Serial.print("\nCONECTANDO AO BROKER MQTT");
      while (!MQTT.connected()&&WiFi.status() == WL_CONNECTED)
      { 
        if (MQTT.connect(ID_MQTT))//aqui tenta conectar ao broker
        {
          Serial.print("\nCONECTADO AO BROKER COM SUCESSO!");
          MQTT.subscribe("LED2"); 
        }
        else
        {
          delay(1000);
          Serial.print("."); 
        }
      }
    }
    delay(1000);
    MQTT.loop();//keep-alive da comunicação com broker MQTT
  }
}

//Função: função de callback 
void mqtt_callback(char* topic, byte* payload, unsigned int length) 
{
    String msg;
  
    /* obtem a string do payload recebido */
    for(int i = 0; i < length; i++) 
    {
       char c = (char)payload[i];
       msg += c;
    }
 
    /*
    Serial.print("\nMensagem do topico (");
    Serial.print(topic);
    Serial.print("):");
    Serial.println(msg);
    */

    /* toma ação dependendo da string recebida */
    Serial.print("\n");
    Serial.print(topic);
    Serial.print(":");
    Serial.print(msg);
    if (strcmp(topic, "LED2") == 0)
    {
      if (msg.equals("1"))
      {
        digitalWrite(2, 0);
        MQTT.publish("RESPOSTA_LED2", "LED2 FOI LIGADO");
      }
      else if (msg.equals("0"))
      {
        digitalWrite(2,1); 
        MQTT.publish("RESPOSTA_LED2", "LED2 FOI DESLIGADO");   
      }
    }
}
