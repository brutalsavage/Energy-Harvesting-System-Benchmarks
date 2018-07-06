//#include <common.h>

#include <msp430.h>
#include <stdio.h>
#include "nvs.h"
#include <ctpl.h>
#include <ctpl_low_level.h>

#define NUMSAMPLES 64

nvs_data_handle nvsHandle1;
nvs_data_handle nvsHandle4;
typedef struct data {
    uint16_t sampmin;
    uint16_t sampmax;
    uint16_t sampmean;
    uint16_t sampstddev;
    uint16_t i;
    uint16_t j;
    uint16_t sumdiff;
    int diff;
    int totalCounter;
    int array[100];
} data_t;

#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(nvsStorage1)
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent
#endif
uint8_t nvsStorage1[NVS_DATA_STORAGE_SIZE(sizeof(data_t))] = {0};


#pragma PERSISTENT(finish);
int finish = 0;
/*
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(nvsStorage4, ".infoA")
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma location="INFOA"
__no_init
#endif

uint8_t nvsStorage4[NVS_DATA_STORAGE_SIZE(sizeof(uint64_t))];
*/


uint16_t status;
data_t data;
//int count;

int adc;
int check = 0;

#define ADC_MONITOR_THRESHOLD   2.2
#define ADC_MONITOR_FREQUENCY   1000
#define MCLK_FREQUENCY          1000000
#define SMCLK_FREQUENCY         1000000

extern void initGpio(void);
extern void initClocks(void);
extern void initAdcMonitor(void);
void save (void);
void enableTimer(void);
extern int ctpl_magic_number;
extern int whichBox;
const unsigned samples[NUMSAMPLES] = {
    81, 577, 207, 752, 327, 687, 212, 382, 512, 335, 770, 522, 357, 503, 850,
    118, 193, 188, 631, 38, 287, 853, 251, 644, 813, 374, 503, 507, 904, 522,
    287, 730, 13, 283, 64, 473, 971, 995, 805, 149, 876, 720, 707, 295, 514,
    465, 874, 252, 442, 770, 98, 185, 312, 2, 195, 785, 888, 52, 658, 94, 350,
    809, 502, 183
};

/* adapted from http://supp.iar.com/FilesPublic/SUPPORT/000419/AN-G-002.pdf */
unsigned fast_sqrt (unsigned radicand) {
    unsigned a, b;
    b = radicand;
    a = radicand = 0x3f;
    radicand = b / radicand;
    a = radicand = (radicand+a)>>1;
    radicand = b / radicand;
    a = radicand = (radicand+a)>>1;
    radicand = b / radicand;
    radicand = (radicand+a)>>1;
    return radicand;
}

unsigned divide (unsigned numerator, unsigned denominator) {
    return (numerator / denominator);
}

