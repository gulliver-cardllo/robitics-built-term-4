
#define SS_PIN 10  
#define RST_PIN 9
#include <SPI.h>      
#include <MFRC522.h>
#include <QTRSensors.h>
MFRC522 mfrc522(SS_PIN, RST_PIN);  // instantiate a MFRC522 reader object.
MFRC522::MIFARE_Key key;  
QTRSensors qtrD;
QTRSensors qtr;
const uint8_t SensorCount = 3;
uint16_t sensorValues[SensorCount];
int frequency = 0;
int block=2;  
byte blockcontent[16] = {"Test-RFID"};
byte readbackblock[18];
const int In1 = 8;
const int In3 = 5;
const int S0 = A4;
const int S1 = A5 ;
const int S2 = A2;
const int S3 = A1;
const int sensorOut = A4;
const int OE = A0 ;



void setup() {
  pinMode(In1, OUTPUT);
  pinMode(In3, OUTPUT);

  pinMode(S0, OUTPUT);
  pinMode(S1, OUTPUT);
  pinMode(S2, OUTPUT);
  pinMode(S3, OUTPUT);

  pinMode(sensorOut, INPUT);
  digitalWrite(S0, HIGH);
  digitalWrite(S1, LOW);


  qtr.setTypeRC();
  qtr.setSensorPins((const uint8_t[]){4, 3, 2}, SensorCount);
  Serial.begin(9600);
  Serial.print("Test ");

    
//  for (uint16_t i = 0; i < 400; i++){
  //  qtrD.calibrate();
   // Serial.print("Calibrated: ");
   // Serial.println(i);
  
 // }
  
    Serial.begin(9600);        // Initialize serial communications with the PC
    SPI.begin();               // Init SPI bus
    mfrc522.PCD_Init();        // Init MFRC522 card (in case you wonder what PCD means: proximity coupling device)
    Serial.println("Scan a MIFARE Classic card");


  for (byte i = 0; i < 6; i++) {

    key.keyByte[i] = 0xFF;  //keyByte is defined in the "MIFARE_Key" 'struct' definition in the .h file of the library

  }


 

}

void colour() {
  // Red Colour Code
  Serial.print("R= ");      
  Serial.print(frequency); 
  Serial.print("  ");
  delay(100);
  // Green Colour Code
  digitalWrite(S2, HIGH);
  digitalWrite(S3, HIGH);
  frequency = pulseIn(sensorOut, LOW);
  Serial.print("G= ");      
  Serial.print(frequency);  
  Serial.print("  ");
  delay(100);
  // Blue Colour Code
  digitalWrite(S2, LOW);
  digitalWrite(S3, HIGH);
  frequency = pulseIn(sensorOut, LOW);
  Serial.print("B= ");      
  Serial.print(frequency);  
  Serial.println("  ");
  delay(1000);
}

void loop() {
  /*
  stayontheline();
    uint16_t position = qtr.readLineBlack(sensorValues);
    qtr.read(sensorValues);
    for (uint8_t i = 0; i < SensorCount; i++) {
      Serial.print(sensorValues[i]);
      Serial.println('\t');
    }
    Serial.print("Postion: ");
    Serial.println(position);
    Serial.println();
    delay(1000);
*/

    
  







if ( ! mfrc522.PICC_IsNewCardPresent()) {

    return;

  }



  // Select one of the cards

  if ( ! mfrc522.PICC_ReadCardSerial()) 

  {

    return;

  }

    Serial.println("card selected"); 

   

   //the blockcontent array is written into the card block

   writeBlock(block, blockcontent);



   //read the block back

   readBlock(block, readbackblock);



   //uncomment below line if you want to see the entire 1k memory with the block written into it.

   mfrc522.PICC_DumpToSerial(&(mfrc522.uid));

}



//Write specific block    

int writeBlock(int blockNumber, byte arrayAddress[]) 

{

  //this makes sure that we only write into data blocks. Every 4th block is a trailer block for the access/security info.

  int largestModulo4Number=blockNumber/4*4;

  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector



  if (blockNumber > 2 && (blockNumber+1)%4 == 0){Serial.print(blockNumber);Serial.println(" is a trailer block:");return 2;}

  Serial.print(blockNumber);

  Serial.println(" is a data block:");



  //authentication of the desired block for access

  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));

  

  if (status != MFRC522::STATUS_OK) {

         Serial.print("PCD_Authenticate() failed: ");

         Serial.println(mfrc522.GetStatusCodeName(status));

         return 3;//return "3" as error message

  }

  //writing the block 

  status = mfrc522.MIFARE_Write(blockNumber, arrayAddress, 16);



  //status = mfrc522.MIFARE_Write(9, value1Block, 16);

  if (status != MFRC522::STATUS_OK) {

           Serial.print("MIFARE_Write() failed: ");

           Serial.println(mfrc522.GetStatusCodeName(status));

           return 4;//return "4" as error message

  }

  Serial.println("block was written");

}
//Read specific block

int readBlock(int blockNumber, byte arrayAddress[]) 

{

  int largestModulo4Number=blockNumber/4*4;

  int trailerBlock=largestModulo4Number+3;//determine trailer block for the sector

  byte status = mfrc522.PCD_Authenticate(MFRC522::PICC_CMD_MF_AUTH_KEY_A, trailerBlock, &key, &(mfrc522.uid));



  if (status != MFRC522::STATUS_OK) {

         Serial.print("PCD_Authenticate() failed (read): ");

         Serial.println(mfrc522.GetStatusCodeName(status));

         return 3;//return "3" as error message

  }
//reading a block
byte buffersize = 18;//we need to define a variable with the read buffer size, since the MIFARE_Read method below needs a pointer to the variable that contains the size... 
status = mfrc522.MIFARE_Read(blockNumber, arrayAddress, &buffersize);//&buffersize is a pointer to the buffersize variable; MIFARE_Read requires a pointer instead of just a number

  if (status != MFRC522::STATUS_OK) {

          Serial.print("MIFARE_read() failed: ");

          Serial.println(mfrc522.GetStatusCodeName(status));

          return 4;//return "4" as error message

  }

  Serial.println("block was read");

}

void stayontheline(){
  uint16_t position = qtr.readLineBlack(sensorValues);
  if (position > 500 && position < 1000){
    digitalWrite(In1, HIGH);  
    digitalWrite(In3, LOW);
    delay(20);
    digitalWrite(In1, LOW);
    digitalWrite(In3, LOW);
  }

  if (position < 500){
    digitalWrite(In1, LOW);  
    digitalWrite(In3, HIGH);
    delay(20);
    digitalWrite(In1, LOW);
    digitalWrite(In3, LOW);
  }
  if (position > 1100){

    digitalWrite(In1, HIGH);
    digitalWrite(In3, HIGH);
    Serial.println(position);
    digitalWrite(In1, LOW);
    digitalWrite(In3, LOW);
  }

}
void sensorfunc() {

  // this code can be updated to use the built in location function in qtr
  uint16_t position = qtr.readLineBlack(sensorValues);
  Serial.print("Position: ");
  Serial.println(position);
  delay(500);
  
}
