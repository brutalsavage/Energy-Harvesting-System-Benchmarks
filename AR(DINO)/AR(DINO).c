#include <msp430.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h>
#include <nvs.h>
#include <ctpl.h>
#include <nvs2.h>
#include <ctpl_low_level.h>

extern int ctpl_magic_number;
extern int whichBox;


int count;
#define LOW_POWER // don't use LEDs
#define MODEL_COMPARISONS 10
#define MODEL_SIZE 100
#define NUM_FEATURES 2
#define ACCEL_WINDOW_SIZE 4

#pragma PERSISTENT(stationary);
float stationary[MODEL_SIZE] = {
#include "int_stationary_model.h"
};

#pragma PERSISTENT(walking);
float walking[MODEL_SIZE] = {
#include "int_walking_model.h"
};

#pragma PERSISTENT(dataSet);
float dataSet[104] = {
#include "dataSet.h"
};

/*
#pragma PERSISTENT(nvs_magic_number);
int nvs_magic_number = 0;
*/

nvs_data_handle nvsHandle1;
nvs_data_handle nvsHandle4;
nvs_data_handle nvsHandle5;
typedef long int accelReading[3];
typedef accelReading accelWindow[ACCEL_WINDOW_SIZE];

typedef struct data {
   int __NV_walkingCount;
   int __NV_stationaryCount;
   int __NV_totalCount;
   int class;
   int currSamp;
   accelWindow aWin;
   long int meanmag;
   long int stddevmag;
} data_t;

#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(nvsStorage1)
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent
#endif

#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(nvsStorage4, ".infoA")
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma location="INFOA"
__no_init
#endif

#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(nvsStorage5, ".infoA")
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma location="INFOA"
__no_init
#endif

#pragma PERSISTENT(check);
int check = 1;



uint8_t nvsStorage4[NVS_DATA_STORAGE_SIZE(sizeof(uint64_t))];
uint8_t nvsStorage1[NVS_DATA_STORAGE_SIZE(sizeof(data_t))] = {0};
uint8_t nvsStorage5[NVS_DATA_STORAGE_SIZE(sizeof(uint64_t))];

uint16_t status;
data_t data;

int restart;

//static accelWindow aWin;

accelReading mean;
accelReading stddev;


void initClocks(void);
void gpioPinSetup (void);
void getOneSample(accelReading tr){
  int counter = data.__NV_totalCount;
  long x = (long)dataSet[counter];
  long y = (long)dataSet[counter+1];
  long z = (long)dataSet[counter+2];

  tr[0] = x;
  tr[1] = y;
  tr[2] = z;

}

void getNextSample(){

  getOneSample( data.aWin[data.currSamp] );
  data.currSamp = (data.currSamp + 1);
  if( data.currSamp >= ACCEL_WINDOW_SIZE ){
    data.currSamp = 0;
  }

}


void featurize(){

  mean[0] = mean[1] = mean[2] = 0;
  stddev[0] = stddev[1] = stddev[2] = 0;
  int i;
  for(i = 0; i < ACCEL_WINDOW_SIZE; i++){
    mean[0] = mean[0] + data.aWin[i][0];//x
    mean[1] = mean[1] + data.aWin[i][1];//y
    mean[2] = mean[2] + data.aWin[i][2];//z
  }
  mean[0] = mean[0] / ACCEL_WINDOW_SIZE;
  mean[1] = mean[1] / ACCEL_WINDOW_SIZE;
  mean[2] = mean[2] / ACCEL_WINDOW_SIZE;

  for(i = 0; i < ACCEL_WINDOW_SIZE; i++){
    stddev[0] += data.aWin[i][0] > mean[0] ? data.aWin[i][0] - mean[0] : mean[0] - data.aWin[i][0];//x
    stddev[1] += data.aWin[i][1] > mean[1] ? data.aWin[i][1] - mean[1] : mean[1] - data.aWin[i][1];//x
    stddev[2] += data.aWin[i][2] > mean[2] ? data.aWin[i][2] - mean[2] : mean[2] - data.aWin[i][2];//x
  }
  stddev[0] = stddev[0] / (ACCEL_WINDOW_SIZE-1);
  stddev[1] = stddev[1] / (ACCEL_WINDOW_SIZE-1);
  stddev[2] = stddev[2] / (ACCEL_WINDOW_SIZE-1);

  data.meanmag = mean[0] + mean[1] + mean[2] / 3;
  data.stddevmag = stddev[0] + stddev[1] + stddev[2] / 3;

}


