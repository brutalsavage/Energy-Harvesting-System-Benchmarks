
#include "msp430.h"
#include <stdio.h>
#include "nvs.h"
#include <ctpl.h>
#include <ctpl_low_level.h>
/**
 * CRC-16 from WISP firmware 4.1.  Comment from their code follows:
 * <quote>
 * This code comes from the Open Tag Systems Protocol Reference Guide version
 * 1.1 dated 3/23/2004.
 * (http://www.opentagsystems.com/pdfs/downloads/OTS_Protocol_v11.pdf) No
 * licensing information accompanied the code snippet.
 * </quote>
 **/

nvs_data_handle nvsHandle1;
nvs_data_handle nvsHandle4;
typedef struct data {
    unsigned short crc_16;
    unsigned short i;
    unsigned short j;
    volatile unsigned short accumulator;
    unsigned int counter;
    unsigned short n;
    volatile unsigned char *info;
} data_t;

#if defined(__TI_COMPILER_VERSION__)
#pragma PERSISTENT(nvsStorage1)
#elif defined(__IAR_SYSTEMS_ICC__)
__persistent
#endif
uint8_t nvsStorage1[NVS_DATA_STORAGE_SIZE(sizeof(data_t))] = {0};
/*
#if defined(__TI_COMPILER_VERSION__)
#pragma DATA_SECTION(nvsStorage4, ".infoA")
#elif defined(__IAR_SYSTEMS_ICC__)
#pragma location="INFOA"
__no_init
#endif

uint8_t nvsStorage4[NVS_DATA_STORAGE_SIZE(sizeof(uint64_t))];
*/

int check = 1;
uint16_t status;
data_t data;
//int count;

extern void initGpio(void);
extern void initClocks(void);
extern void initAdcMonitor(void);
void save (void);
extern int ctpl_magic_number;
extern int whichBox;
#define ADC_MONITOR_THRESHOLD   2.2
#define ADC_MONITOR_FREQUENCY   1000
#define MCLK_FREQUENCY          1000000
#define SMCLK_FREQUENCY         1000000

unsigned short crc16_ccitt(volatile unsigned char *info, unsigned short n) {
    data.i = 0;
    data.j =0;
    data.n = n;
    data.crc_16 = 0xFFFFu; // Equivalent Preset to 0x1D0F
    data.info = info;
    while(data.i<data.n) {
        data.crc_16^=data.info[data.i] << 8;
        data.j = 0;
        while(data.j< 8) {
            if (data.crc_16 & 0x8000) {
                data.crc_16 <<= 1;
                data.crc_16 ^= 0x1021; // (CCITT) x16 + x12 + x5 + 1
            } else {
                data.crc_16 <<= 1;
            }
            data.j = data.j+1;
            if (check == 0) {
                save();
            }
        }
        data.i = data.i +1;
        if (check == 0) {
            save();
        }
    }
    return(data.crc_16^0xFFFFu);
}

void do_crc (void) {
    // check the upper halves of eight 512B flash segments
    data.accumulator = 0;
    data.counter=0;
    while(data.counter < 8) {
        data.accumulator += crc16_ccitt((unsigned char *)(0xE000 + data.counter*512), 0x100);
        data.counter = data.counter+1;
        if (check == 0) {
            save();
        }
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
//MEMENTOS_MAIN_ATTRIBUTES
int main (void) {
    WDTCTL = WDTPW + WDTHOLD; // stop WDT

    initGpio();
    gpioPinSetup();
    initClocks();
    initAdcMonitor();
    P1DIR |= BIT0;
    P1DIR |= BIT1;
    P1OUT &=~ BIT0;
    P1OUT &=~ BIT1;
    __enable_interrupt();
    nvsHandle1 = nvs_data_init(nvsStorage1, sizeof(data_t));
    status = nvs_data_restore(nvsStorage1, &data,ctpl_magic_number,whichBox);
    //nvsHandle4 = nvs_data_init(nvsStorage4, sizeof(int));
    //status = nvs_data_restore(nvsHandle4, &count);
    ctpl_init();
    do_crc();
    if (data.accumulator == 9856) {
        P1OUT |= BIT0;
        P4OUT |= BIT3;
        P4OUT &=~ BIT3;
        P2OUT |= BIT6;
        P2OUT &=~ BIT6;
    }
    while(1) {

    }
    return 27; // to indicate success
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
void initAdcMonitor()
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
#pragma vector = ADC12_VECTOR
__interrupt void ADC12_ISR(void)
{
    switch(__even_in_range(ADC12IV, ADC12IV_ADC12LOIFG)) {
            case ADC12IV_NONE:        break;        // Vector  0: No interrupt
            case ADC12IV_ADC12OVIFG:  break;        // Vector  2: ADC12MEMx Overflow
            case ADC12IV_ADC12TOVIFG: break;        // Vector  4: Conversion time overflow
            case ADC12IV_ADC12HIIFG:                // Vector  6: Window comparator high side
                /* Disable the high side and enable the low side interrupt. */
               // P1OUT |= BIT1;
               // __delay_cycles(300000);
               // P1OUT &=~ BIT1;
                ADC12IER2 &= ~ADC12HIIE;
                ADC12IER2 |= ADC12LOIE;
                ADC12IFGR2 &= ~ADC12LOIFG;
                break;
            case ADC12IV_ADC12LOIFG:                // Vector  8: Window comparator low side
                //P1OUT |= BIT0;
                //P1OUT &=~ BIT0;
                check = 0;
                /* Disable the low side and enable the high side interrupt. */
                ADC12IER2 &= ~ADC12LOIE;
                ADC12IER2 |= ADC12HIIE;
                ADC12IFGR2 &= ~ADC12HIIFG;
                break;
            default: break;
        }
}
void save(void) {
    check = 1;
    //P1OUT &=~ BIT1;
    P4OUT |= BIT3;
     P4OUT &=~ BIT3;
    status = nvs_data_commit(nvsHandle1, &data,whichBox);
    ctpl_saveCpuStackEnterLpm(CTPL_MODE_NONE,0);
}
