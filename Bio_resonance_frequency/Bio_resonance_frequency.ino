/*
 * Arduino bioresonance generator
 */
#include <U8g2lib.h>

U8G2_ST7920_128X64_1_SW_SPI u8g2(U8G2_R0, /* clock=*/ 13, /* data=*/ 11, /* CS=*/ 10, /* reset=*/ 8);

const int index_choroby = 37;          //the number of diseases in the disease array[]
const char* choroby[index_choroby] = {
  "Alcoholism",       "Angina",          "Stomachache",         "Pain in general",  "Headaches",
  "Infection",     "Acute pain",       "Back pain 2",        "Arthralgia",   "Toothache",
  "No appetite",     "No taste",      "Motion sickness", "Hoarseness",      "Dolegl. gastric",
  "Prostate ailments", "Deafness",        "Flu",               "Hemorrhoids",    "Kidney stones", 
  "Cough",           "runny nose",           "Hair loss",            "Hypertension", "Low pressure", 
  "Disease. thyroid gland","Bad breath","General herpes",   "Epilepsy",     "Constipation",
  "Dizziness"
  };
const int liczby[index_choroby * 10] = { 
  10000 ,0     ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      , //"Alkoholizm"
  787   ,776   ,727    ,690    ,465    ,428    ,660    ,0      ,0      ,0      , //"Angina"
  10000 ,3000  ,95     ,0      ,0      ,0      ,0      ,0      ,0      ,0      , //"Stomachache"
  3000  ,2720  ,95     ,666    ,80     ,40     ,0      ,0      ,0      ,0      , //"Pain in general"
  10000 ,144   ,160    ,520    ,304    ,0      ,0      ,0      ,0      ,0      , //"Headaches"
  3000  ,95    ,880    ,1550   ,802    ,787    ,776    ,727    ,0      ,0      , //"Infection"
  3000  ,95    ,10000  ,1550   ,802    ,880    ,787    ,727    ,690    ,666    , //"Acute pain"
  787   ,784   ,776    ,728    ,727    ,465    ,432    ,0      ,0      ,0      , //"Back pain 2"
  160   ,500   ,1600   ,5000   ,324    ,528    ,0      ,0      ,0      ,0      , //"Arthralgia"
  5170  ,3000  ,2720   ,2489   ,1800   ,1600   ,1550   ,880    ,832    ,666    , //"Toothache" 
  10000 ,465   ,444    ,1865   ,125    ,95     ,72     ,880    ,787    ,727    , //"No appetite"
  10000 ,20    ,0      ,0      ,0      ,0      ,0      ,0      ,0      ,0      , //"No taste"
  10000 ,5000  ,648    ,624    ,600    ,465    ,440    ,648    ,444    ,1865   , //"Motion sickness"
  880   ,760   ,727    ,0      ,0      ,0      ,0      ,0      ,0      ,0      , //"Hoarseness"
  10000 ,1550  ,802    ,880    ,832    ,787    ,727    ,465    ,0      ,0      , //"Dolegl. gastric",
  2050  ,880   ,1550   ,802    ,787    ,727    ,465    ,20     ,0      ,0      , //"Bladder and prostate ailments",
  10000 ,1550  ,880    ,802    ,787    ,727    ,20     ,0      ,0      ,0      , //"Deafness",
  954   ,889   ,841    ,787    ,763    ,753    ,742    ,523    ,513    ,482    , //"Flu",
  4474  ,6117  ,774    ,1550   ,447    ,880    ,802    ,727    ,0      ,0      , //"Hemorrhoids",
  10000 ,444   ,727    ,787    ,880    ,6000   ,3000   ,1552   ,0      ,0      , //"Kidney stones",
  7760  ,7344  ,3702   ,3672   ,1550   ,1500   ,1234   ,776    ,766    ,728    , //"Cough",
  1800  ,1713  ,1550   ,802    ,800    ,880    ,787    ,727    ,444    ,20     , //"runny nose",
  10000 ,5000  ,2720   ,2170   ,1552   ,880    ,800    ,787    ,727    ,465    , //"Hair loss",
  10000 ,3176  ,2112   ,95     ,324    ,528    ,880    ,787    ,727    ,304    , //"Hypertension",
  727   ,787   ,880    ,0      ,0      ,0      ,0      ,0      ,0      ,0      , //"Low pressure",
  16000 ,10000 ,160    ,80     ,35     ,0      ,0      ,0      ,0      ,0      , //"Disease. thyroid gland"
  1550  ,802   ,880    ,787    ,727    ,0      ,0      ,0      ,0      ,0      , //"Bad breath",
  2950  ,1900  ,1577   ,1550   ,1489   ,1488   ,629    ,464    ,450    ,383    , //"General herpes",
  10000 ,880   ,802    ,787    ,727    ,700    ,650    ,600    ,210    ,125    , //"Epilepsy"'
  3176  ,1550  ,880    ,832    ,802    ,787    ,776    ,727    ,444    ,422    , //"Constipation",
  1550  ,880   ,802    ,784    ,787    ,786    ,766    ,522    ,727    ,72       //"Dizziness",
};

