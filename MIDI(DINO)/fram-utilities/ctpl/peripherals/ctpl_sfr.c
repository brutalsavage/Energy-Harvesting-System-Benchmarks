/* --COPYRIGHT--,FRAM-Utilities
 * Copyright (c) 2015, Texas Instruments Incorporated
 * All rights reserved.
 *
 * This source code is part of FRAM Utilities for MSP430 FRAM Microcontrollers.
 * Visit http://www.ti.com/tool/msp-fram-utilities for software information and
 * download.
 * --/COPYRIGHT--*/
#include <msp430.h>

#if defined(__MSP430FR2XX_4XX_FAMILY__) && !defined(__AUTOGENERATED__)
#include <msp430fr2xx_4xxgeneric.h>
#elif defined(__MSP430FR57XX_FAMILY__) && !defined(__AUTOGENERATED__)
#include <msp430fr57xxgeneric.h>
#elif defined(__MSP430FR5XX_6XX_FAMILY__) && !defined(__AUTOGENERATED__)
#include <msp430fr5xx_6xxgeneric.h>
#endif

#ifdef __MSP430_HAS_SFR__

#include <stdint.h>

#include "ctpl_sfr.h"
#include "ctpl_hwreg.h"
#include "../ctpl_low_level.h"

void ctpl_SFR_save(uint16_t baseAddress, uint16_t *storage, uint16_t mode)
{
    /* Save register context to non-volatile storage. */
    storage[1] = HWREG16(baseAddress + OFS_SFRRPCR);
    storage[0] = HWREG16(baseAddress + OFS_SFRIE1);
    
    /* disable UNMI & SNMI interrupt sources (not masked by GIE bit) */
    HWREG16(baseAddress + OFS_SFRIE1) = 0;

    return;
}

void ctpl_SFR_restore(uint16_t baseAddress, uint16_t *storage, uint16_t mode)
{
    /* Restore register context from non-volatile storage. */
    HWREG16(baseAddress + OFS_SFRRPCR) = storage[1];

    return;
}

void ctpl_SFR_epilogue(uint16_t baseAddress, uint16_t *storage, uint16_t mode)
{
    /* Restore NMI interrupt sources with the epilogue function
       to avoid premature ISR call (not masked by GIE bit) */
    HWREG16(baseAddress + OFS_SFRIE1) = storage[0];

    return;
}


#endif //__MSP430_HAS_SFR__
