#include <ArduinoJson.h>
#include <Ethernet.h>
#include <SPI.h>
#define DEBUG 1 //1 - true, 0 - false


EthernetClient client;

const char* server = "192.168.1.160";
const char* resource = "/dispositivos";
int cont = 0;

const unsigned long HTTP_TIMEOUT = 10000; //max response time server
const size_t MAX_CONTENT_SIZE = 512;      //max size of the HTTP response

byte mac[] = {0xDE, 0xAD, 0xBE, 0xEF, 0xFE, 0xED};

// clientData
void setup() {
  Serial.begin(9600);
  while(!Serial){
    ;
  }
  Serial.println("Serial ready");
  if(!Ethernet.begin(mac)){
    Serial.println("Failed to configure Ethernet");
  }else{
    Serial.println("Ethernet ready");
  }
  delay(1000);
  Serial.println(Ethernet.localIP());

  //pinModes
}

void loop() {
  if(connect(server)){
    if(sendRequest(server, resource) && skipResponseHeaders()){
      if(readResponseContent()){
        disconnect();
        if(connect(server)){
          disconnect();
        }    
      }
    }
  }
wait();
}

bool connect(const char* hostName){

  bool ok = client.connect(hostName,80);
  return ok;
}

bool sendRequest(const char* host, const char* resource) {

  client.print("GET ");
  client.print(resource);
  client.println(" HTTP/1.1");
  client.print("Host: ");
  client.println(host);
  client.println("Connection: close");
  client.println();

  return true;
}

bool skipResponseHeaders() {
  // HTTP headers end with an empty line
  char endOfHeaders[] = "\r\n\r\n";

  client.setTimeout(HTTP_TIMEOUT);
  bool ok = client.find(endOfHeaders);

  if (!ok) {
    Serial.println("No response or invalid response!") ;
  }
  return ok;
}
bool readResponseContent(){
  const size_t capacity = JSON_ARRAY_SIZE(2) + 2*JSON_OBJECT_SIZE(3);
  DynamicJsonBuffer jsonBuffer(capacity);
  JsonArray& root = jsonBuffer.parseArray(client);
  
  if(!root.success()){
    return false;
  }
  for(int i=0; i<root.size();i++){
    JsonObject& root1= root.get<JsonObject&>(i);
    int id = root1.get<int>("id");
    int estatos = root1.get<int>("estatos");
      if(cont != root.size()){
        PinoModo(root.size(), id+1);
        cont = i+1; 
      }
      digitalWrite(id+1, estatos);
  }

}
void PinoModo(int tam, int id){
    pinMode(id, OUTPUT);
}
    
void disconnect() {
  client.stop();
}
void wait() {
  delay(500);
}
