
#pragma once

#include "../drivers/customFlash.h"
#include "../drivers/powerComparator.h"


/*
 * This is .h file, but defines code that is not called except by handlers,
 * and code that is to be cut and pasted into the main app (to get it to override weak handlers.)
 * This is #include'd in the library.
 */

/*
 * Handler for debugging:
 * - hard faults
 * - brownout
 *
 * Use:
 * - with debugger
 * - without debugger, recording fault PC in flash
 *
 * Original code from Nordic Devzone: Back trace from hard fault handler with s110 flashed?
 */

extern "C" {

//void HardFault_HandlerFlash(unsigned long *);
void ExceptionHandlerWritePCToFlash(unsigned long *);
void DebugHandler(void);

/*
 * Called from DebugHandler
 * Parameter is pointer to stack frame.
 * Writes PC to flash.
 * Never returns.
 */
void ExceptionHandlerWritePCToFlash(unsigned long *hardfault_args){

    volatile unsigned long stacked_pc;

    // PC was pushed on stack and is 6 words above current SP (the frame)
    stacked_pc = ((unsigned long)hardfault_args[6]) ;

    /*
     * Since for some power supplies, the system may repeatedly brownout and POR,
     * and since flash is not writeable more than once,
     * only write PC if not written already.
     */
    if ( ! CustomFlash::isWrittenAtIndex(LineNumberFlagIndex)) {
    	/*
    	 * !!! First must counteract HW lock of flash during brownout.
    	 * Disable comparator so it doesn't trigger again when we clear the event.
    	 */
    	PowerComparator::disable();
    	PowerComparator::clearPOFEvent();
    	// assert NVMC is not HW locked against writes

    	CustomFlash::writeIntAtIndex(LineNumberFlagIndex, stacked_pc);
    }

    /*
     * Typically no further execution is possible (browning out)
     * or we want to stop execution (at time of fault.)
     */
    __asm("BKPT #0\n") ; // Break into the debugger
    // What does this do if no debugger?

    while(true) {};
}

#ifdef NOT_USED
This is original code, from the web.

/**
* HardFaultHandler_C:
* This is called from the HardFault_HandlerAsm with a pointer the Fault stack
* as the parameter. We can then read the values from the stack and place them
* into local variables for ease of reading.
* We then read the various Fault Status and Address Registers to help decode
* cause of the fault.
* The function ends with a BKPT instruction to force control back into the debugger
*/
void HardFault_HandlerC(unsigned long *hardfault_args){
    volatile unsigned long stacked_r0 __attribute__((unused));
    volatile unsigned long stacked_r1 __attribute__((unused));
    volatile unsigned long stacked_r2 __attribute__((unused));
    volatile unsigned long stacked_r3 __attribute__((unused));
    volatile unsigned long stacked_r12 __attribute__((unused));
    volatile unsigned long stacked_lr __attribute__((unused));
    volatile unsigned long stacked_pc __attribute__((unused));
    volatile unsigned long stacked_psr __attribute__((unused));
    volatile unsigned long _CFSR __attribute__((unused));
    volatile unsigned long _HFSR __attribute__((unused));
    volatile unsigned long _DFSR __attribute__((unused));
    volatile unsigned long _AFSR __attribute__((unused));
    volatile unsigned long _BFAR __attribute__((unused));
    volatile unsigned long _MMAR __attribute__((unused));

    stacked_r0 = ((unsigned long)hardfault_args[0]) ;
    stacked_r1 = ((unsigned long)hardfault_args[1]) ;
    stacked_r2 = ((unsigned long)hardfault_args[2]) ;
    stacked_r3 = ((unsigned long)hardfault_args[3]) ;
    stacked_r12 = ((unsigned long)hardfault_args[4]) ;
    stacked_lr = ((unsigned long)hardfault_args[5]) ;
    stacked_pc = ((unsigned long)hardfault_args[6]) ;
    stacked_psr = ((unsigned long)hardfault_args[7]) ;

    // Configurable Fault Status Register
    // Consists of MMSR, BFSR and UFSR
    _CFSR = (*((volatile unsigned long *)(0xE000ED28))) ;

    // Hard Fault Status Register
    _HFSR = (*((volatile unsigned long *)(0xE000ED2C))) ;

    // Debug Fault Status Register
    _DFSR = (*((volatile unsigned long *)(0xE000ED30))) ;

    // Auxiliary Fault Status Register
    _AFSR = (*((volatile unsigned long *)(0xE000ED3C))) ;

    // Read the Fault Address Registers. These may not contain valid values.
    // Check BFARVALID/MMARVALID to see if they are valid values
    // MemManage Fault Address Register
    _MMAR = (*((volatile unsigned long *)(0xE000ED34))) ;
    // Bus Fault Address Register
    _BFAR = (*((volatile unsigned long *)(0xE000ED38))) ;

    __asm("BKPT #0\n") ; // Break into the debugger

}
#endif



#ifdef NOT_USED
This is a template to be copied to calling app.
DO NOT DELETE

/**
*
* Handler to help debug the reason for a fault or IRQ.
* Typically HardFault_Handler or brownout IRQ (POWER_CLOCK_IRQHandler)
*
* To use, copy this to the main project and rename it,
* so it link-time overrides the weak handler defined in
* e.g. file gcc_startup_nrf51.S.
*
* Suitable for Cortex-M3 and Cortex-M0 cores.
* ??? Why not the Cortex-M4
*/

/*
 * 'naked' attribute so C stacking is not used.
 * C stacking would change
 */
__attribute__((naked))
void DebugHandler(void){
/*
 * Get stack pointer that depends on mode.
 * (If RTOS in use, priveleged/user mode uses different stacks?
 * Pass SP to a C handler.
 * This function will never return.
 *
 * Modifications from original code:
 * - use bl instead of b which suffers link error "relocation truncated to fit"
 * - remove trailing ".syntax divided\n") which causes link error "instruction not supported in Thumb16" ???
 */

__asm(  ".syntax unified\n"
        "MOVS   R0, #4  // test mode\n"
        "MOV    R1, LR  \n"
        "TST    R0, R1  \n"
        "BEQ    _MSP    \n"
        "MRS    R0, PSP // load r0 with MainSP\n"
        "bl      ExceptionHandlerWritePCToFlash      \n"
        "_MSP:  \n"
        "MRS    R0, MSP // load r0 with ProcessSP\n"
        "bl      ExceptionHandlerWritePCToFlash      \n"
#ifdef NRF51
		".syntax divided \n"
#endif
        );
}

// XXX use B __cpp(ExceptionHandlerWritePCToFlash) without extern C?
#endif

}	// extern C
