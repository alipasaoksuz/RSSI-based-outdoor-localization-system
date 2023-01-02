#include <SPI.h>              // include libraries
#include <LoRa.h>

#define ss 5
#define rst 27
#define dio3 4                // change for your board; must be a hardware interrupt pin

////////LoRa//////////////
String outgoing;              // outgoing message

byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xFF;     // address of this device
byte destination = 0xBB;      // destination to send to
long lastSendTime = 0;        // last send time
int interval = 2000;          // interval between sends
int NodeID  = 2;
int NodeLat = 2105;
int NodeLong = 2002;
int NodeAlti = 2103;   
String d1[15];

void setup() {
  Serial.begin(115200);                   // initialize serial
  while (!Serial);

  Serial.println("LoRa Duplex");

  // override the default CS, reset, and IRQ pins (optional)
  LoRa.setPins(ss, rst, dio3);;// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
}

void loop() {
  if (millis() - lastSendTime > randomtime) {
    sendMessage(LoRaMessage());
    //Serial.println("Sending " + message);
    lastSendTime = millis();            // timestamp the message
    randomtime =  random(500,1500); // 2-3 seconds
  }
  
  //onReceive(LoRa.parsePacket());        // parse for a packet, and call onReceive with the result:
}

void sendMessage(String outgoing) {
  LoRa.beginPacket();                   // start packet
  LoRa.write(destination);              // add destination address
  LoRa.write(localAddress);             // add sender address
  LoRa.write(msgCount);                 // add message ID
  LoRa.write(outgoing.length());        // add payload length
  LoRa.print(outgoing);                 // add payload
  LoRa.endPacket();                     // finish packet and send it
  msgCount++;                           // increment message ID
}

void onReceive(int packetSize) {
  if (packetSize == 0) return;          // if there's no packet, return

  // read packet header bytes:
  int recipient = LoRa.read();          // recipient address
  byte sender = LoRa.read();            // sender address
  byte incomingMsgId = LoRa.read();     // incoming msg ID
  byte incomingLength = LoRa.read();    // incoming msg length

  String incoming = "";

  while (LoRa.available()) {
    incoming += (char)LoRa.read();
  }

  if (incomingLength != incoming.length()) {   // check length for error
    Serial.println("error: message length does not match length");
    return;                             // skip rest of function
  }

  // if the recipient isn't this device or broadcast,
  if (recipient != localAddress && recipient != 0xFF) {
    Serial.println("This message is not for me.");
    return;                             // skip rest of function
  }

  // if message is for this device, or broadcast, print details:
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
}

String LoRaMessage(){
  String loramessage="";
  d1[0]=NodeID;
  d1[1]=NodeLat;
  d1[2]=NodeLong;
  d1[3]=NodeAlti;
  d1[4]=msgCount;
  for(int i = 0;i<14;i++){
    loramessage+=d1[i];
    //loramessage+=",";
    }
  Serial.println(loramessage);
  return loramessage;
 }
