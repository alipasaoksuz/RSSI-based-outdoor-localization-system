#include <SPI.h>              // include libraries
#include <LoRa.h>

#define ss 5
#define rst 27
#define dio3 4                // change for your board; must be a hardware interrupt pin

//String outgoing;              // outgoing message
String hamveri = "";
byte msgCount = 0;            // count of outgoing messages
byte localAddress = 0xBB;     // address of this device
byte destination = 0xFF;      // destination to send to
long lastSendTime = 0;        // last send time
float MPwr1 = -37.0;
float MPwr2 = -36.0;
float MPwr3 = -37.0;
float MPwr4 = -43.0;           // 1m referans rssı değeri
float N1 = 1.8;
float N2 = 1.8; 
float N3 = 1.8; 
float N4 = 1.8; // ortam kaybı
long Node1[9];
long Node2[9];
long Node3[9];
long Node4[9];
float payX[3][3];
float paydaX[3][3];
float payY[3][3];
float paydaY[3][3];
float payZ[3][3];
float paydaZ[3][3];
float gpsXpay = 1;
float gpsXpayda = 1;
float gpsX = 1;
float gpsYpay = 1;
float gpsYpayda = 1;
float gpsY = 1;
float gpsZpay = 1;
float gpsZpayda = 1;
float gpsZ = 1;

void setup() {
  Serial.begin(115200);
  while (!Serial);
  
  Serial.println("LoRa Duplex");
  
  LoRa.setPins(ss, rst, dio3);// set CS, reset, IRQ pin

  if (!LoRa.begin(433E6)) {             // initialize ratio at 915 MHz
    Serial.println("LoRa init failed. Check your connections.");
    while (true);                       // if failed, do nothing
  }

  Serial.println("LoRa init succeeded.");
}

void loop() {
  String incoming = "";
  onReceive(LoRa.parsePacket());// parse for a packet, and call onReceive with the result:
  telemetriparcala(hamveri); //NodeID[0]->RSSI, 
                             //NodeID[1]->Latitude-Enlem,
                             //NodeID[2]->Longtitude-Boylam,
                             //NodeID[3]->Altitude-Yükseklik,
                             //NodeID[4]->msgCount-Paket sayısı
  uzaklikolcum();            //NodeID[5]->GPS-Node Uzaklığı
  gpsX_Matrix();
  gpsY_Matrix();
  gpsZ_Matrix();
  gpsX_det();
  gpsY_det();
  gpsZ_det();
  
  //Node1_info();
  //Node2_info();
  //Node3_info();
  //Node4_info();
  
  Serial.println(hamveri);
  delay(40);
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
  String incoming = "";                 // incoming

  while (LoRa.available()) {incoming += (char)LoRa.read();}

  if (incomingLength != incoming.length()) {Serial.println("error: message length does not match length");return;}
  if (recipient != localAddress && recipient != 0xFF) {Serial.println("This message is not for me."); return;}
  
  // if message is for this device, or broadcast, print details:
  
  /*
  Serial.println("Received from: 0x" + String(sender, HEX));
  Serial.println("Sent to: 0x" + String(recipient, HEX));
  Serial.println("Message ID: " + String(incomingMsgId));
  Serial.println("Message length: " + String(incomingLength));
  Serial.println("Message: " + incoming);
  Serial.println("RSSI: " + String(LoRa.packetRssi()));
  Serial.println("Snr: " + String(LoRa.packetSnr()));
  Serial.println();
  */
  hamveri = incoming;
  Serial.println(hamveri);
}

