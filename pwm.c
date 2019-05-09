/**********************************************************
 *
 * pwmGen.c - Example code which generates a single PWM
 *    output on J4-05 (M0PWM7) with duty cycle fixed and
 *    the frequency controlled by UP and DOWN buttons in
 *    the range 50 Hz to 400 Hz.
 * 2017: Modified for Tiva and using straightforward, polled
 *    button debouncing implemented in 'buttons4' module.
 *
 * P.J. Bones   UCECE
 * Last modified:  7.2.2018
 **********************************************************/
#include "pwm.h"
#include <stdint.h>
#include <stdbool.h>
#include "utils/ustdlib.h"
#include "stdlib.h"
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/pin_map.h" //Needed for pin configure
#include "driverlib/debug.h"
#include "driverlib/gpio.h"
#include "driverlib/pwm.h"
#include "driverlib/systick.h"
#include "driverlib/sysctl.h"
#include "driverlib/interrupt.h"
/**********************************************************
 * Constants
 **********************************************************/
// Systick configuration
#define SYSTICK_RATE_HZ    100



/*******************************************
 *      Local prototypes
 *******************************************/
void SysTickIntHandler (void);
void initClocks (void);
void initSysTick (void);
void initDisplay (void);
void initialisePWM (void);
void setMainPWM (uint32_t ui32MainFreq, uint32_t ui32MainDuty);
void setTailPWM (uint32_t ui32TailFreq, uint32_t ui32TailDuty);
void displayPWM (uint32_t frequency, uint32_t duty_cycle);


/********************************************************
 * Function to set the freq, duty cycle of M0PWM7
 ********************************************************/
void
setMainPWM (uint32_t ui32MainFreq, uint32_t ui32MainDuty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / ui32MainFreq;

    PWMGenPeriodSet(PWM_MAIN_BASE, PWM_MAIN_GEN, ui32Period);
    PWMPulseWidthSet(PWM_MAIN_BASE, PWM_MAIN_OUTNUM,
        ui32Period * ui32MainDuty / 100);
}

void
setTailPWM (uint32_t ui32TailFreq, uint32_t ui32TailDuty)
{
    // Calculate the PWM period corresponding to the freq.
    uint32_t ui32Period =
        SysCtlClockGet() / PWM_DIVIDER / ui32TailFreq;

    PWMGenPeriodSet(PWM_TAIL_BASE, PWM_TAIL_GEN, ui32Period);
    PWMPulseWidthSet(PWM_TAIL_BASE, PWM_TAIL_OUTNUM,
        ui32Period * ui32TailDuty / 100);
}