byte inPin1 = 7;                  // digital input sw-1
byte inPin2 = 6;                  // digital input sw-2
byte inPin3 = 5;                  // digital input sw-3
byte buzzerPin = 4;
//byte ledPin = 3;
byte i, a, b, c, intr;
byte reading, reading1, reading2, reading3;  // button reading variables
byte P1, P2;                      // button states
unsigned long time0, time1, time2, time3;
char* tytul;                 //name of the selected disease
byte ilosc_f;  
byte strona;                        //menu page
char f_str[3];                      //string with an amount of f
char i_str[3];                      //string with number f
char* k_str;                        //the word END
int f_gen;                          //generator frequency


void setup(void) {
  u8g2.begin();
  Serial.begin(9600);
  pinMode(13, OUTPUT);         
  digitalWrite (13, LOW);                 //turning on the LCD backlight
  pinMode(inPin1, INPUT_PULLUP);          //setting the pin as a digital input
  pinMode(inPin2, INPUT_PULLUP);
  pinMode(inPin3, INPUT_PULLUP);
  P1 = 0; P2 = 1;
  intr = 0; strona = 0;
}

void loop(void) {
  if (intr == 1) {stuffHappened();}
  u8g2.firstPage();
  do {
    if ( intr == 0) {Intro();}              //display a splash screen
    if ( intr == 1) {
      Menu();  
    }
  } while ( u8g2.nextPage() );
  if (intr == 0) {delay(1000); intr = 1;}
  delay(20);                                  // screen refresh period
  //randomSeed(analogRead(0));
}

void Intro(void){ 
  u8g2.setFont(u8g2_font_helvB12_te);         // choosing large fonts
  u8g2.drawStr( 8, 20,  "Bio resonance ");
  u8g2.drawStr( 11, 45,  "RIFE machine");
  u8g2.setFont(u8g2_font_6x12_te);            // choosing large fonts
  u8g2.drawStr( 20, 62,  "Click  to start");
  //u8g2.drawGlyph(107,62, 261);                
  //u8g2.drawGlyph(113,62, 263);                
  u8g2.setFont(u8g2_font_open_iconic_arrow_1x_t);
  u8g2.drawGlyph(52,62, 64);                  // adding a down arrow
  //u8g2.drawGlyph(55,62, 67);                // adding an up arrow
}

void Menu(void){
  u8g2.setFont(u8g2_font_6x12_te);
  tytul = "  ILLNESS";
  Ramka();
  for (i = strona; i < 6 + strona; i++){
    if (i <= index_choroby - 1){
      u8g2.drawStr( 10, 20 + 10* (i-strona), choroby[i]);
    }        
  }
}

void Ramka(void) {
  u8g2.drawFrame( 0, 0 , 128, 64);         //drawing a frame around the screen
  u8g2.drawBox(0, 0, 128, 10);             //drawing a black background
  u8g2.setDrawColor(2);                    //select a writing mode
  u8g2.setFontMode(1);                     //white on black
  u8g2.drawStr(30, 8, tytul);
  u8g2.setDrawColor(1);                    //return to writing mode 
  u8g2.setFontMode(1);                     //black on white
}

void stuffHappened(void){
  P1 = 0; P2 = 0;
  do {
    Dipsw();
  }while (P2 == 0);  
}

