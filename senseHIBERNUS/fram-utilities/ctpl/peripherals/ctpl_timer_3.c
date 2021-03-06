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

#ifdef __MSP430_HAS_TxA7__

#include <stdint.h>

#include "ctpl_timer_3.h"
#include "ctpl_hwreg.h"
#include "../ctpl_low_level.h"

#if !defined(__AUTOGENERATED__) || \
    (__MSP430_HAS_TA0__ == 3) || (__MSP430_HAS_TA1__ == 3) || (__MSP430_HAS_TA2__ == 3) || \
    (__MSP430_HAS_TA3__ == 3) || (__MSP430_HAS_TA4__ == 3) || (__MSP430_HAS_TA5__ == 3)

void ctpl_TIMER_3_save(uint16_t baseAddress, uint16_t *storage, uint16_t mode)
{
    /* Save register context to non-volatile storage. */
    storage[8] = HWREG16(baseAddress + OFS_TAxCTL);

    /* HALT Counter */
    HWREG16(baseAddress + OFS_TAxCTL) &= ~MC_3;

    storage[7] = HWREG16(baseAddress + OFS_TAxCCTL0);
    storage[6] = HWREG16(baseAddress + OFS_TAxCCTL1);
    storage[5] = HWREG16(baseAddress + OFS_TAxCCTL2);
    storage[4] = HWREG16(baseAddress + OFS_TAxR);
    storage[3] = HWREG16(baseAddress + OFS_TAxCCR0);
    storage[2] = HWREG16(baseAddress + OFS_TAxCCR1);
    storage[1] = HWREG16(baseAddress + OFS_TAxCCR2);
    storage[0] = HWREG16(baseAddress + OFS_TAxEX0);

    return;
}

void ctpl_TIMER_3_restore(uint16_t baseAddress, uint16_t *storage, uint16_t mode)
{
    /* Restore register context from non-volatile storage. */
    HWREG16(baseAddress + OFS_TAxEX0)   = storage[0];
    HWREG16(baseAddress + OFS_TAxCCR2)  = storage[1];
    HWREG16(baseAddress + OFS_TAxCCR1)  = storage[2];
    HWREG16(baseAddress + OFS_TAxCCR0)  = storage[3];
    HWREG16(baseAddress + OFS_TAxR)     = storage[4];
    HWREG16(baseAddress + OFS_TAxCCTL2) = storage[5];
    HWREG16(baseAddress + OFS_TAxCCTL1) = storage[6];
    HWREG16(baseAddress + OFS_TAxCCTL0) = storage[7];
    HWREG16(baseAddress + OFS_TAxCTL)   = storage[8];

    return;
}

#elif (__MSP430_HAS_TB0__ == 3) || (__MSP430_HAS_TB1__ == 3) || (__MSP430_HAS_TB2__ == 3) || \
      (__MSP430_HAS_TB3__ == 3) || (__MSP430_HAS_TB4__ == 3) || (__MSP430_HAS_TB5__ == 3)

void ctpl_TIMER_3_save(uint16_t baseAddress, uint16_t *storage, uint16_t mode)
{
    /* Save register context to non-volatile storage. */
    storage[8] = HWREG16(baseAddress + OFS_TBxCTL);

    /* HALT Counter */
    HWREG16(baseAddress + OFS_TBxCTL) &= ~MC_3;

    storage[7] = HWREG16(baseAddress + OFS_TBxCCTL0);
    storage[6] = HWREG16(baseAddress + OFS_TBxCCTL1);
    storage[5] = HWREG16(baseAddress + OFS_TBxCCTL2);
    storage[4] = HWREG16(baseAddress + OFS_TBxR);
    storage[3] = HWREG16(baseAddress + OFS_TBxCCR0);
    storage[2] = HWREG16(baseAddress + OFS_TBxCCR1);
    storage[1] = HWREG16(baseAddress + OFS_TBxCCR2);
    storage[0] = HWREG16(baseAddress + OFS_TBxEX0);

    return;
}

void ctpl_TIMER_3_restore(uint16_t baseAddress, uint16_t *storage, uint16_t mode)
{
    /* Restore register context from non-volatile storage. */
    HWREG16(baseAddress + OFS_TBxEX0)   = storage[0];
    HWREG16(baseAddress + OFS_TBxCCR2)  = storage[1];
    HWREG16(baseAddress + OFS_TBxCCR1)  = storage[2];
    HWREG16(baseAddress + OFS_TBxCCR0)  = storage[3];
    HWREG16(baseAddress + OFS_TBxR)     = storage[4];
    HWREG16(baseAddress + OFS_TBxCCTL2) = storage[5];
    HWREG16(baseAddress + OFS_TBxCCTL1) = storage[6];
    HWREG16(baseAddress + OFS_TBxCCTL0) = storage[7];
    HWREG16(baseAddress + OFS_TBxCTL)   = storage[8];

    return;
}

#endif //__AUTOGENERATED__

#endif //__MSP430_HAS_TxA3__
