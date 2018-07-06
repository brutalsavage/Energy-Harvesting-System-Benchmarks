#include <stdlib.h>
#include <limits.h>

#include <msp430.h>
#include <nvs.h>
#include <ctpl.h>
#include <ctpl_low_level.h>



extern int ctpl_magic_number;
extern int whichBox;
#define STATE_BUTTON_OPEN 0
#define STATE_BUTTON_CLOSED 1
#define STATE_DONE 2
#define STATE_UPDATING 3
#define STATE_ERROR 4

#define NUM_LEDS 3
#define MAX_LED 2 /*0,1, & 2*/
#define LED_STATE_COUNT 10

#undef USE_LEDS
#undef FLASH_ON_BOOT

#define NOTEOFF 0x80
#define NOTEON 0x90
#define MAXCHAN 0x0F //0x83 - chan 3, off; 0x9c - chan 13 on; etc
#define MAXNOTE 0x7F //Keys 0 -- 127
#define MAXVEL 0x7F //Strike velocity 0 -- 127
#define MAXPACKETS 10000
#define BUFLEN  3

#pragma PERSISTENT(someAdcValue);
float someAdcValue[100] = {
#include "someAdcValue.h"
};

nvs_data_handle nvsHandle1;


typedef struct _MIDImsg{
  unsigned int seqNum;
  unsigned char status; /*NOTEOFF or NOTEON*/
  unsigned char data0;   /*Key number*/
  unsigned char data1;   /*Velocity (0 - 127)*/
} MIDImsg;


typedef struct data{

  MIDImsg msg0;
  MIDImsg msg1;
  MIDImsg msg2;
  MIDImsg msg3;
  unsigned int msgSequenceNumber;
  unsigned int which;
  unsigned int speed;

} data_t;

#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(nvsStorage1)
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent
#endif

uint8_t nvsStorage1[NVS_DATA_STORAGE_SIZE(sizeof(data_t))] = {0};



void gpioPinSetup (void);

/*currTemp is a pointer to the current entry in the data log*/
unsigned int currentState;
unsigned int currentLED;
unsigned int inErrorState;
unsigned int initialized;
//unsigned int reboots;


uint16_t status;
data_t data;

/*The "transmission buffer"*/
unsigned char TXvel0;
unsigned char TXvel1;
unsigned char TXvel2;
unsigned char TXvel3;

void abortWithError(){

  inErrorState = 0xBEEE;
  while(1){
      P1OUT ^= BIT0;
      int i;
      for(i = 0; i < 0xfff; i++);
  }

}




void initializeNVData() {

  if(initialized != 0xBEEE){

    currentState = 0;
    currentLED = 0;
    //reboots = 0;

    data.msgSequenceNumber = 0;
    data.which = -1;
    data.msg0.seqNum = 0xFFFF;
    data.msg0.status = 0xFF;
    data.msg0.data0 = 0xFF; /*for now... -- eventually this should be the speed/dir*/
    data.msg0.data1 = 0xFF;

    data.msg1.seqNum = 0xFFFF;
    data.msg1.status = 0xFF;
    data.msg1.data0 = 0xFF; /*for now... -- eventually this should be the speed/dir*/
    data.msg1.data1 = 0xFF;

    data.msg2.seqNum = 0xFFFF;
    data.msg2.status = 0xFF;
    data.msg2.data0 = 0xFF; /*for now... -- eventually this should be the speed/dir*/
    data.msg2.data1 = 0xFF;

    data.msg3.seqNum = 0xFFFF;
    data.msg3.status = 0xFF;
    data.msg3.data0 = 0xFF; /*for now... -- eventually this should be the speed/dir*/
    data.msg3.data1 = 0xFF;
  }

  initialized = 0xBEEE;
}

