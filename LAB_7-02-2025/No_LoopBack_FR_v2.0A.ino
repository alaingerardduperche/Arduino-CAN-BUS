#include <ACAN2515.h>
#include <ACAN2515Settings.h>
#include <ACAN2515_Buffer16.h>
#include <ACAN2515_CANMessage.h>
#include <ACANBuffer.h>
#include <MCP2515ReceiveFilters.h>

//———————————————————————————————————————————
//  ACAN2515 Demo in  NO loopback mode Modified By DuduStar 20 Fevrier 2025
//  LANGUE FRANCAIS
// afficher les 29 bits de l’identifiant message de la GLEISBOX
//———————————————————————————————————————————

#include <ACAN2515.h>


static const byte MCP2515_CS  = 9 ; // CS input of MCP2515 SEEED
static const byte MCP2515_INT =  2 ; // INT output of MCP2515 SEEED

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Driver object
//——————————————————————————————————————————————————————————————————————————————

ACAN2515 can (MCP2515_CS, SPI, MCP2515_INT) ;

//——————————————————————————————————————————————————————————————————————————————
//  MCP2515 Quartz: adapt to your design
//——————————————————————————————————————————————————————————————————————————————

static const uint32_t QUARTZ_FREQUENCY = 16UL * 1000UL * 1000UL ; // 16 MHz

//——————————————————————————————————————————————————————————————————————————————
//   SETUP
//——————————————————————————————————————————————————————————————————————————————

void setup () {
//--- Switch on builtin led
  pinMode (LED_BUILTIN, OUTPUT) ;
  digitalWrite (LED_BUILTIN, HIGH) ;
//--- Start serial
  Serial.begin (38400) ;
//--- Wait for serial (blink led at 10 Hz during waiting)
  while (!Serial) {
    delay (50) ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
  }
//--- Begin SPI
  SPI.begin () ;
//--- Configure ACAN2515
  Serial.println ("Configure ACAN2515 FRANCAIS") ;
  ACAN2515Settings settings (QUARTZ_FREQUENCY, 250UL * 1000UL) ; // CAN bit rate 125 kb/s  // Modified Duperche 250
//  settings.mRequestedMode = ACAN2515Settings::LoopBackMode ; // Select loopback mode
  const uint16_t errorCode = can.begin (settings, [] { can.isr () ; }) ;
  if (errorCode == 0) {
    Serial.print ("Bit Rate prescaler: ") ;
    Serial.println (settings.mBitRatePrescaler) ;
    Serial.print ("Propagation Segment: ") ;
    Serial.println (settings.mPropagationSegment) ;
    Serial.print ("Phase segment 1: ") ;
    Serial.println (settings.mPhaseSegment1) ;
    Serial.print ("Phase segment 2: ") ;
    Serial.println (settings.mPhaseSegment2) ;
    Serial.print ("SJW: ") ;
    Serial.println (settings.mSJW) ;
    Serial.print ("Triple Sampling: ") ;
    Serial.println (settings.mTripleSampling ? "OUI" : "no") ;
    Serial.print ("Actual bit rate: ") ;
    Serial.print (settings.actualBitRate ()) ;
    Serial.println (" bit/s") ;
    Serial.print ("Exact bit rate ? ") ;
    Serial.println (settings.exactBitRate () ? "OUI" : "no") ;
    Serial.print ("Sample point: ") ;
    Serial.print (settings.samplePointFromBitStart ()) ;
    Serial.println ("%") ;
  }else{
    Serial.print ("Configuration error 0x") ;
    Serial.println (errorCode, HEX) ;
  }
}

//----------------------------------------------------------------------------------------------------------------------

static uint32_t gBlinkLedDate = 0 ;
static uint32_t gReceivedFrameCount = 0 ;
static uint32_t gSentFrameCount = 0 ;

//——————————————————————————————————————————————————————————————————————————————

void loop () {
  CANMessage frame ;
  if (gBlinkLedDate < millis ()) {
    gBlinkLedDate += 2000 ;
    digitalWrite (LED_BUILTIN, !digitalRead (LED_BUILTIN)) ;
    const bool ok = can.tryToSend (frame) ;
    if (ok) {
      gSentFrameCount += 1 ;
      //Serial.print ("ENVOI: ") ;
      //Serial.println (gSentFrameCount) ;
    }else{
      Serial.println ("Send failure") ;
    }
  }
  if (can.available ()) {
    can.receive (frame) ;
    gReceivedFrameCount ++ ;
    //Serial.print ("RECEPTION: ") ;
    //Serial.println (gReceivedFrameCount) ;
    Serial.print("Identifiant Trame : ");Serial.println (frame.id);
    Serial.print("Commande : "); Serial.println((frame.id & 0x1FE0000) >> 17);
    
    Serial.print("Received (BIN): ");
    Serial.println(frame.id, BIN); // Affichage sous forme binaire de l'identifiant

    Serial.print("Received (Decimal): ");
    Serial.println(frame.id); // Affichage sous forme decimale de l'identifiant

    Serial.print("Received (HEX): 0x");
    Serial.println(frame.id, HEX); // Affichage sous forme hexadecimale de l'identifiant
  }
}

//——————————————————————————————————————————————————————————————————————————————
