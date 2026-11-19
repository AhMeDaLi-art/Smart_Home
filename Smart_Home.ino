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



const String password = "";  // Password for the smart home system
int mode =0; //0 door , 1 indoor , -1 alarm state
int security =0; //0 door , 1 indoor_0ff , 2 indoor_on , -1 alarm state
String entered = "";
bool unlocked = false; // Flag to indicate if the door is unlocked
unsigned long unlockUntil = 0, ledDelay=0; // Timestamp until which the door remains unlocked and the last time the LED was turned on
unsigned long lastDisplay = 0; // Timestamp for the last display update
int astrict_count =0; // Counter for the number of asterisks displayed on the LCD
int ReedState = -1; // State of the reed switch (0: closed, 1: open)
int tempSet = 0; // Desired temperature threshold for the fan
int poten; // Variable to store the potentiometer reading
int lastPoten=0; // Variable to store the last potentiometer reading
bool pir_state=0; // State of the PIR sensor (0: no motion, 1: motion detected)





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
    int level = analogRead(LDR) < 512 ? 1 : 0; // Determine the LED level based on LDR reading
    float humidity = dht.readHumidity();
    float temp = dht.readTemperature();
    poten = analogRead(A3);




  if (key) {
/* If the system is in indoor mode and a key
   other than '*' or '#' is pressed, 
   reset the mode and security state, 
   clear the entered password, 
   and prompt for password entry again.
    */
    if (mode ==1 && key!='*' && key!='#'){ 
    mode = 0;
    security = 0;
    entered = "";
    lcd.clear();
    lcd.setCursor(0,0);
    lcd.print("Enter the pass: ");
    astrict_count=0;
    }


    /* If the system is in alarm state and a key
     other than '*' or '#' is pressed, 
     prompt for password entry.
    */
    else if(mode == -1 && key!='*' && key!='#'){
    lcd.setCursor(0,0);
    lcd.print("Enter the pass: ");
    }


    /* If the system is in indoor mode
       and the '*' key is pressed,
       toggle the security state. */
    if(key == '*' && mode ==1 && security == 1){
      lcd.clear();
      security = 2;
      lcd.print("Window: Safe   ");
    }


    /* If the system is in indoor mode
       and the '*' key is pressed,
       toggle the security state. */
    else if(key == '*' && mode == 1 && security == 2 ){
      lcd.clear();
      security = 1;
    } 

    /* If the system is in door mode and the '#' key is pressed,
       or if the entered password length reaches 16 characters,
       check if the entered password matches the stored password.
       If it matches, unlock the door and set the system to indoor mode.
       If it doesn't match, display an error message and prompt for password entry again. */
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



      /* If the entered password does not match the stored password,
         display an error message, clear the entered password,
         and prompt for password entry again. */
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


      /* If the system is in door mode and the '*' key is pressed,
         or if the system is in indoor mode and the '#' key is pressed,
         do nothing. */
    } 
    else if((key == '#' && mode !=0)|| (key =='*'&& mode !=1)){
        ;
    }



    /* If a key other than '*' or '#' is pressed,
       append the key to the entered password,
       display an asterisk on the LCD, and increment the asterisk count. */
    else{
      entered += key;
      lcd.setCursor(astrict_count, 1);
      lcd.print('*');
      astrict_count++;
    }
  }



  /* If the door is unlocked and the unlock duration has elapsed,
     lock the door and reset the unlocked flag. */
if (unlocked && (long)(millis() - unlockUntil) >= 0) {
  doorServo.write(0);
  unlocked = false;
}


/* If the system is in indoor mode,
   read the potentiometer value, map it to a temperature threshold,
   display the threshold on the LCD,
   and control the fan based on the current temperature and the set threshold.
   then display the current temperature and humidity on the LCD. */
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


  /* If the reed switch is open and the system is not in alarm state,
     display an alarm message, sound the buzzer, clear the entered password,
     reset the asterisk count, set the security state to alarm, and set the mode to alarm. */

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

  /* If the PIR sensor detects motion and the system is not in alarm state,
     turn on the output LED for 2.5 seconds, then turn it off. */
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

    // Read the state of the reed switch and update the ReedState variable accordingly
    if(Reed_read == 0){
    ReedState=1;

    }
    else{
    ReedState =0 ;
    }
  }



}