int classify(){

  int walk_less_error = 0;
  int stat_less_error = 0;
  int i;
  for( i = 0; i < MODEL_COMPARISONS; i+=NUM_FEATURES ){

    long int stat_mean_err = (stationary[i] > data.meanmag) ?
                          (stationary[i] - data.meanmag) :
                          (data.meanmag - stationary[i]);

    long int stat_sd_err =   (stationary[i+1] > data.stddevmag) ?
                          (stationary[i+1] - data.stddevmag) :
                          (data.stddevmag - stationary[i+1]);

    long int walk_mean_err = (walking[i] > data.meanmag) ?
                          (walking[i] - data.meanmag) :
                          (data.meanmag - walking[i]);

    long int walk_sd_err =   (walking[i+1] > data.stddevmag) ?
                          (walking[i+1] - data.stddevmag) :
                          (data.stddevmag - walking[i+1]);

    if( walk_mean_err < stat_mean_err ){
      walk_less_error ++;
    }else{
      stat_less_error ++;
    }

    if( walk_sd_err < stat_sd_err ){
      walk_less_error ++;
    }else{
      stat_less_error ++;
    }

  }

  if(walk_less_error > stat_less_error+5 ){
    return 1;
  }else{
    return 0;
  }


}



void clearExperimentalData(){
  data.__NV_walkingCount = 0;
  data.__NV_stationaryCount = 0;
  data.__NV_totalCount = 0;
  data.class =0;
  data.currSamp = 0;
  //data.aWin = {0};
}

