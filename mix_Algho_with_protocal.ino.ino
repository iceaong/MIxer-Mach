//////////////////////////////////////////////////////////////////
//©2011 bildr
//Released under the MIT License - Please reuse change and share
//Using the easy stepper with your arduino
//use rotate and/or rotateDeg to controll stepper motor
//speed is any number from .01 -> 1 with 1 being fastest - 
//Slower Speed == Stronger movement
/////////////////////////////////////////////////////////////////

#define DIR_PIN 8
#define STEP_PIN 6
#define ENA_PIN 7
#define Limit_PIN 9
#define M1 2
#define M2 3 
#define M3 4
#define M4 5
 
int distance=13000;
//ทิศ + หมุนเข้าหา limitswitch,- หมุนไปยังจุดเสริฟ
int p=0,old=0,Div=0;
int time_P=0;//ตัวซ้ำ
int error=0;

const uint8_t header = 0x23;
const uint8_t bufferSize = 7;

char buffer[bufferSize]={0};
uint8_t readCounter;
uint8_t isHeader;

//Flag that helps us restart counter when we first find header byte
uint8_t firstTimeHeader; 



void setup() { 
  pinMode(DIR_PIN, OUTPUT); 
  pinMode(STEP_PIN, OUTPUT); 
  pinMode(ENA_PIN, OUTPUT);
  pinMode(Limit_PIN, INPUT_PULLUP);
  pinMode(M1,OUTPUT);
  pinMode(M2,OUTPUT);
  pinMode(M3,OUTPUT);
  pinMode(M4,OUTPUT);
  Serial.begin(9600);
   while (!Serial) ; // wait for serial port to connect. Needed for native USB port only
  readCounter = 0;
  isHeader = 0;
  firstTimeHeader = 0;
  for(int i=0;i<5;i++){
    Serial.print("Point  ");
    Serial.print(i+1);Serial.print(" : ");
    Serial.println((distance/5)*(i+1));
  } 
  digitalWrite(ENA_PIN, LOW);
  Serial.println("----------------------------------Ready To Reserlve Protocal----------------------------");
} 

 

