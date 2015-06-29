#include "pid.h"
#include "util.h"

void pidInit(float Kp, float Ki, float Kd, int32_t maxIntegral,
    pidParams_t *params) {
    params->Kp = Kp;
    params->Ki = Ki;
    params->Kd = Kd;
    params->maxIntegral = maxIntegral;

    params->lastProcessValue = 0;
    params->sumError = 0;
}

int16_t pidControl(int16_t setPoint, int16_t processValue,
    pidParams_t *params) {
    int16_t error;
    int32_t tmp;
    float pTerm, iTerm, dTerm;

    error = setPoint - processValue;

    tmp = params->sumError + error;
    params->sumError = CLAMP(tmp, -params->maxIntegral, params->maxIntegral);

    pTerm = params->Kp * error;
    iTerm = params->Ki * params->sumError;
    dTerm = params->Kd * (params->lastProcessValue - processValue);

    params->lastProcessValue = processValue;

    return (int16_t)CLAMP(pTerm + iTerm + dTerm, INT16_MIN, INT16_MAX);
}
