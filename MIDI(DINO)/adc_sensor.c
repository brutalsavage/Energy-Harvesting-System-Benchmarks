#include <msp430.h>


void adc12_setup(){

}

int adc12_sample(){

  ADC12CTL0 &= ~ADC12ENC;
  ADC12CTL0 |= ADC12SHT0_8 | ADC12ON;
  ADC12CTL1 |= ADC12SHP;
  ADC12MCTL0 |= ADC12INCH_2 | ADC12VRSEL_0;
  ADC12CTL0 |= ADC12SC | ADC12ENC;
  ADC12IFGR0 = 0;
  ADC12IER0 = ADC12IE0;
  while (ADC12CTL1 & ADC12BUSY);
  int ret = ADC12MEM0;
  ADC12CTL0 &= ~ADC12ENC;
  ADC12CTL1 = 0;
  ADC12CTL0 = 0;

  return ret;

}
