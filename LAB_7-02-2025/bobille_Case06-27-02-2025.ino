// Bobille Cristophe du 27-02-2025
#include <ACAN2515.h>

static const byte MCP2515_CS = 9;  // CS input of MCP2515 (adapt to your design)
static const byte MCP2515_INT = 2;  // INT output of MCP2515 (adapt to your design)

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Driver object Sur SEEED V2.0
//——————————————————————————————————————————————————————————————————————————————

ACAN2515 can(MCP2515_CS, SPI, MCP2515_INT);

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Quartz: adapt to your design
//——————————————————————————————————————————————————————————————————————————————

static const uint32_t QUARTZ_FREQUENCY = 16UL * 1000UL * 1000UL;  // 16 MHz

CANMessage frame;
uint16_t thisHash = 0x2f39;

void setup() {
  //--- Start serial
  Serial.begin(115200);
  //--- Begin SPI
  SPI.begin();
  //--- Configure ACAN2515
  Serial.println("Configure ACAN2515");
  ACAN2515Settings settings(QUARTZ_FREQUENCY, 250UL * 1000UL);  // CAN bit rate 250 kb/s
  //settings.mRequestedMode = ACAN2515Settings::LoopBackMode;     // Select loopback mode
  const uint16_t errorCode = can.begin(settings, [] {
    can.isr();
  });
  if (errorCode == 0) {
    Serial.println("Configuration can ok");
  } else {
    Serial.print("Configuration error 0x");
    Serial.println(errorCode, HEX);
  }
  Serial.println("Fin du setup");
}


void loop() {

  memset(frame.data, 0x00, 8);  // On efface les datas

  if (can.available()) {
    Serial.println("-----------------------------------");
    Serial.println("Nouveau message");
    Serial.println("-----------------------------------");
    can.receive(frame);
    Serial.print("Received ID frame (BIN): ");
    Serial.println(frame.id, BIN);  // Affichage sous forme binaire de l'identifiant

    Serial.print("Received ID frame (Decimal): ");
    Serial.println(frame.id);  // Affichage sous forme decimale de l'identifiant

    Serial.print("Received ID frame (HEX): 0x");
    Serial.println(frame.id, HEX);  // Affichage sous forme hexadecimale de l'identifiant

    uint32_t hash = frame.id & 0xFFFF;
    Serial.print("Received hash (HEX): 0x");
    Serial.println(hash, HEX);  // Affichage sous forme hexadecimale de l'identifiant

    uint32_t commande = (frame.id & 0x1FE0000) >> 17;
    Serial.print("Received commande (HEX): 0x");
    Serial.println(commande, HEX);  // Affichage sous forme hexadecimale de l'identifiant

    uint32_t reponse = (frame.id & 0x10000) >> 16;
    Serial.print("Received : ");
    //Serial.println(reponse);  // Affichage sous forme hexadecimale de l'identifiant
    Serial.println(reponse ? "Reponse" : "Demande");  // Affichage sous forme hexadecimale de l'identifiant

    Serial.println("-----------------------------------");

    switch (commande) {
      case 0x18:
        if (frame.len == 0) {  // Ping : Interrogation de tous les participants du bus
          Serial.println("Ping : Interrogation de tous les participants du bus");
          CANMessage frameOut;
          frameOut.id |= 0x18 << 17;
          frameOut.id |= 1 << 16;  // Reponse
          frameOut.id |= thisHash;
          frameOut.len = 4;
          frameOut.data32[0] = thisHash;

          if (can.tryToSend(frameOut)) {
            Serial.println("\nReponse envoyee");
          } else {
            Serial.println("\nEchec de l'envoi");
          }

        } else if (frame.len == 8) {
          Serial.println("Ping :");
          uint32_t expediteur = frame.data32[0];
          Serial.print("ID expediteur : 0x");
          Serial.println(expediteur, HEX);
          uint16_t versLogiciel = frame.data16[4];
          Serial.print("Num version logiciel : 0x");
          Serial.println(versLogiciel, HEX);
          uint16_t typeAppareil = frame.data16[6];
          Serial.print("Type de l'appareil : 0x");
          Serial.println(typeAppareil, HEX);
        }
        break;

      case 0x00:  // Commande systeme
        Serial.println("Commande systeme");
        Serial.print("data32[0] : ");
        Serial.println(frame.data32[0]);
        Serial.print("data[4] : ");
        Serial.println(frame.data[4]);
        Serial.print("long : ");
        Serial.println(frame.len);
        break;

       case 0x06:  // Fonctions
         Serial.print("data[3] : ");Serial.println(frame.data[3]);
         break;

      default:
        Serial.print("autre commande : 0x");
        Serial.println(commande);
    }
    Serial.println("-----------------------------------\n");
  }
}
