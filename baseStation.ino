/*base station για c part 
 * δυο κομβοι στελνουν στο σταθμο βασης 
 * πακετα και καταγραφουμε τον αριθμο
 * των collisions που συμβαινουν
 */

#include <SoftwareSerial.h>

#define HC12RxdPin 4
#define HC12TxdPin 5

SoftwareSerial HC12( HC12TxdPin , HC12RxdPin );

char ID_HEADER;
char payload[4];//θα ειναι το "paok"
byte fcsRx;
unsigned long collisionCounter=0L;
unsigned long TotalPacketsRx=0L;

void setup() {
  Serial.begin(9600);
  HC12.begin(9600);
}

void loop() {
  listenUntilRxApacket();
  if (computeFcs()== fcsRx){
       //send ACK
       TotalPacketsRx=TotalPacketsRx+1L;
       HC12.print("ASTART");
       HC12.write(ID_HEADER);
       HC12.write(255);
       HC12.print("ASTOP");
       //print sto serial monitor
       Serial.println("Εγινε λήψη πακέτου");
       Serial.print("packets Rx : ");Serial.println(TotalPacketsRx);
       Serial.print("collisions : ");Serial.println(collisionCounter);
       Serial.println("----------------------------------");
  }else{
      TotalPacketsRx=TotalPacketsRx+2L;
      collisionCounter=collisionCounter+1L;
      Serial.println("Εγινε collision");
      Serial.print("packets Rx : ");Serial.println(TotalPacketsRx);
      Serial.print("collisions : ");Serial.println(collisionCounter);
      Serial.println("------------------------------ ");    
  }
}

void listenUntilRxApacket(){
  int counter=0;
  char tempChar;
  String delimiter="";
  while(true){
     if(HC12.available()){
        tempChar=HC12.read();
        delimiter+=String(tempChar);
        if (delimiter=="PSTART"){
           while(true){
            if (HC12.available()){ 
             tempChar=HC12.read();
             ID_HEADER=tempChar;
             break; 
            } 
           }
           while(counter<4){
            if (HC12.available()){
              payload[counter]=HC12.read();
              counter++;
            }  
           }
           while(true){
             if (HC12.available()){
                fcsRx=HC12.read();
                //Serial.print(fcsRx);
                break;  
             } 
           }
          return;  
       }else if (delimiter=="PP"){
          delimiter="P"; 
       }else if (!((delimiter=="P")||(delimiter=="PS")||(delimiter=="PST")||(delimiter=="PSTA")||(delimiter=="PSTAR"))){
           delimiter="";  
       }
     } 
  }
}

byte computeFcs(){
  int i;
  byte fcs=payload[0]^payload[1];
  for (i=0;i<4;i++){
  //Serial.print(payload[i]);}
  for(i=2;i<sizeof(payload);i++){
    fcs=fcs^payload[i];  
  }  
  //Serial.print(fcs);
  return fcs;
}}