void telemetriparcala(String incoming){
  int ID[1];
  ID[0] = incoming.substring(0,1).toInt();
  Serial.println(ID[0]);
  if (ID[0] == 1){                // Node 1 için
    Node1[0] = LoRa.packetRssi();                   // RSSI Değeri
    Node1[1] = incoming.substring(1,5).toInt() - 1000;     // Latitude-Enlem
    Node1[2] = incoming.substring(5,9).toInt() - 1000;    // Longtitude-Boylam
    Node1[3] = incoming.substring(9,13).toInt() - 1000;   // Altitude-Yükseklik
    Node1[4] = incoming.substring(13,20).toInt();   // msgCount-Paket sayısı
    Serial.println("Node1 için degerler -> RSSI: " + String(Node1[0]) + "X: " + String(Node1[1]) + "Y: " + String(Node1[2]) + "Z: " + String(Node1[3]) + "Count: " + String(Node1[4]));

/*
    //Node 2 İçin Fake Değerler
    //Node2[0] = ;
    Node2[1] = 81; 
    Node2[2] = 2;
    Node2[3] = 103;
    Node2[4] = 40;
    //Node2[5] = 40;
*/   
  }
  else if (ID[0] == 2){                // Node 2 için
    Node2[0] = LoRa.packetRssi();                   // RSSI Değeri
    Node2[1] = incoming.substring(1,5).toInt() - 2000;     // Latitude-Enlem
    Node2[2] = incoming.substring(5,9).toInt() - 2000;    // Longtitude-Boylam
    Node2[3] = incoming.substring(9,13).toInt() - 2000;   // Altitude-Yükseklik
    Node2[4] = incoming.substring(13,20).toInt();   // msgCount-Paket sayısı
    Serial.println("Node2 için degerler -> RSSI: " + String(Node2[0]) + "X: " + String(Node2[1]) + "Y: " + String(Node2[2]) + "Z: " + String(Node2[3]) + "Count: " + String(Node2[4]));
  }
  else if (ID[0] == 3){                // Node 3 için
    Node3[0] = LoRa.packetRssi();                   // RSSI Değeri
    Node3[1] = incoming.substring(1,5).toInt() - 3000;     // Latitude-Enlem
    Node3[2] = incoming.substring(5,9).toInt() - 3000;    // Longtitude-Boylam
    Node3[3] = incoming.substring(9,13).toInt() - 3000;   // Altitude-Yükseklik
    Node3[4] = incoming.substring(13,20).toInt();   // msgCount-Paket sayısı
    Serial.println("Node3 için degerler -> RSSI: " + String(Node3[0]) + "X: " + String(Node3[1]) + "Y: " + String(Node3[2]) + "Z: " + String(Node3[3]) + "Count: " + String(Node3[4]));
  }
  else if (ID[0] == 4){                // Node 4 için
    Node4[0] = LoRa.packetRssi();                   // RSSI Değeri
    Node4[1] = incoming.substring(1,5).toInt() - 4000;     // Latitude-Enlem
    Node4[2] = incoming.substring(5,9).toInt() - 4000;    // Longtitude-Boylam
    Node4[3] = incoming.substring(9,13).toInt() - 4000;   // Altitude-Yükseklik
    Node4[4] = incoming.substring(13,20).toInt();   // msgCount-Paket sayısı
    Serial.println("Node4 için degerler -> RSSI: " + String(Node4[0]) + "X: " + String(Node4[1]) + "Y: " + String(Node4[2]) + "Z: " + String(Node4[3]) + "Count: " + String(Node4[4]));
  }
  else{
    Serial.println("**********Yanlış ID**********");
  }
}
void uzaklikolcum(){
  float ratio1 = (MPwr1 - float(Node1[0]))/(10 * N1);
  Node1[5] = (pow(10,ratio1));

  float ratio2 = (MPwr2 - float(Node2[0]))/(10 * N2);
  Node2[5] = (pow(10,ratio2));

  float ratio3 = (MPwr3 - float(Node3[0]))/(10 * N3);
  Node3[5] = (pow(10,ratio3));

  float ratio4 = (MPwr4 - float(Node4[0]))/(10 * N4);
  Node4[5] = (pow(10,ratio4));
}

void gpsX_Matrix(){
  payX[0][0] = ((sq(Node1[5])-sq(Node2[5])) - (sq(Node1[1])-sq(Node2[1])) - (sq(Node1[2])-sq(Node2[2])) - (sq(Node1[3])-sq(Node2[3])));
  payX[0][1] = 2 * (Node2[2]-Node1[2]);
  payX[0][2] = 2 * (Node2[3]-Node1[3]);
  payX[1][0] = ((sq(Node1[5])-sq(Node3[5])) - (sq(Node1[1])-sq(Node3[1])) - (sq(Node1[2])-sq(Node3[2])) - (sq(Node1[3])-sq(Node3[3])));
  payX[1][1] = 2 * (Node3[2]-Node1[2]);
  payX[1][2] = 2 * (Node3[3]-Node1[3]);
  payX[2][0] = ((sq(Node1[5])-sq(Node4[5])) - (sq(Node1[1])-sq(Node4[1])) - (sq(Node1[2])-sq(Node4[2])) - (sq(Node1[3])-sq(Node4[3])));
  payX[2][1] = 2 * (Node4[2]-Node1[2]);
  payX[2][2] = 2 * (Node4[3]-Node1[3]);

  paydaX[0][0] = 2 * (Node2[1]-Node1[1]);
  paydaX[0][1] = payX[0][1];
  paydaX[0][2] = payX[0][2];
  paydaX[1][0] = 2 * (Node3[1]-Node1[1]);
  paydaX[1][1] = payX[1][1];
  paydaX[1][2] = payX[1][2];
  paydaX[2][0] = 2 * (Node4[1]-Node1[1]);
  paydaX[2][1] = payX[2][1];
  paydaX[2][2] = payX[2][2];
}