void addMIDIMessage(unsigned int speed, unsigned int direction){

    data.which = data.which +1;
    data.msgSequenceNumber = data.msgSequenceNumber+1;
    switch(data.which){

      case 0:
      data.msg0.seqNum = data.msgSequenceNumber;
      data.msg0.status = NOTEON;
      data.msg0.data0 = 0x80; /*for now... -- eventually this should be the speed/dir*/
      data.msg0.data1 = speed % MAXVEL;
      break;

      case 1:
      data.msg1.seqNum = data.msgSequenceNumber;
      data.msg1.status = NOTEON;
      data.msg1.data0 = 0x80; /*for now... -- eventually this should be the speed/dir*/
      data.msg1.data1 = speed % MAXVEL;
      break;

      case 2:
      data.msg2.seqNum = data.msgSequenceNumber;
      data.msg2.status = NOTEON;
      data.msg2.data0 = 0x80; /*for now... -- eventually this should be the speed/dir*/
      data.msg2.data1 = speed % MAXVEL;
      break;

      case 3:
      data.msg3.seqNum = data.msgSequenceNumber;
      data.msg3.status = NOTEON;
      data.msg3.data0 = 0x80; /*for now... -- eventually this should be the speed/dir*/
      data.msg3.data1 = speed % MAXVEL;
      break;

      default:
      abortWithError();
      break;

    };

}


void sendAllMessages(){



  TXvel0 = data.msg0.data1;
  data.msg0.seqNum = 0xFFFF;
  data.msg0.status = 0xFF;
  data.msg0.data0 = 0xFF; /*for now... -- eventually this should be the speed/dir*/
  data.msg0.data1 = 0xFF;

  TXvel1 = data.msg1.data1;
  data.msg1.seqNum = 0xFFFF;
  data.msg1.status = 0xFF;
  data.msg1.data0 = 0xFF; /*for now... -- eventually this should be the speed/dir*/
  data.msg1.data1 = 0xFF;

  TXvel2 = data.msg2.data1;
  data.msg2.seqNum = 0xFFFF;
  data.msg2.status = 0xFF;
  data.msg2.data0 = 0xFF; /*for now... -- eventually this should be the speed/dir*/
  data.msg2.data1 = 0xFF;

  TXvel3 = data.msg3.data1;
  data.msg3.seqNum = 0xFFFF;
  data.msg3.status = 0xFF;
  data.msg3.data0 = 0xFF; /*for now... -- eventually this should be the speed/dir*/
  data.msg3.data1 = 0xFF;

}


void getInput(unsigned int *s, unsigned int *d){

  *s = (unsigned int)someAdcValue[data.msgSequenceNumber%100];

}

int main(void){

  WDTCTL = WDTPW | WDTHOLD;  // Stop watchdog timer
  PM5CTL0 &= ~LOCKLPM5;
  gpioPinSetup();
  P1DIR |= BIT0;
  P1OUT &=~ BIT0;
  P1DIR |= BIT1;
  P1OUT &=~ BIT1;
  //reboots++;
  nvsHandle1 = nvs_data_init(nvsStorage1, sizeof(data_t));
  status = nvs_data_restore(nvsHandle1, &data,ctpl_magic_number,whichBox);
  ctpl_init();

  if( inErrorState == 0xBEEE ){

    abortWithError();

  }


  initializeNVData();


  while( data.msgSequenceNumber < MAXPACKETS ){

    data.speed = 0;
    unsigned int direction = 0;

    status = nvs_data_commit(nvsHandle1, &data,whichBox);
    ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
    P4OUT |= BIT3;
            P4OUT &=~ BIT3;
    getInput(&data.speed,&direction); /*Reads speed and direction*/

    status = nvs_data_commit(nvsHandle1, &data,whichBox);
    ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
    P4OUT |= BIT3;
    P4OUT &=~ BIT3;
    addMIDIMessage(data.speed,direction);

    if( data.which == BUFLEN ){
      status = nvs_data_commit(nvsHandle1, &data,whichBox);
      ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
      P4OUT |= BIT3;
      P4OUT &=~ BIT3;
      sendAllMessages();
      data.which = -1;
    }


    data.msgSequenceNumber = data.msgSequenceNumber +1;
  }




  P2OUT |= BIT6;
   P2OUT &=~ BIT6;

  P1OUT |= BIT0;
  while(1){

  };

}
void gpioPinSetup () {
    P4OUT &=~ BIT3;
    P4SEL0 &=~ (BIT3);
    P4SEL1 &=~ (BIT3);
    P4REN &=~ BIT3;
    P4DIR |= BIT3;

    P2OUT |= BIT5;
    P2SEL0 &=~ (BIT5);
    P2REN |= BIT5;
    P2DIR |= BIT5;
    P2OUT &=~ BIT5;

    P2OUT &=~ BIT6;
    P2SEL0 &=~ (BIT6);
    P2REN |= BIT6;
    P2DIR |= BIT6;


}