void loop(){ 
  //Check if there is any data available to read
  if(Serial.available() > 0){
    //read only one byte at a time
    uint8_t inChar = Serial.read(); 
     //Check if header is found
    if(inChar == header){
      //We must consider that we may sometimes receive unformatted data, and
      //given the case we must ignore it and restart our reading code.
      //If it's the first time we find the header, we restart readCounter
      //indicating that data is coming.
      //It's possible the header appears again as a data byte. That's why
      //this conditional is implemented, so that we don't restart readCounter
      //and corrupt the data. 
      if(!firstTimeHeader){
        isHeader = 1;
        readCounter = 0;
        firstTimeHeader = 1;
      }
    }
    
    //store received byte, increase readCounter
    buffer[readCounter] = inChar;
    Serial.print(buffer[readCounter]);
    readCounter++;
    
    //prior overflow, we have to restart readCounter
    if(readCounter <= bufferSize && inChar == ';'){
      if(buffer[readCounter-1]==';'&&(buffer[readCounter-2]=='E'||buffer[readCounter-2]=='e')){
        //if header was found
        if(isHeader){
          //Your code
          Serial.print("  OK");
          Serial.print("-----> Chang : ");
          for(int i=0;i<readCounter;i++){
              if(i==0||i==readCounter-1)
                Serial.print(buffer[i]);
              else if(i>0&&i<readCounter){
                if(buffer[i]>=65&&buffer[i]<70)
                  Serial.print(buffer[i]-64);
                else if (buffer[i]>=97&&buffer[i]<102)
                  Serial.print(buffer[i]-96);
              } 
          }
          Serial.print("\nNumber OF MIXUR : ");Serial.println(readCounter-3);
          while(1){
            if(digitalRead(Limit_PIN)==1){
              rotate(10, .10);
            }
            else{
              delay(1000); 
              break;    
            }   
          }
            for(int i=1;i<readCounter-1;i++){
              if(buffer[i]>=65&&buffer[i]<70)
                  error = 64;
              else if (buffer[i]>=97&&buffer[i]<102)
                  error = 96;
              if(i==1){
                old=0;
                Div=0;
                p=0;
              }    
              else{
                if((old>=0&&old<6)&&((buffer[i-1]-1)-(error-1)>=0&&(buffer[i-1]-1)-(error-1)<6))
                  old=(buffer[i-1]-1)-(error-1);
                else if(error==64)
                  old=(buffer[i-1]-33)-(error-1);
                else if(error==96)
                  old=(buffer[i-1]-1)-(error-33);
               Div=abs(buffer[i]-old-error);
               p=(distance/5)*Div;
              }
              Serial.print("Point is ");Serial.println(buffer[i]-error);
              Serial.print("Old : ");Serial.println(old);
              Serial.print("Differene Point : ");Serial.println(Div);
              if(buffer[i]=='A'||buffer[i]=='a'){
                if(i>1){//ผ่าน limit ล่ะ
                  if(buffer[i-1]=='B'||buffer[i-1]=='C'||buffer[i-1]=='D'||buffer[i-1]=='E'||buffer[i-1]=='b'||buffer[i-1]=='c'||buffer[i-1]=='d'||buffer[i-1]=='e'){        
                    Serial.print(p);Serial.println(" direction +");
                    rotate(p, .15);
                    }
                  else
                    time_P=1000;
                }
                else{ //i==1
                  Serial.print(distance/5);Serial.println(" direction -");
                  rotate(-distance/5, .15);
                }
              }
              else if(buffer[i]=='B'||buffer[i]=='b'){
                if(i>1){//ผ่าน limit ล่ะ     
                  Serial.print(p);
                  if(buffer[i-1]=='A'||buffer[i-1]=='a'){
                    Serial.println(" direction -");rotate(-p, .15);
                  }
                  else if (buffer[i-1]=='C'||buffer[i-1]=='D'||buffer[i-1]=='E'||buffer[i-1]=='c'||buffer[i-1]=='d'||buffer[i-1]=='e'){
                    Serial.println(" direction +");rotate(p, .15);
                  }
                  else
                    time_P=1000;
                }
                else{ //i==1
                  Serial.print(((distance/5)*2));Serial.println(" direction -");
                  rotate(-((distance/5)*2), .15);
                }
              }
              
              else if(buffer[i]=='C'||buffer[i]=='c'){
                if(i>1){//ผ่าน limit ล่ะ
                  Serial.print(p);
                  if(buffer[i-1]=='A'||buffer[i-1]=='B'||buffer[i-1]=='a'||buffer[i-1]=='b'){
                    Serial.println(" direction -");rotate(-p, .15);
                  }
                  else if(buffer[i-1]=='D'||buffer[i-1]=='E'||buffer[i-1]=='d'||buffer[i-1]=='e'){
                    Serial.println(" direction +");rotate(p, .15);
                  }
                  else
                    time_P=1000;
                }
                else{ //i==1
                  Serial.print(((distance/5)*3));Serial.println(" direction -");
                  rotate(-((distance/5)*3), .15);
                }
              }
              else if(buffer[i]=='D'||buffer[i]=='d'){
                if(i>1){//ผ่าน limit ล่ะ
                  Serial.print(p);
                  if(buffer[i-1]=='A'||buffer[i-1]=='B'||buffer[i-1]=='C'||buffer[i-1]=='a'||buffer[i-1]=='b'||buffer[i-1]=='c'){
                    Serial.println(" direction -");rotate(-p, .15);
                  }
                  else if(buffer[i-1]=='E'){
                    Serial.println(" direction +");rotate(p, .15);
                  }
                  else
                    time_P=1000;
                }
                else{ //i==1
                  Serial.print(((distance/5)*4));Serial.println(" direction -");
                  rotate(-((distance/5)*4), .15);
                }
              }
               else if(buffer[i]=='E'||buffer[i]=='e'){
                if(i>1){//ผ่าน limit ล่ะ
                  if(buffer[i-1]=='A'||buffer[i-1]=='B'||buffer[i-1]=='C'||buffer[i-1]=='D'||buffer[i-1]=='a'||buffer[i-1]=='b'||buffer[i-1]=='c'||buffer[i-1]=='d'){
                  Serial.print(p);Serial.println(" direction -");
                  rotate(-p, .15);
                  }
                }
                else{ //i==1
                  Serial.print(distance);Serial.println(" direction -");
                  rotate(-distance, .15);
                }
              }
              //rotate(-distance/5, .15);
              //Serial.println(distance/(5-i));
              Motor_PUM(buffer[i]); 
              delay(2000+time_P);
              Motor_PUM('E'); 
              time_P=0;
            } 
             Serial.print("Complece\n");
             Serial.println("----------------------------------Ready To Reserlve Protocal----------------------------");
             readCounter = 0;
          
          //restart header flag
          isHeader = 0;
          firstTimeHeader = 0;
        }
        else{
        Serial.println("ERROR No Header '#' protocal or Header potocal are mismake.");
        isHeader = 0;
        readCounter = 0;
        firstTimeHeader = 0;
        }
      }
      else{
        Serial.println("ERROR No close protocal or close protocal are mismake.");
        isHeader = 0;
        readCounter = 0;
        firstTimeHeader = 0;
      }
    }
    else if(readCounter > bufferSize){
      Serial.println("ERROR Protocal has long and mismake.");
      isHeader = 0;
      readCounter = 0;
      firstTimeHeader = 0;
    }
  }
}