void gpsY_Matrix(){
  payY[0][0] = 2 * (Node2[1]-Node1[1]);
  payY[0][1] = ((sq(Node1[5])-sq(Node2[5])) - (sq(Node1[1])-sq(Node2[1])) - (sq(Node1[2])-sq(Node2[2])) - (sq(Node1[3])-sq(Node2[3])));
  payY[0][2] = 2 * (Node2[3]-Node1[3]);
  payY[1][0] = 2 * (Node3[1]-Node1[1]);
  payY[1][1] = ((sq(Node1[5])-sq(Node3[5])) - (sq(Node1[1])-sq(Node3[1])) - (sq(Node1[2])-sq(Node3[2])) - (sq(Node1[3])-sq(Node3[3])));
  payY[1][2] = 2 * (Node3[3]-Node1[3]);
  payY[2][0] = 2 * (Node4[1]-Node1[1]);
  payY[2][1] = ((sq(Node1[5])-sq(Node4[5])) - (sq(Node1[1])-sq(Node4[1])) - (sq(Node1[2])-sq(Node4[2])) - (sq(Node1[3])-sq(Node4[3])));
  payY[2][2] = 2 * (Node4[3]-Node1[3]);

  paydaY[0][0] = payY[0][0];
  paydaY[0][1] = 2 * (Node2[2]-Node1[2]); 
  paydaY[0][2] = payY[0][2];
  paydaY[1][0] = payY[1][0];
  paydaY[1][1] = 2 * (Node3[2]-Node1[2]);
  paydaY[1][2] = payY[1][2];
  paydaY[2][0] = payY[2][0];
  paydaY[2][1] = 2 * (Node4[2]-Node1[2]);
  paydaY[2][2] = payY[2][2];
}

void gpsZ_Matrix(){
  payZ[0][0] = 2 * (Node2[1]-Node1[1]);
  payZ[0][1] = 2 * (Node2[2]-Node1[2]);
  payZ[0][2] = ((sq(Node1[5])-sq(Node2[5])) - (sq(Node1[1])-sq(Node2[1])) - (sq(Node1[2])-sq(Node2[2])) - (sq(Node1[3])-sq(Node2[3])));
  payZ[1][0] = 2 * (Node3[1]-Node1[1]);
  payZ[1][1] = 2 * (Node3[2]-Node1[2]);
  payZ[1][2] = ((sq(Node1[5])-sq(Node3[5])) - (sq(Node1[1])-sq(Node3[1])) - (sq(Node1[2])-sq(Node3[2])) - (sq(Node1[3])-sq(Node3[3])));
  payZ[2][0] = 2 * (Node4[1]-Node1[1]);
  payZ[2][1] = 2 * (Node4[2]-Node1[2]);
  payZ[2][2] = ((sq(Node1[5])-sq(Node4[5])) - (sq(Node1[1])-sq(Node4[1])) - (sq(Node1[2])-sq(Node4[2])) - (sq(Node1[3])-sq(Node4[3])));

  paydaZ[0][0] = payZ[0][0];
  paydaZ[0][1] = payZ[0][1];
  paydaZ[0][2] = 2 * (Node2[3]-Node1[3]);
  paydaZ[1][0] = payZ[1][0];
  paydaZ[1][1] = payZ[1][1];
  paydaZ[1][2] = 2 * (Node3[3]-Node1[3]);
  paydaZ[2][0] = payZ[2][0];
  paydaZ[2][1] = payZ[2][1];
  paydaZ[2][2] = 2 * (Node4[3]-Node3[3]);
}

void gpsX_det(){
  gpsXpay = ((payX[0][0]*payX[1][1]*payX[2][2])+(payX[0][1]*payX[1][2]*payX[2][0])+(payX[0][2]*payX[1][0]*payX[2][1]));
  gpsXpay = gpsXpay - ((payX[2][0]*payX[1][1]*payX[0][2])+(payX[2][1]*payX[1][2]*payX[0][0])+(payX[2][2]*payX[1][0]*payX[0][1]));

  gpsXpayda = ((paydaX[0][0]*paydaX[1][1]*paydaX[2][2])+(paydaX[0][1]*paydaX[1][2]*paydaX[2][0])+(paydaX[0][2]*paydaX[1][0]*paydaX[2][1]));
  gpsXpayda = gpsXpayda - ((paydaX[2][0]*paydaX[1][1]*paydaX[0][2])+(paydaX[2][1]*paydaX[1][2]*paydaX[0][0])+(paydaX[2][2]*paydaX[1][0]*paydaX[0][1]));

  if(gpsXpayda == 0){
    gpsXpayda = 1;
  }
  gpsX = gpsXpay / gpsXpayda;
  Serial.println(gpsX);
  return;
}

