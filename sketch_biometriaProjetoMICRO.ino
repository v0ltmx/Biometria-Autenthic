/*
 * UNIVERSIDADE FEDERAL DO RIO GRANDE DO NORTE
 * Escola Agrícola de Jundiaí
   @lucasgabr1el
*/
#include <Adafruit_Fingerprint.h>
#include <SoftwareSerial.h>
#include <PushButton.h>
#define pinBot 11
#define pinTrava 4


SoftwareSerial mySerial(7, 8);

Adafruit_Fingerprint finger = Adafruit_Fingerprint(&mySerial);
PushButton botao(pinBot);


uint8_t numID = 1;
bool gravar=false;

uint8_t modoGravacaoID(uint8_t IDgravar);

void setup() {
  pinMode(pinTrava, OUTPUT);
  digitalWrite(pinTrava, LOW);
  
  Serial.begin(9600);
  finger.begin(57600);

  if(finger.verifyPassword()){
    Serial.println("Sensor biometrico Conectado!");
  } else {
    Serial.println("Sensor biometrico não encontrado! Verifique a conexão e reinicie o sistema");
    while(true) { 
      delay(1); 
      }
  }

  Serial.println("Disponível para inserir a digital cadastrada.");
}

void loop() {
  botao.button_loop();

  if ( botao.pressed() ){
    gravar = true;
  }

  if(gravar){
    modoGravacaoID(0);
    gravar = false;
  }

  getFingerprintIDez();
  
}

uint8_t modoGravacaoID(uint8_t IDgravar) {

  int p = -1;
  Serial.print("Esperando uma leitura válida para gravar #"); Serial.println(IDgravar);
  delay(2000);
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.println(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de leitura");
      break;
    default:
      Serial.println("Erro desconhecido");
      break;
    }
  }



  p = finger.image2Tz(1);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar propriedade da digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  
  Serial.println("Remova o dedo");
  delay(2000);
  p = 0;
  while (p != FINGERPRINT_NOFINGER) {
    p = finger.getImage();
  }
  Serial.print("ID "); Serial.println(IDgravar);
  p = -1;
  Serial.println("Coloque o Mesmo dedo novamente");
  while (p != FINGERPRINT_OK) {
    p = finger.getImage();
    switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura concluída");
      break;
    case FINGERPRINT_NOFINGER:
      Serial.print(".");
      delay(200);
      break;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      break;
    case FINGERPRINT_IMAGEFAIL:
      Serial.println("Erro de Leitura");
      break;
    default:
      Serial.println("Erro desconhecido");
      break;
    }
  }



  p = finger.image2Tz(2);
  switch (p) {
    case FINGERPRINT_OK:
      Serial.println("Leitura convertida");
      break;
    case FINGERPRINT_IMAGEMESS:
      Serial.println("Leitura suja");
      return p;
    case FINGERPRINT_PACKETRECIEVEERR:
      Serial.println("Erro de comunicação");
      return p;
    case FINGERPRINT_FEATUREFAIL:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      return p;
    case FINGERPRINT_INVALIDIMAGE:
      Serial.println("Não foi possível encontrar as propriedades da digital");
      return p;
    default:
      Serial.println("Erro desconhecido");
      return p;
  }
  

  Serial.print("Criando modelo para #");  Serial.println(IDgravar);
  
  p = finger.createModel();
  if (p == FINGERPRINT_OK) {
    Serial.println("As digitais batem!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_ENROLLMISMATCH) {
    Serial.println("As digitais não batem");
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  }   
  
  Serial.print("ID "); Serial.println(IDgravar);
  p = finger.storeModel(IDgravar);
  if (p == FINGERPRINT_OK) {
    Serial.println("Armazenado!");
  } else if (p == FINGERPRINT_PACKETRECIEVEERR) {
    Serial.println("Erro de comunicação");
    return p;
  } else if (p == FINGERPRINT_BADLOCATION) {
    Serial.println("Não foi possível gravar neste local da memória");
    return p;
  } else if (p == FINGERPRINT_FLASHERR) {
    Serial.println("Erro durante escrita na memória flash");
    return p;
  } else {
    Serial.println("Erro desconhecido");
    return p;
  }   
}

int getFingerprintIDez() {
  uint8_t p = finger.getImage();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.image2Tz();
  if (p != FINGERPRINT_OK)  return -1;

  p = finger.fingerFastSearch();
  if (p != FINGERPRINT_OK)  return -1;
  
  
  if (finger.fingerID == 0) {
     Serial.print("Modo Administrador!");
     
     numID++;
     modoGravacaoID(numID);
     return 0; 
  
  }
  else {

     digitalWrite(pinTrava, HIGH);
     Serial.print("ID encontrado #"); Serial.print(finger.fingerID); 
     Serial.print(" com confiança de "); Serial.println(finger.confidence);
     delay(500);
     digitalWrite(pinTrava, LOW);
     return finger.fingerID;
  
  } 
}
