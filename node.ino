//c 
#include <SoftwareSerial.h>

#define HC12RxdPin 4
#define HC12TxdPin 5
#define DT_LISTEN_ACK 2000

char ID='2';
String sinthima;
char payload[4];
byte fcs;
unsigned long t1;

SoftwareSerial HC12( HC12TxdPin , HC12RxdPin );

void setup() {
 randomSeed(analogRead(0)); 
 Serial.begin(9600);
 HC12.begin(9600);
}

void loop() {
  delay(random(0,1000));
  createPayloadAndComputeFcs();
  while(true){
    txPacket(payload,fcs);
    if (!elabaACKstoDt()){
       delay(random(0,2000));//random delay
    }else{ 
       break;
    }
  } 
}

void createPayloadAndComputeFcs(){
  sinthima="paok";
  int i;
  //compute the FCS.XOR as java diktya1
  sinthima.toCharArray(payload,4);
  fcs=payload[0]^payload[1];
  for(i=2;i<4;i++){
    fcs=fcs^payload[i];  
  }  
}

void txPacket(  char  *payload , byte fcs  ){
  //Tx the packet=header+payload+fcs+P
  int i;
  t1=millis();
  HC12.print("PSTART");
  HC12.write(ID);
  for(i=0;i<4;i++){
    HC12.write(payload[i]);  
  }
  HC12.write(fcs);
  HC12.print("PSTOP");  
}


bool elabaACKstoDt(){
   //listen for ACK gia Δt=2000ms . ACK=255 
   String delimiter="";
   char tempByte;
   byte fcsRx;
   char ID;
   while(millis()-t1<DT_LISTEN_ACK){
      if(HC12.available()){
         tempByte=HC12.read();
         delimiter+=String(tempByte);
         if (delimiter=="ASTART"){
            //elaba ack gia mena h gia kapoion allon kombo
            //diabazo to ack
            while(true){
              if (HC12.available()){
                ID=HC12.read();
                break; 
              }
            }
            while(true){
              if (HC12.available()){
                fcsRx=HC12.read();
                break; 
              }
            }
             //htan ack gia mena?
            if (ID=='2'){
               //to header htan to diko moy  
               //an elaba ack
               if(fcsRx==255){
                 return true; 
               }else{
                   delimiter=""; 
               }
            }else{
               delimiter="";
            }           
         }else if (!((delimiter=="A")||(delimiter=="AS")||(delimiter=="AST")||(delimiter=="ASTA")||(delimiter=="ASTAR"))){
                 delimiter=""; 
         }
      }
    }          
  return false;
}