void compute (void) {
    data.i =0;
    while(data.i<NUMSAMPLES) {
        if (samples[data.i] < data.sampmin) data.sampmin = samples[data.i];
        if (samples[data.i] > data.sampmax) data.sampmax = samples[data.i];
        data.sampmean = divide(((data.i) * data.sampmean) + samples[data.i], data.i+1);
        data.i = data.i+1;
    }

    data.j =0;

    while(data.j<NUMSAMPLES) {
        data.diff = (samples[data.j] - data.sampmean);
        data.sumdiff += data.diff * data.diff;
        data.j = data.j+1;
    }
    data.sampstddev = fast_sqrt(data.sumdiff / NUMSAMPLES);
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
//MEMENTOS_MAIN_ATTRIBUTES
int main (void) {

    WDTCTL = WDTPW + WDTHOLD; // stop WDT

    initGpio();
    gpioPinSetup();
    initClocks();
    initAdcMonitor();

    nvsHandle1 = nvs_data_init(nvsStorage1, sizeof(data_t));
    status = nvs_data_restore(nvsHandle1, &data,ctpl_magic_number,whichBox);
    //nvsHandle4 = nvs_data_init(nvsStorage4, sizeof(int));
    //status = nvs_data_restore(nvsHandle4, &count);
    P1DIR |= BIT0;
    P1DIR |= BIT1;
    P1OUT &=~ BIT0;
    P1OUT &=~ BIT1;
        __enable_interrupt();
    //count = 0;
    //status = nvs_data_commit(nvsHandle4, &count);
    ctpl_init();
    data.totalCounter =0;
    while(data.totalCounter<50) {
    data.sampmin = 0;
    data.sampmax = 0;
    data.sampmean = 0;
    data.sampstddev = 0;
    compute();
    data.totalCounter = data.totalCounter +1;
    }

    if (data.sampmax == 995) {
        P1OUT |= BIT0;
        P2OUT |= BIT6;
        P2OUT &=~ BIT6;
        P4OUT |= BIT3;
        P4OUT &=~ BIT3;
        finish = 1;
        __disable_interrupt();
    }

    else {
        while (1) {
        P1OUT |= BIT0;
        __delay_cycles(300000);
        P1OUT &=~ BIT0;
        __delay_cycles(300000);
        }
    }

    while(1) {

    }

    //return 91;
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
void enableTimer(void) {
    TA0CCR0 = 5000; //setting the compared time
    TA0CCTL0 = CCIE; //enable interrupt for comparison
    TA0CTL = TASSEL_1 + MC__UP ; //setting the time to SMCLK with Time that counts up to ... and divider of 4
    __enable_interrupt();
}
void initAdcMonitor(void)
{
    /* Initialize timer for ADC trigger. */
    TA0CCR0 = (SMCLK_FREQUENCY/ADC_MONITOR_FREQUENCY);
    TA0CCR1 = TA0CCR0/2;
    TA0CCTL1 = OUTMOD_3;
    TA0CTL = TASSEL__SMCLK | MC__UP;

    /* Configure internal 2.0V reference. */
    while(REFCTL0 & REFGENBUSY);
    REFCTL0 |= REFVSEL_1 | REFON;
    while(!(REFCTL0 & REFGENRDY));
    /*
     * Initialize ADC12_B window comparator using the battery monitor.
     * The monitor will first enable the high side to the monitor voltage plus
     * 0.1v to make sure the voltage is sufficiently above the threshold. When
     * the high side is triggered the interrupt service routine will switch to
     * the low side and wait for the voltage to drop below the threshold. When
     * the voltage drops below the threshold voltage the device will invoke the
     * compute through power loss shutdown function to save the application
     * state and enter complete device shutdown.
     */
    ADC12CTL0 = ADC12SHT0_2 | ADC12ON;
    ADC12CTL1 = ADC12SHS_1 | ADC12SSEL_0 | ADC12CONSEQ_2 | ADC12SHP;
    ADC12CTL3 = ADC12BATMAP;
    ADC12MCTL0 = ADC12INCH_31 | ADC12VRSEL_1 | ADC12WINC;
    ADC12HI = (uint16_t)(4096*((ADC_MONITOR_THRESHOLD+0.1)/2)/(2.0));
    ADC12LO = (uint16_t)(4096*(ADC_MONITOR_THRESHOLD/2)/(2.0));
    ADC12IFGR2 &= ~(ADC12HIIFG | ADC12LOIFG);
    ADC12IER2 = ADC12HIIE;
    ADC12CTL0 |= ADC12ENC;
}

void initGpio(void)
{
    /* Configure GPIO to default state */
    P1OUT  = 0; P1DIR  = 0xFF;
    P2OUT  = 0; P2DIR  = 0xFF;
    P3OUT  = 0; P3DIR  = 0xFF;
    P4OUT  = 0; P4DIR  = 0xFF;
    P5OUT  = 0; P5DIR  = 0xFF;
    P6OUT  = 0; P6DIR  = 0xFF;
    P7OUT  = 0; P7DIR  = 0xFF;
    P8OUT  = 0; P8DIR  = 0xFF;
    PJOUT  = 0; PJDIR  = 0xFFFF;

    /* Disable the GPIO power-on default high-impedance mode. */
    PM5CTL0 &= ~LOCKLPM5;
}
#pragma vector = TIMER0_A0_VECTOR
__interrupt void TIMERA_ISR(void) {
    __bic_SR_register_on_exit(LPM3_bits);
    int i = 0;
    for (i= 0; i<30000; i++) {
    }
    if (adc == 1) {
        __bic_SR_register(LPM3_bits);
    }
}
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12LOIFG)) {
        case ADC12IV_NONE:        break;        // Vector  0: No interrupt
        case ADC12IV_ADC12OVIFG:  break;        // Vector  2: ADC12MEMx Overflow
        case ADC12IV_ADC12TOVIFG: break;        // Vector  4: Conversion time overflow
        case ADC12IV_ADC12HIIFG:                // Vector  6: Window comparator high side
            /* Disable the high side and enable the low side interrupt. */
           // P1OUT ^= BIT1;
            adc = 0;
            ADC12IER2 &= ~ADC12HIIE;
            ADC12IER2 |= ADC12LOIE;
            ADC12IFGR2 &= ~ADC12LOIFG;
            break;
        case ADC12IV_ADC12LOIFG:                // Vector  8: Window comparator low side
            //P1OUT |= BIT1;
            //P1OUT &=~ BIT0;
            /* Disable the low side and enable the high side interrupt. */
            check = 1;
            ADC12IER2 &= ~ADC12LOIE;
            ADC12IER2 |= ADC12HIIE;
            ADC12IFGR2 &= ~ADC12HIIFG;
            status = nvs_data_commit(nvsHandle1, &data,whichBox);
            ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
            if (check == 1) {
               adc = 1;
               P4OUT |= BIT3;
               P4OUT &=~ BIT3;
               enableTimer();
               __bis_SR_register(LPM3_bits + GIE);
            }
            break;
        default: break;
    }
}