/*while(1){
        if(digitalRead(Limit_PIN)==1){
          rotate(10, .10);
        }
        else{
          delay(1000); 
          break;    
        }   
  }
  /*for(int i=0;i<5;i++){
    rotate(-distance/5, .15);
    Serial.println(distance/(5-i));
    Motor_PUM(i+1); 
    delay(2000);
    Motor_PUM(5); 
  } 
  */
  /*
  //rotate a specific number of degrees 
       rotateDeg(360, 1); 
       delay(1000);

 
       rotateDeg(-360, .1);  //reverse
       delay(1000); 

 
  //rotate a specific number of microsteps (8 microsteps per step)
  //a 200 step stepper would take 1600 micro steps for one full revolution
       rotate(1600, .5); 
       delay(1000); 

 

       rotate(-1600, .25); //reverse
       delay(1000); 

}*/

void rotate(int steps, float speed){ 
  //rotate a specific number of microsteps (8 microsteps per step) - (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger

       int dir = (steps > 0)? HIGH:LOW;
       steps = abs(steps); 
       digitalWrite(DIR_PIN,dir); 
       float usDelay = (1/speed) * 70;
       for(int i=0; i < steps; i++){ 
       digitalWrite(STEP_PIN, HIGH); 
       delayMicroseconds(usDelay); 
       digitalWrite(STEP_PIN, LOW); 
       delayMicroseconds(usDelay); 
  } 
} 

 

void rotateDeg(float deg, float speed){ 
  //rotate a specific number of degrees (negitive for reverse movement)
  //speed is any number from .01 -> 1 with 1 being fastest - Slower is stronger
       int dir = (deg > 0)? HIGH:LOW;
       digitalWrite(DIR_PIN,dir); 
       int steps = abs(deg)*(1/0.225);
       float usDelay = (1/speed) * 70;
       for(int i=0; i < steps; i++){ 
       digitalWrite(STEP_PIN, HIGH); 
       delayMicroseconds(usDelay); 
       digitalWrite(STEP_PIN, LOW); 
       delayMicroseconds(usDelay); 
  } 
}

void Motor_PUM(char No){
    if(No == 'A'||No == 'a'){
      Serial.println("M1");
      digitalWrite(M1, HIGH);
      digitalWrite(M2, LOW);
      digitalWrite(M3, LOW);
      digitalWrite(M4, LOW);
    }
    else if(No == 'B'||No == 'b'){
      Serial.println("M2");
      digitalWrite(M1, LOW);
      digitalWrite(M2, HIGH);
      digitalWrite(M3, LOW);
      digitalWrite(M4, LOW);
    }
    else if(No == 'C'||No == 'c'){
      Serial.println("M3");
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      digitalWrite(M3, HIGH);
      digitalWrite(M4, LOW);
    }
    else if(No == 'D'||No == 'd'){
      Serial.println("M4");
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      digitalWrite(M3, LOW);
      digitalWrite(M4, HIGH);
    }
    else if(No == 'E'||No == 'e'){
      Serial.println("MStop");
      digitalWrite(M1, LOW);
      digitalWrite(M2, LOW);
      digitalWrite(M3, LOW);
      digitalWrite(M4, LOW);
    }
}


