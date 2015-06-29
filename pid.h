#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct {
    float Kp, Ki, Kd;
    int32_t maxIntegral;

    int16_t lastProcessValue;
    int32_t sumError;
} pidParams_t;

void pidInit(float Kp, float Ki, float Kd, int32_t maxIntegral,
    pidParams_t *params);
int16_t pidControl(int16_t setPoint, int16_t processValue, pidParams_t *params);

#endif
