#include "Timer.h"
#include "Adafruit_LiquidCrystal.h"
// Depending on the LiquidCrystal library you are able to install, it might be:
// #include "LiquidCrystal.h"
#include "pitches.h"

// Sound Variables  
int buzzer = 8;
int sw = 10;

// == Song 1 ==
int song1[] = {
NOTE_E4, NOTE_C5, NOTE_B1, NOTE_F3, NOTE_C4, 
NOTE_A4, NOTE_A4, NOTE_GS5, NOTE_C5, NOTE_CS4, 
NOTE_AS4, NOTE_C5, NOTE_DS4, NOTE_CS5, NOTE_GS4, 
NOTE_C3, NOTE_E3, NOTE_DS5, NOTE_D4, NOTE_D3
};
int song1_time[] = {
2, 1, 2, 1, 1, 4, 8, 16, 8, 4, 4, 1, 8, 4, 2, 4, 4, 16, 4, 2
};

// == Song 2 ==

int song2[] = {
  NOTE_FS5, NOTE_D2, NOTE_DS5, NOTE_G2, NOTE_B3, 
  NOTE_C2, NOTE_G5, NOTE_D6, NOTE_CS5, NOTE_AS4, 
  NOTE_DS6, NOTE_D3, NOTE_CS4, NOTE_E5, NOTE_DS6,
   NOTE_E4, NOTE_B4, NOTE_F4, NOTE_E6, NOTE_DS4
};

int song2_time[] = {
  2, 2, 4, 8, 1, 8, 4, 4, 16, 8, 2, 4, 16, 8, 2, 4, 16, 4, 8, 1
};

// == Song 3 == 

int song3[] = {
  NOTE_A5, NOTE_D4, NOTE_D6, NOTE_DS3, NOTE_G4, 
  NOTE_B2, NOTE_F2, NOTE_A3, NOTE_AS2, NOTE_B5, 
  NOTE_C6, NOTE_C3, NOTE_GS3, NOTE_G2, NOTE_FS5, 
  NOTE_AS4, NOTE_GS2, NOTE_CS3, NOTE_C3, NOTE_AS2
};

int song3_time[] = {
  1, 2, 16, 4, 16, 2, 16, 1, 1, 2, 1, 8, 2, 16, 8, 1, 16, 4, 1, 2
};


// LCD variables
const int rs = 12, en = 11, d4 = 5, d5 = 4, d6 = 3, d7 = 2;
Adafruit_LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
// Depending on the LiquidCrystal library you are able to install, it might be:
// LiquidCrystal lcd(rs, en, d4, d5, d6, d7);



// Task Structure Definition
typedef struct task {
   int state;                  // Tasks current state
   unsigned long period;       // Task period
   unsigned long elapsedTime;  // Time elapsed since last task tick
   int (*TickFct)(int);        // Task tick function
} task;


const unsigned char tasksNum = 4;
task tasks[tasksNum]; // We have 4 tasks

// Task Periods

const unsigned long periodLCDOutput = 100;
const unsigned long periodJoystickInput = 100;
const unsigned long periodSoundOutput = 100;
const unsigned long periodController = 500;


// GCD 
const unsigned long tasksPeriodGCD = 100;

// Task Function Definitions
int TickFct_LCDOutput(int state);
int TickFct_JoystickInput(int state);
int TickFct_SoundOutput(int state);
int TickFct_Controller(int state);

// Task Enumeration Definitions
enum LO_States {LO_init, LO_MenuOptionA, LO_MenuOptionB, LO_MenuOptionC, LO_start, LO_start2, LO_song1, LO_song2, LO_song3, LO_pause, LO_resume};
enum JI_States {JI_init, JI_Sample};
enum SO_States {SO_init, SO_SoundOn, SO_SoundOn2, SO_SoundOn3, SO_SoundOff, SO_SoundOff2, SO_SoundOff3, SO_off, SO_pause, SO_pause2, SO_pause3};
enum C_States {C_init, C_T1};



void TimerISR() { // TimerISR actually defined here
  unsigned char i;
  for (i = 0; i < tasksNum; ++i) { // Heart of the scheduler code
     if ( tasks[i].elapsedTime >= tasks[i].period ) { // Ready
        tasks[i].state = tasks[i].TickFct(tasks[i].state);
        tasks[i].elapsedTime = 0;
     }
     tasks[i].elapsedTime += tasksPeriodGCD;
  }
}


