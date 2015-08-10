/*
A simple PID control library for AVR
Copyright (C) 2015 Sakari Kapanen

This program is free software; you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation; either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License along
with this program; if not, write to the Free Software Foundation, Inc.,
51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "pid.h"
#include "util.h"

void pidInit(float Kp, float Ki, float Kd, float Ks, int32_t maxIntegral,
    pidParams_t *params) {
    params->Kp = Kp;
    params->Ki = Ki;
    params->Kd = Kd;
    params->Ks = Ks;
    params->maxIntegral = maxIntegral;

    params->lastProcessValue = 0;
    params->sumError = 0;
}

int16_t pidControl(int16_t setPoint, int16_t processValue,
    pidParams_t *params) {
    int16_t error;
    int32_t tmp;
    float pTerm, iTerm, dTerm, sTerm;

    error = setPoint - processValue;

    tmp = params->sumError + error;
    params->sumError = CLAMP(tmp, -params->maxIntegral, params->maxIntegral);

    pTerm = params->Kp * error;
    iTerm = params->Ki * params->sumError;
    dTerm = params->Kd * (params->lastProcessValue - processValue);
    sTerm = params->Ks * error * error;
    if (error < 0) sTerm *= -1;

    params->lastProcessValue = processValue;

    return (int16_t)CLAMP(pTerm + iTerm + dTerm + sTerm, INT16_MIN, INT16_MAX);
}
