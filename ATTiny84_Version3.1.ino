// this should be the produciton version for 3.1

// Rotary Controller pins
#define CLK   2 // read the clock on the rotary controller
#define DATA  1 // read the data from the rotary controller 

// Joystick pins
#define WEST  5 // pin for west  intv pin 1 ColecoVision Pin 3
#define EAST  4 // pin for east  intv pin 3 ColecoVision Pin 4
#define NORTH 3 // pin for north intv pin 2 ColecoVision Pin 1
#define SOUTH 0 // pin for south intv pin 4 ColecoVision Pin 2
// The fire buttons aren't hooked to the arduino.
// They are either hooked to a optocoupler, Intellivison,  or to the 
// controller plug directly, ColecoVision.
// On the Intellivision fire are pins 6 & 8
// On the ColecoVision Pin 6 is fire.
// Intellivision uses pin 5 for common
// ColecoVision uses pin 8 for common

// Orientation and reverse switches.
#define REVERSE 6 // pin reverse the direction the paddle will go so East becomes West - etc
#define ORIENTATION 7 // pin for up/down or left/right

boolean stopped = true;
int unsigned wait = 0;
int delayDurationMS = 64;
int CCW = WEST;
int CW = EAST;
int defaultWait = 64; //may need to adjust - was 256 - 20200208

void setup() {
 // set up the rotary encoder
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DATA, INPUT_PULLUP);
// set up the orientation and reverse switches.  
  pinMode(REVERSE, INPUT_PULLUP);
  pinMode(ORIENTATION, INPUT_PULLUP);

  pinMode(WEST, OUTPUT);
  pinMode(EAST, OUTPUT);
  pinMode(NORTH, OUTPUT);
  pinMode(SOUTH, OUTPUT);

// Currently the paddle checks the orientation and 
// reverse direction switches at starup.  This may
// change, I migh tbe able to set them to interupts.

// read the orientation switch first
  if (digitalRead(ORIENTATION) == LOW) {
    CW = SOUTH;
    CCW = NORTH;
  }

// then check if we are reversing
  if (digitalRead(REVERSE) == LOW) {
    int hold = CCW;
    CCW = CW;
    CW = hold;
  }

//1,2,4,8 may need to adjust
  delayDurationMS = 8;

}

static uint8_t prevNextCode = 0;
static uint16_t store=0;

void loop() {
  
  static int8_t c,val;

  if( val=read_rotary() ) {
    c +=val;
    if ( prevNextCode==0x0b) {
      stopped = false;
      wait = 0;
      pressTheButton(CCW, HIGH, delayDurationMS);
    } 
    if ( prevNextCode==0x07) {
      stopped = false;
      wait = 0;
      pressTheButton(CW, HIGH, delayDurationMS);
    } 
  } else {
    if (!stopped) {
      wait++;
      if (wait > defaultWait) {
        stopped = true;
        pressTheButton(EAST, LOW, 1);
        pressTheButton(WEST, LOW, 1);
        pressTheButton(NORTH, LOW, 1);
        pressTheButton(SOUTH, LOW, 1);
        wait = 0;
      }
    }
  }
}

void pressTheButton (int8_t direction, int8_t state, int delayDuration) {
//   digitalWrite(direction, state);
   for  (unsigned int i = 0; i < delayDuration; i++) {
     digitalWrite(direction, state);
   }
}

// A vald CW or CCW move returns -1 or 1, invalid returns 0.
  int8_t read_rotary() {
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

  prevNextCode <<= 2;
  if (digitalRead(DATA)) prevNextCode |= 0x02;
  if (digitalRead(CLK)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

   // If valid then store as 16 bit data.
   if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      if ((store&0xff)==0x2b) return -1;
      if ((store&0xff)==0x17) return 1;
   }
   return 0;
}