void Dipsw(void){
  time0 = micros();
  reading = HIGH;
  //wybór choroby w dół
  reading = digitalRead(inPin1);              //reading the state sw = 1
  reading1 = digitalRead(inPin1);
  if (reading == LOW and reading1 == LOW) { 
    time1 = micros() - time0;
  }  
  if (time1 > 10 and reading1 == HIGH) {
    P1++;
    if (P1 > index_choroby) {P1 = index_choroby;}     //number of games in the disease table []
    if (P1 <= 5) {strona = 0;}
    if (P1 > 5 && P1 <=10) {strona = 5;}
    if (P1 > 10 && P1 <=15) {strona = 10;}
    if (P1 > 15 && P1 <=20) {strona = 15;}
    if (P1 > 20 && P1 <=25) {strona = 20;}
    if (P1 > 25 && P1 <=30) {strona = 25;}
    if (P1 > 30 && P1 <=35) {strona = 30;}
    if (P1 > 35 && P1 <=40) {strona = 35;}
    if (P1 > 40) {strona = 35;}
    Wyswietl_kursor();                               //displays a marquee
    time0 = 0; time1 = 0;
  }
  //wybranie choroby w górę
  reading = digitalRead(inPin2);                     //reading the state sw = 2
  reading2 = digitalRead(inPin2);
  if (reading == LOW and reading2 == LOW ) {                       
    time2 = micros() - time0;
  }  
  if (time2 > 10 and reading2 == HIGH) {
    P1--;
    if (P1 < 1) {P1 = 1;}
    if (P1 <=5) {strona = 0;}
    if (P1 > 5 && P1 <=10) {strona = 5;}
    if (P1 > 10 && P1 <=15) {strona = 10;}
    if (P1 > 15 && P1 <=20) {strona = 15;}
    if (P1 > 20 && P1 <=25) {strona = 20;}
    if (P1 > 25 && P1 <=30) {strona = 25;}
    if (P1 > 30 && P1 <=35) {strona = 30;}
    if (P1 > 35 && P1 <=40) {strona = 35;}
    if (P1 > 40) {strona = 35;}
    Wyswietl_kursor();
    time0 = 0; time2 = 0;
  }
  //włączenie trybu "PRACA"
  reading = digitalRead(inPin3);            //reading the state sw = 3
  reading3 = digitalRead(inPin3);
  if (reading == LOW and reading3 == LOW) {   
    time3 = micros() - time0;
  } 
  if (time3 > 20 and reading3 == HIGH) {
    tytul = choroby[P1-1];
    Generuj();
    time0 = 0; time3 = 0;
  }
}

void Wyswietl_kursor(void){                        //displays text and cursor
  u8g2.firstPage();
  do {
     u8g2.drawHLine( 5, 11 + (P1-strona)*10, 118);
     u8g2.drawHLine( 5, 2 + (P1-strona)*10, 118);
     u8g2.drawVLine( 5, 2 + (P1-strona)*10, 10);
     u8g2.drawVLine( 122, 2 + (P1-strona)*10, 10);     
     Menu();
  } while ( u8g2.nextPage() );
}

void Generuj(void){
  int f, b9;
  f = 0; ilosc_f = 0; k_str = "";
  for (i=0; i < 10; i++) {                    //determining the amount of f in the disease table
     f = liczby[10*(P1-1) + i];
     if (f > 0) ilosc_f ++;
  }
  if (ilosc_f > 4) {strcpy(f_str, u8x8_u8toa(ilosc_f, 2)); b = 1;}
  if (ilosc_f == 4){strcpy(f_str, u8x8_u8toa(8, 2)); b = 2;}
  if (ilosc_f == 3){strcpy(f_str, u8x8_u8toa(6, 2)); b = 2;}
  if (ilosc_f == 2){strcpy(f_str, u8x8_u8toa(6, 2)); b = 3;}
  if (ilosc_f == 1){strcpy(f_str, u8x8_u8toa(5, 2)); b = 5;}  
  for (i=0; i < ilosc_f; i++) {
     f_gen = liczby[10*(P1-1) + i];
     strcpy(i_str, u8x8_u8toa(i+1, 2));        //Convert to a 2-digit string
     DisplayRamka();
     for (a=0; a < b*6; a++){                    //generating 1 minute (6 * 10sec)
        beep(f_gen, 10000);
     }   
     delay (1000);
  }
  k_str = "FINISH";
  DisplayRamka();
  k_str = "";
}

void beep(int note, int duration){
  tone(buzzerPin, note, duration);
  delay(duration);
  noTone(buzzerPin);
  delay(30);
}

void   DisplayRamka() {
  u8g2.firstPage();
  do {
    u8g2.setFont(u8g2_font_6x12_te);           // choosing small fonts
    Ramka();
    u8g2.drawStr(22, 25, "Start of therapy");
    u8g2.drawStr(25, 38, "Time:");
    u8g2.drawStr(58, 38, f_str);
    u8g2.drawStr(75, 38, "min.");
    u8g2.setFont(u8g2_font_helvB14_te);         // choosing large fonts
    if (k_str == "") u8g2.drawStr(55, 58, i_str);
    if (k_str != "") u8g2.drawStr(22, 58, k_str);
  } while ( u8g2.nextPage() );
}
  
