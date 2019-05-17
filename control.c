#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "inc/hw_memmap.h"
#include "inc/hw_types.h"
#include "driverlib/gpio.h"
#include "control.h"
#include "pwm.h"
#include "quadrature.h"

static int referencePercentHeight;
static int currentPercentHeight;

static uint8_t currentMode;

static int heightError;
static int yawError;

static double yawErrorPrevious;
static double heightErrorPrevious;
static double heightErrorIntegrated;
static double yawErrorIntegrated;
static double heightErrorDerivative;
static double yawErrorDerivative;

static uint8_t referenceFound;

static int16_t outputMain;
static int16_t outputTail;

static int referenceYaw;
static int currentYaw;
static volatile int lastRefCrossing;
static int yawFind = 15;

void findIndependentYawReference(void) {

    // Begin flying heli, rotate CCW slowly
    setReferenceHeight(TAKE_OFF_HEIGHT);
    setReferenceYaw(yawFind);

    if (yawError < 2) {
        yawFind += 15;
    }

    // Read PC4 while it is high (while the independent reference isn't found)
    if (lastRefCrossing != 0) {
        resetYawSlots();
        setReferenceYaw(ZERO_YAW);
        lastRefCrossing = ZERO_YAW;
        setMode(FLYING);
    }
}


void setLastRefCrossing(int yawSlotCount) {
    lastRefCrossing = yawSlotCount;
}


void setCurrentHeight(int height) {
    currentPercentHeight = height;
}


void setCurrentYaw(int yaw) {
    currentYaw = yaw;
}


void setReferenceUp(void) {
    if (currentMode == FLYING) {
        referencePercentHeight += HEIGHTSTEP;
        if (referencePercentHeight > MAXHEIGHT) {
            referencePercentHeight = MAXHEIGHT;
        }
    }
}


void setReferenceDown(void) {
    if (currentMode == FLYING) {
        referencePercentHeight -= HEIGHTSTEP;
        if (referencePercentHeight < MINHEIGHT) {
            referencePercentHeight = MINHEIGHT;
        }
    }
}

void setReferenceCW(void) {
    if (currentMode == FLYING) {
        referenceYaw += YAWSTEP;
    }
}


void setReferenceYaw(int yaw) {
    referenceYaw = yaw;
}

void setReferenceHeight(int height) {
    referencePercentHeight = height;
}


void setReferenceCCW(void) {
    if (currentMode == FLYING) {
        referenceYaw -= YAWSTEP;
    }
}


void setMode(uint8_t mode) {
    currentMode = mode;
}


int getDistanceYaw(void) {
    return yawError;
}


int getDistanceHeight(void) {
    return heightError;
}

int getClosestRef(void) {
    if ((currentYaw - lastRefCrossing) < ((lastRefCrossing + TOTAL_SLOTS) - currentYaw)) {
        return lastRefCrossing;
    } else {
        return lastRefCrossing + TOTAL_SLOTS;
    }
}


void updateYaw(void) {
    yawError = referenceYaw - currentYaw; // yaw error signal
    yawErrorIntegrated += yawError * DELTA_T; // integral of yaw error signal
    yawErrorDerivative = (yawError - yawErrorPrevious) / DELTA_T; // derivative of error signal

    yawErrorPrevious = yawError;

    outputTail = (KpTail * yawError) + (KiTail * yawErrorIntegrated) + (KdTail * yawErrorDerivative);

    if (outputTail > 98) {
        outputTail = 98;
    }

    if (outputTail < 2) {
        outputTail = 2;
    }

    setTailPWM(PWM_TAIL_START_RATE_HZ, outputTail);
}


void updateHeight(void) {
    heightError = referencePercentHeight - currentPercentHeight; // height error signal
    heightErrorIntegrated += heightError * DELTA_T; // height integral of error signal
    heightErrorDerivative = (heightError - heightErrorPrevious) / DELTA_T; // derivative of error signal

    heightErrorPrevious = heightError;

    outputMain = (KpMain * heightError) + (KiMain * heightErrorIntegrated) + (KdMain * heightErrorDerivative);

    if (outputMain > 98) {
        outputMain = 98;
    }

    if (outputMain < 2) {
        outputMain = 2;
    }

    setMainPWM(PWM_MAIN_START_RATE_HZ, outputMain);
}

int16_t getOutputMain(void) {
    return outputMain;
}


int16_t getOutputTail(void) {
    return outputTail;
}


void updateControl(void) {
    switch (currentMode) {
        case (LANDING):
                int closestRef;
                closestRef = getClosestRef();
                setReferenceYaw(closestRef);
                updateYaw();
                if (closestRef < currentYaw + 10 && closestRef > currentYaw - 10) {
                    setReferenceHeight((referencePercentHeight-10));
                }
                break;
        case (TAKINGOFF):
                findIndependentYawReference();
                updateYaw();
                updateHeight();
                break;
        case (FLYING):
                updateYaw();
                updateHeight();
                break;
        case (LANDED):
                lastRefCrossing = 0;
                setMainPWM(PWM_MAIN_START_RATE_HZ, PWM_OFF);
                setTailPWM(PWM_TAIL_START_RATE_HZ, PWM_OFF);
                break;
    }
}