//__attribute__((section(".init9"), aligned(2)))
int main(int argc, char *argv[]){
  WDTCTL = WDTPW | WDTHOLD;// Stop watchdog timer
  PM5CTL0 &= ~LOCKLPM5;
  initClocks();
  gpioPinSetup();
  P1DIR |= BIT0;
  P1OUT &=~ BIT0;
  P1DIR |= BIT1;
  P1OUT &=~ BIT1;
/*
  if (check == 1) {
      TB0CCR0 = 25000; //setting the compared time
       TB0CCTL0 = CM_1 + CCIE; //enable interrupt for comparison
       TB0CTL = TBSSEL_2 + MC__UP  + TBCLR + ID_3; //setting the time to SMCLK with Time that counts up to ... and divider of 2
       __disable_interrupt();
  }
  nvsHandle4 = nvs_data_init2(nvsStorage4, sizeof(int));
  status = nvs_data_restore2(nvsHandle4, &count);
*/
  //count = 0;
  //status = nvs_data_commit2(nvsHandle4,&count);

  //nvsHandle5 = nvs_data_init2(nvsStorage5, sizeof(int));
  //status = nvs_data_restore2(nvsHandle5, &restart);

  //check = 0;
  //restart = 0;
  //status = nvs_data_commit2(nvsHandle5, &restart);

  //if (check == 1) {
  //    P3OUT |= BIT7;
   //   P3OUT &=~ BIT7;
  //}

  nvsHandle1 = nvs_data_init(nvsStorage1, sizeof(data_t));
  status = nvs_data_restore(nvsHandle1, &data, ctpl_magic_number,whichBox);

  ctpl_init();

  clearExperimentalData();



  status = nvs_data_commit(nvsHandle1, &data, whichBox);
  ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
  //count = count+1;
  //status = nvs_data_commit2(nvsHandle4, &count);

  while(data.__NV_totalCount < 100){


    status = nvs_data_commit(nvsHandle1, &data, whichBox);
    ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
    P4OUT |= BIT3;
   //__delay_cycles(100);
    P4OUT &=~ BIT3;
    //__delay_cycles(100);

    /*
    count =count + TB0R;
    status = nvs_data_commit2(nvsHandle4, &count);
    TB0CTL = TBSSEL_2 + MC__UP  + TBCLR + ID_3; //setting the time to SMCLK with Time that counts up to ... and divider of 2
    */
    /*
    count = count+1;
    status = nvs_data_commit2(nvsHandle4, &count);
    */

    getNextSample();




    status = nvs_data_commit(nvsHandle1, &data,whichBox);
    ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
    P4OUT |= BIT3;
   //__delay_cycles(100);
    P4OUT &=~ BIT3;
    //__delay_cycles(100);


    /*
    count =count + TB0R;
        status = nvs_data_commit2(nvsHandle4, &count);
        TB0CTL = TBSSEL_2 + MC__UP  + TBCLR + ID_3; //setting the time to SMCLK with Time that counts up to ... and divider of 2
    */
    /*
    count = count+1;
      status = nvs_data_commit2(nvsHandle4, &count);
*/

    //status = nvs_data_commit(nvsHandle1, &data);
    //ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
    featurize();


    status = nvs_data_commit(nvsHandle1, &data,whichBox);
    ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
    P4OUT |= BIT3;
    //__delay_cycles(100);
    P4OUT &=~ BIT3;
   // __delay_cycles(100);
    /*
    count =count + TB0R;
        status = nvs_data_commit2(nvsHandle4, &count);
        TB0CTL = TBSSEL_2 + MC__UP  + TBCLR + ID_3; //setting the time to SMCLK with Time that counts up to ... and divider of 2
    //count = count+1;
     // status = nvs_data_commit2(nvsHandle4, &count);
    */

    data.class = classify();
    /*__NV_totalCount, __NV_walkingCount, and __NV_stationaryCount
    have an nv-internal consistency requirement.
    This code should be atomic.
    */





    status = nvs_data_commit(nvsHandle1, &data,whichBox);
    ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
    P4OUT |= BIT3;
    //__delay_cycles(100);
    P4OUT &=~ BIT3;
   // __delay_cycles(100);
    /*
    count =count + TB0R;
        status = nvs_data_commit2(nvsHandle4, &count);
        TB0CTL = TBSSEL_2 + MC__UP  + TBCLR + ID_3; //setting the time to SMCLK with Time that counts up to ... and divider of 2
    //count = count+1;
     // status = nvs_data_commit2(nvsHandle4, &count);
    */
    data.__NV_totalCount = data.__NV_totalCount +1;


    if(data.class == 1){
      data.__NV_walkingCount= data.__NV_walkingCount+1;
    }else{
      data.__NV_stationaryCount = data.__NV_stationaryCount+1;
    }
  }




  status = nvs_data_commit(nvsHandle1, &data,whichBox);
  ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
  check = 0;
  P2OUT |= BIT6;
  P2OUT &=~ BIT6;

  if (data.__NV_stationaryCount == 78) {
      P1OUT |= BIT0;
  }
  else {
      P1OUT |= BIT1;
  }
/*
     int i =0;
     for (i = 0; i<4; i++) {
     P4OUT |= BIT3;

     P4OUT &=~ BIT3;
     __delay_cycles(8000);
     }
*/
     while (1) {

     }


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
#pragma vector = TIMER0_B0_VECTOR
__interrupt void TIMERB_ISR(void) {

    //count = count+1;
    //status = nvs_data_commit2(nvsHandle4, &count);
}
void initClocks(void)
{
    /* Clock System Setup, MCLK = SMCLK = DCO (1MHz), ACLK = VLOCLK */
    CSCTL0_H = CSKEY >> 8;
    CSCTL1 = DCOFSEL_0;
    CSCTL2 = SELA__VLOCLK | SELS__DCOCLK | SELM__DCOCLK;
    CSCTL3 = DIVA__1 | DIVS__1 | DIVM__1;
    CSCTL0_H = 0;
}