void gpsY_det(){
  gpsYpay = ((payY[0][0]*payY[1][1]*payY[2][2])+(payY[0][1]*payY[1][2]*payY[2][0])+(payY[0][2]*payY[1][0]*payY[2][1]));
  gpsYpay = gpsYpay - ((payY[2][0]*payY[1][1]*payY[0][2])+(payY[2][1]*payY[1][2]*payY[0][0])+(payY[2][2]*payY[1][0]*payY[0][1]));

  gpsYpayda = ((paydaY[0][0]*paydaY[1][1]*paydaY[2][2])+(paydaY[0][1]*paydaY[1][2]*paydaY[2][0])+(paydaY[0][2]*paydaY[1][0]*paydaY[2][1]));
  gpsYpayda = gpsYpayda - ((paydaY[2][0]*paydaY[1][1]*paydaY[0][2])+(paydaY[2][1]*paydaY[1][2]*paydaY[0][0])+(paydaY[2][2]*paydaY[1][0]*paydaY[0][1]));

  if(gpsYpayda == 0){
    gpsYpayda = 1;
  }
  
  gpsY = gpsYpay / gpsYpayda;
  Serial.println(gpsY);
  return;
}

void gpsZ_det(){
  gpsZpay = ((payZ[0][0]*payZ[1][1]*payZ[2][2])+(payZ[0][1]*payZ[1][2]*payZ[2][0])+(payZ[0][2]*payZ[1][0]*payZ[2][1]));
  gpsZpay = gpsZpay - ((payZ[2][0]*payZ[1][1]*payZ[0][2])+(payZ[2][1]*payZ[1][2]*payZ[0][0])+(payZ[2][2]*payZ[1][0]*payZ[0][1]));

  gpsZpayda = ((paydaZ[0][0]*paydaZ[1][1]*paydaZ[2][2])+(paydaZ[0][1]*paydaZ[1][2]*paydaZ[2][0])+(paydaZ[0][2]*paydaZ[1][0]*paydaZ[2][1]));
  gpsZpayda = gpsZpayda - ((paydaZ[2][0]*paydaZ[1][1]*paydaZ[0][2])+(paydaZ[2][1]*paydaZ[1][2]*paydaZ[0][0])+(paydaZ[2][2]*paydaZ[1][0]*paydaZ[0][1]));

  if(gpsZpayda == 0){
    gpsZpayda = 1;
  }
  
  gpsZ = gpsZpay / gpsZpayda;
  Serial.println(gpsZ);
  return;
}

void Node1_info(){
  Serial.println("Node1 için RSSI degeri " + String(Node1[0],DEC));
  Serial.println("Node1 uzaklığı " + String(Node1[5],DEC) + "m");
  Serial.println("Node1 konumu (x,y,z)" + String(Node1[1],DEC) + "," + String(Node1[2],DEC) + "," + String(Node1[3],DEC));
  Serial.println();
  delay(10);
}

void Node2_info(){
  Serial.println("Node2 için RSSI degeri " + String(Node2[0],DEC));
  Serial.println("Node2 uzaklığı " + String(Node2[5],DEC) + "m");
  Serial.println("Node2 konumu (x,y,z)" + String(Node2[1],DEC) + "," + String(Node2[2],DEC) + "," + String(Node2[3],DEC));
  Serial.println();
  delay(10);
}

void Node3_info(){
  Serial.println("Node3 için RSSI degeri " + String(Node3[0],DEC));
  Serial.println("Node3 uzaklığı " + String(Node3[5],DEC) + "m");
  Serial.println("Node3 konumu (x,y,z)" + String(Node3[1],DEC) + "," + String(Node3[2],DEC) + "," + String(Node3[3],DEC));
  Serial.println();
  delay(10);
}

void Node4_info(){
  Serial.println("Node4 için RSSI degeri " + String(Node4[0],DEC));
  Serial.println("Node4 uzaklığı " + String(Node4[5],DEC) + "m");
  Serial.println("Node4 konumu (x,y,z)" + String(Node4[1],DEC) + "," + String(Node4[2],DEC) + "," + String(Node4[3],DEC));
  Serial.println();
  delay(10);
}
