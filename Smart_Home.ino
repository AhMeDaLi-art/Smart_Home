#include <Keypad.h>
#include <Servo.h>
#include <DHT.h>
#include <LCD_I2C.h>



#define led 12
#define outLed A2
#define fan 10
#define buzz 11
#define servo 9
#define REED 7
#define PIR 8
#define LDR A0
#define button 1
#define dhtPin A1
#define pot A3
#define DHTTYPE DHT11  



LCD_I2C lcd(0x27, 16, 2);
DHT dht(dhtPin, DHTTYPE);
Servo doorServo;

const byte ROWS = 4;
const byte COLS = 3;
char keys[ROWS][COLS] = {
  {'1','2','3'},
  {'4','5','6'},
  {'7','8','9'},
  {'*','0','#'}
};
byte rowPins[ROWS] = {0,13,2,3};
byte colPins[COLS] = {4,5,6};
Keypad keypad = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);



const String password = "1234";
int mode =0; //0 door , 1 indoor , -1 alarm state
int security =0; //0 door , 1 indoor_0ff , 2 indoor_on , -1 alarm state
String entered = "";
bool unlocked = false;
unsigned long unlockUntil = 0, ledDelay=0;
unsigned long lastDisplay = 0, lastBlink =0 ;
int astrict_count =0,ReedState = -1;
int tempSet = 0;
int poten;
int lastPoten=0;
bool pir_state=0;





void setup() {
  pinMode(REED,INPUT);
  pinMode(PIR,INPUT);
  pinMode(led, OUTPUT);
  pinMode(outLed,OUTPUT);
  pinMode(buzz,OUTPUT);
  pinMode(button,INPUT_PULLUP);
  pinMode(rowPins,INPUT_PULLUP);
  pinMode(colPins,INPUT_PULLUP);  
  pinMode(fan,OUTPUT);
  doorServo.attach(servo);
  doorServo.write(0);
  dht.begin();
  lcd.begin();
  lcd.backlight();
  lcd.clear();
  lcd.setCursor(0,0);
  lcd.print("Enter the pass: ");

}


  

void loop() {
  const char key = keypad.getKey();
    int Reed_read = digitalRead(REED);
    int pir_read = digitalRead(PIR);
    int level = analogRead(LDR) < 512 ? 1 : 0;
    float humidity = dht.readHumidity();
    float temp = dht.readTemperature();
    poten = analogRead(A3);




  if (key) {
    if (mode ==1 && key!='*' && key!='#'){
    mode = 0;
    security = 0;
    entered = "";
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter the pass: ");
    astrict_count=0;
    }
    else if(mode == -1 && key!='*' && key!='#'){
    lcd.setCursor(0,0);
    lcd.print("Enter the pass: ");
    }
    
    if(key == '*' && mode ==1 && security == 1){
      lcd.clear();
      security = 2;
      lcd.print("Window: Safe   ");


    }
    else if(key == '*' && mode == 1 && security == 2 ){
      lcd.clear();
      security = 1;

    } 
    else if ((key == '#' || entered.length() == 16) && mode != 1 ) {
      if (entered == password) {
        unlocked = true;
        unlockUntil = millis() + 5000UL;
        doorServo.write(90);
        noTone(buzz);
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("PIN accepted");
        delay(500);
        ReedState = 0;
        entered="";
        mode =1;
        security =1;
        astrict_count=0;
      } 
      else {
        lcd.clear();
        lcd.setCursor(0, 0);
        lcd.print("Wrong PIN");
        lcd.setCursor(0,1);
        lcd.print(entered);
        delay(700);
        lcd.clear();
        entered = "";
        lcd.setCursor(0,0);
        lcd.print("Enter the pass: ");
        astrict_count=0;
      }
    } 
    else if((key == '#' && mode !=0)|| (key =='*'&& mode !=1)){
        ;
    }
    else{
      entered += key;
      lcd.setCursor(astrict_count, 1);
      lcd.print('*');
      astrict_count++;
    }
  }


if (unlocked && (long)(millis() - unlockUntil) >= 0) {
  doorServo.write(0);
  unlocked = false;
}
  if(mode ==1 ){
    
    tempSet= map(poten,0,1023,10,50);
    if((tempSet < lastPoten || tempSet  > lastPoten) && (tempSet>=15 && tempSet<=45) ){
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Threshold: "); lcd.print(tempSet, 1); lcd.print(" C   ");
      
      lastPoten=tempSet;
    delay(200);
    }
    
    if(temp>=tempSet || tempSet<=15){
       digitalWrite(fan,1);
       delay(500);

    }
    else if (tempSet>45) {
      digitalWrite(fan,0);
      delay(500);
    }
    else{
      digitalWrite(fan,0);
      delay(500);
    }
      if(security==1){ 
      if (millis() - lastDisplay >= 500UL) {
        
      lastDisplay = millis();
        
        if(digitalRead(button) == 1){
            digitalWrite(led, level);
        }
        else{
            digitalWrite(led,1);
            
        }

    lcd.setCursor(0, 0);
    lcd.print("Temp: "); lcd.print(temp, 1); lcd.print(" C   ");
    lcd.setCursor(0, 1);
    lcd.print("Humidity: "); lcd.print(humidity, 0); lcd.print("%  ");
      } 
    }


  } 

  if (ReedState ==1 && security!=-1) {
        lcd.clear();
        lcd.setCursor(0,0);
        lcd.print("Window:   Alarm ");
        tone(buzz,1023);
        entered ="";
        astrict_count =0;
        security =-1;
        mode =-1;
  }
  if(security==0 || security ==2){
    if(pir_read == 1 && pir_state == 0){
    digitalWrite(outLed,1);
    ledDelay = millis() + 2500UL;
    pir_state = 1;
    }
    if(pir_state ==1 && millis() - ledDelay >=0){
      digitalWrite(outLed,0);
      pir_state=0;
    }
    if(Reed_read == 0){
    ReedState=1;

    }
    else{
    ReedState =0 ;
    }
  }



}