void LCDWriteLines(String line1, String line2) {
  lcd.clear();          
  lcd.setCursor(0, 0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

// Task Function Definitions

int menuOption = 0;
int playSong = 0;
int soundPlay = 0;
int again = 0;
int pause = 0;
int resume = 1;

// Task 1
int TickFct_LCDOutput(int state) {
  switch (state) { // State Transitions
    case LO_init:
      state = LO_MenuOptionA;
      LCDWriteLines("Song 1  Song 2", "Song 3  Start");
    break;
    case LO_MenuOptionA:
      // if(menuOption == 1) {
      //   lcd.setCursor(6, 1);
      //   LCDWriteLines("Current Song", "Song 1");
      if (menuOption == 1){
        state = LO_song1;
        Serial.println("zauibnfuidbf");
        lcd.setCursor(7, 0);
        lcd.blink();
      }
      if (menuOption == 2){
        state = LO_song2;
        lcd.setCursor(6, 0);
        lcd.blink();
      }
      if (menuOption == 3){
        state = LO_song3;
        lcd.setCursor(6, 1);
        lcd.blink();
      }
      if (menuOption == 4){
        lcd.setCursor(7, 1);
        lcd.blink();
      }
      if (digitalRead(sw) == 0 && menuOption == 5){
          lcd.print("*");
      }
      // }
    break;
    case LO_MenuOptionB:
      if (playSong == 1){
        soundPlay = 1;
        Serial.println("OT WWEN T GHOYHIO");
        LCDWriteLines("Playing  Song 1", "Pause    Resume");
        state = LO_MenuOptionC;
      }
      if (playSong == 2){
        soundPlay = 2;
        LCDWriteLines("Playing  Song 2", "Pause    Resume");
        state = LO_MenuOptionC;
      }
      if (playSong == 3){
        soundPlay = 3;
        LCDWriteLines("Playing  Song 3", "Pause    Resume");
        state = LO_MenuOptionC;
      }
      // if(menuOption == 0) {
      //   state = LO_MenuOptionA;
      // LCDWriteLines("Lab 7", "Sound System");
      // }
      // if (again == 1){
      //   again = 0;
      //   state = LO_init;
      // } 
    break;
    case LO_MenuOptionC:
      if (menuOption == 3){
        state = LO_pause;
        // lcd.setCursor(8, 1);
        // lcd.print("");
        lcd.setCursor(5, 1);
        lcd.blink();
      }
      if (menuOption == 4){
        state = LO_resume;
        lcd.setCursor(8, 1);
        lcd.blink();
      }  
      if (again == 1){
        again = 0;
        state = LO_init;
      } 

    break;
    case LO_song1:
      if (digitalRead(sw) == 0){
        playSong = 2;
        lcd.print("*");
        state = LO_start;
      }
      else if (menuOption == 2 || menuOption == 3 || menuOption == 4){
        state = LO_MenuOptionA;
      }
    break;
    case LO_song2:
      if (digitalRead(sw) == 0){
        playSong = 1;
        lcd.print("*");
        state = LO_start;
      }
      else if (menuOption == 3 || menuOption == 4 || menuOption == 1){
        state = LO_MenuOptionA;
      }
    break;
    case LO_song3:
      if (digitalRead(sw) == 0){
        playSong = 3;
        lcd.print("*");
        state = LO_start;
      }
      else if (menuOption == 4 || menuOption == 1 || menuOption == 2){
        state = LO_MenuOptionA;
      }
    break;
    case LO_start:
      if (digitalRead(sw) == 1){
        state = LO_start2;
      }
    break;
    case LO_start2:
      lcd.setCursor(7, 1);
      lcd.blink();
      if (digitalRead(sw) == 0){
        lcd.clear();
        state = LO_MenuOptionB;
      }
    break;
    case LO_pause:
      if (digitalRead(sw) == 0){
        lcd.print("*");
        pause = 1;
      }
      else if (menuOption == 4){
        state = LO_MenuOptionC;
      }
    break;
    case LO_resume:
      if (digitalRead(sw) == 0){    
        lcd.print("*");
        lcd.setCursor(5, 1);
        lcd.print(" ");
        //lcd.clear(5, 1);  
        resume = 0;
      }
      else if (menuOption == 3){
        state = LO_MenuOptionC;
      }
      if (again == 1){
        again = 0;
        state = LO_init;
      } 
    break;
  }

  switch (state) { // State Actions
    case LO_MenuOptionA:
    break;

    case LO_MenuOptionB:
    break;

    case LO_MenuOptionC:
    break;

    case LO_song1:
    break;

    case LO_song2:
    break;

    case LO_song3:
    break;

    case LO_start:
    break;

    case LO_start2:
    break;

    case LO_resume:
    break;

    case LO_pause:
    break;
  }
  return state;
}
// Task 2
int TickFct_JoystickInput(int state) {
  switch (state) { // State Transitions
    case JI_init:
    state = JI_Sample;
    break;
    case JI_Sample:
    break;
  }

   switch (state) { // State Actions
    case JI_Sample:
      if (analogRead(A0) < 200 && analogRead(A1) < 200){
        menuOption = 1;
      }
      if (analogRead(A0) > 900 && analogRead(A1) < 200){ // TOPLEFT
        menuOption = 2;
      }
      if (analogRead(A0) > 900 && analogRead(A1) > 900){ // BOTTOMLEFT
        menuOption = 3;
      }
      if (analogRead(A0) < 200 && analogRead(A1) > 900){ // BOTTOMRIGHT
        menuOption = 4;
      }
      if ((analogRead(A0) > 450 && analogRead(A0) < 600) && (analogRead(A1) > 450 && analogRead(A1) < 600)){
        menuOption = 5;
      }  

      // if ((analogRead(A0) > 900) && (analogRead(A1) > 450 && analogRead(A0) < 600)){
      //   menuOption = 6;
      // }
      // if ((analogRead(A0) < 200) && (analogRead(A1) > 450 && analogRead(A0) < 600)){
      //   menuOption = 7;
      // }
      if (again == 1){
        again = 0;
        state = JI_init;
      }    

    break;
  }
  return state;
}
// Sound Output
int counter = 0;
int note = 0;
int TickFct_SoundOutput(int state) {
  switch (state) { // State Transitions
    case SO_init:
      //state = SO_SoundOn;
      if (soundPlay == 1){
        Serial.println("SHOULD GO HERE");
        state = SO_SoundOn;
      }
      if (soundPlay == 2){
        state = SO_SoundOn2;
      }
      if (soundPlay == 3){
        state = SO_SoundOn3;
      }
    break;
    case SO_SoundOn:
      if(counter >= song1_time[note]) {
         state = SO_SoundOff;
         note++;
         counter = 0;
         note = note % 20;
         Serial.println(song1[note]);
      }
      if (note == 19){
        Serial.println("TURN OFFFF");
        again = 1;
        state = SO_off;
      }
    if (pause == 1){
      state = SO_pause;
    }
    break;
    case SO_SoundOn2:
      if(counter >= song2_time[note]) {
         state = SO_SoundOff2;
         note++;
         counter = 0;
         note = note % 20;
      }
      if (note == 19){
        Serial.println("TURN OFFFF");
        again = 1;
        state = SO_off;
      }
    if (pause == 1){
      state = SO_pause2;
    }
    break;
    case SO_SoundOn3:
      if(counter >= song3_time[note]) {
         state = SO_SoundOff3;
         note++;
         counter = 0;
         note = note % 20;
      }
      if (note == 19){
        Serial.println("TURN OFFFF");
        again = 1;
        state = SO_off;
      }
    if (pause == 1){
      state = SO_pause3;
    }
    break;
    case SO_SoundOff:
      state = SO_SoundOn;
    break;
    case SO_SoundOff2:
      state = SO_SoundOn2;
    break;
    case SO_SoundOff3:
      state = SO_SoundOn3;
    break;
    case SO_off:

      // if (soundPlay == 5){
      //   int menuOption = 0;
      //   int playSong = 0;
      //   int soundPlay = 0;
      //   state = SO_init;
      // }
        note = 0;
        counter = 0;
        again = 0;
        menuOption = 0;
        playSong = 0;
        soundPlay = 0;
      state = SO_init;
      // state = SO_init;
    break;
    
  }
   switch (state) { // State Actions
    case SO_SoundOn:
      tone(buzzer, song1[note], periodSoundOutput * song1_time[note]);
      counter++;
    break;
    case SO_SoundOn2:
      tone(buzzer, song2[note], periodSoundOutput * song2_time[note]);
      counter++;
    break;
    case SO_SoundOn3:
      tone(buzzer, song3[note], periodSoundOutput * song3_time[note]);
      counter++;
    break;
    case SO_SoundOff:
      noTone(buzzer);
    break;
    case SO_SoundOff2:
      noTone(buzzer);
    break;
    case SO_SoundOff3:
      noTone(buzzer);
    break;
    case SO_off:
      //noTone(buzzer);
    break;
    case SO_pause:
    noTone(buzzer);
      if (resume == 0){
        pause = 0;
        resume = 1;
        state = SO_SoundOn;
      }
    break;
    case SO_pause2:
    noTone(buzzer);
      if (resume == 0){
        pause = 0;
        resume = 1;
        state = SO_SoundOn2;
      }
    break;
    case SO_pause3:
    noTone(buzzer);
      if (resume == 0){
        pause = 0;
        resume = 1;
        state = SO_SoundOn3;
      }
    break;

  }
  return state;
}

// Task 4 (Unused)
int TickFct_Controller(int state) {
  switch (state) { // State Transitions
    case 0:
    break;
  }

   switch (state) { // State Actions
    case 0:
    break;
  }
  return state;
}



void InitializeTasks() {
  unsigned char i=0;
  tasks[i].state = LO_init;
  tasks[i].period = periodLCDOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_LCDOutput;
  ++i;
  tasks[i].state = JI_init;
  tasks[i].period = periodJoystickInput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_JoystickInput;
  ++i;
  tasks[i].state = SO_init;
  tasks[i].period = periodSoundOutput;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_SoundOutput;
  ++i;
  tasks[i].state = C_init;
  tasks[i].period = periodController;
  tasks[i].elapsedTime = tasks[i].period;
  tasks[i].TickFct = &TickFct_Controller;

}

void setup() {
  // put your setup code here, to run once:
  InitializeTasks();

  TimerSet(tasksPeriodGCD);
  TimerOn();
  Serial.begin(9600);
  // Initialize Outputs
  lcd.begin(16, 2);
  // Initialize Inputs
  pinMode(sw, INPUT_PULLUP);
}

void loop() {
  // put your main code here, to run repeatedly:
  // Task Scheduler with Timer.h
  
}
