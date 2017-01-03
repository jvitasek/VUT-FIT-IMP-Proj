// Jakub Vitasek, xvitas02
// ORIGINAL
// 15.12.2016

#include <hidef.h>
#include "derivative.h"

/** Constants */
#define LOGIN_SIZE 64
#define DISPLAY_HORIZONTAL_SIZE 32
#define DISPLAY_VERTICAL_SIZE 8
#define DEFAULT_SPEED 100
#define MAX_SPEED 1200
#define SPEED_QUANTIFIER 4

/** States */
#define STATE_INIT 0
#define STATE_HORIZONTAL_ROTATION 1
#define STATE_VERTICAL_ROTATION 2

/** Macros */
#define CALCULATE_SPEED(x) (MAX_SPEED - (SPEED_QUANTIFIER*x)) 

/* ********************* */
/* FUNCTION DECLARATIONS */
/* ********************* */

unsigned char set_state();
void horizontal_rotation(int horizontal);
void vertical_rotation(int horizontal, int vertical);

/* **************** */
/* PORT DEFINITIONS */
/* **************** */

/** Buttons */
unsigned char *port_btn_init = (char *) 0xF1;
unsigned char *port_btn_horizontal = (char *) 0xF2;
unsigned char *port_btn_vertical = (char *) 0xF3;

/** Potentiometer */
unsigned char *port_speed = (char *) 0xF0;

/** Diodes */
unsigned char *port_led = (char *) 0xD0;

/* **************** */
/* GLOBAL VARIABLES */
/* **************** */

/** Login string */
const unsigned char login[LOGIN_SIZE] = {
  0, 0, 0, 36, 34, 126, 32, 32,
  0, 0, 0, 0, 94, 0, 0, 0,
  0, 60, 4, 2, 2, 4, 60, 0,
  0, 0, 34, 20, 8, 20, 34, 0,
  0, 18, 58, 42, 42, 38, 22, 0,
  0, 0, 62, 38, 42, 50, 62, 0,
  0, 0, 38, 42, 42, 42, 58, 0,
  0, 0, 62, 42, 42, 42, 46, 0,
};

/* state indicator */
unsigned char state = STATE_INIT;

/* ************* */
/* MAIN FUNCTION */
/* ************* */
void main(void) {
  int horizontal;
  int vertical;

  *port_btn_init = 0;  
  *port_btn_horizontal = 0;
  *port_btn_vertical = 0;
  
  *port_speed = DEFAULT_SPEED;
  
  EnableInterrupts;
  
  for(;;) {
    horizontal_rotation(DISPLAY_HORIZONTAL_SIZE-1);
    horizontal = DISPLAY_HORIZONTAL_SIZE-1;
    vertical = 0;
  
    for(;;) {
      __RESET_WATCHDOG();
      state = set_state();
      
      /* ******************* */
      /* DECIDING WHAT TO DO */
      /* ******************* */
      if(state == STATE_INIT) break;
      else if(state == STATE_HORIZONTAL_ROTATION) {
        ++horizontal;
        horizontal_rotation(horizontal);
      } else if(state == STATE_VERTICAL_ROTATION) {
        ++vertical;
        vertical_rotation(horizontal, vertical);
      }
    }
  }
}

unsigned char set_state()  {
   if(*port_btn_init == 1) return STATE_INIT;
   else if(*port_btn_horizontal == 1) return STATE_HORIZONTAL_ROTATION;
   else if(*port_btn_vertical == 1) return STATE_VERTICAL_ROTATION;
   return state;  
}

void horizontal_rotation(int horizontal) {
  int x = 0;
  unsigned int limit = CALCULATE_SPEED(*port_speed);
  unsigned int i,j = 0;

  /* ******************* */
  /* HORIZONTAL ROTATION */
  /* ******************* */
    
  for(x = DISPLAY_HORIZONTAL_SIZE-1; x >= 0; --x) {
    port_led[x] = login[(horizontal-x)%LOGIN_SIZE];
  }
  
  /* ********************************* */
  /* SETTING THE SPEED OF THE ROTATION */
  /* ********************************* */
  
  /** Busy wait to delay the rotation */
  for(i = 0; i < limit; i++) {
    for(j = 0; j < 2; j++) {;}
  }
}

void vertical_rotation(int horizontal, int vertical) {
  int x, y = 0;
  unsigned char shifted, unshifted = 0;
  unsigned int limit = CALCULATE_SPEED(*port_speed);
  unsigned int i,j = 0;

  /* ***************** */
  /* VERTICAL ROTATION */
  /* ***************** */
  
  for(x = DISPLAY_HORIZONTAL_SIZE-1; x >= 0; --x) {
    unshifted = login[(horizontal-x)%LOGIN_SIZE];
    for(y = vertical%DISPLAY_VERTICAL_SIZE; y >= 0; --y) {
      shifted = (unshifted << 1);
      unshifted = shifted;
    }
    port_led[x] = shifted;
  }
  
  /* ********************************* */
  /* SETTING THE SPEED OF THE ROTATION */
  /* ********************************* */
  
  /** Busy wait to delay the rotation */ 
  for(i = 0; i < limit; i++) {
    for(j = 0; j < 2; j++) {;}
  }
}