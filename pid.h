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

#ifndef PID_H
#define PID_H

#include <stdint.h>

typedef struct {
    float Kp, Ki, Kd, Ks;
    int32_t maxIntegral;

    int16_t lastProcessValue;
    int32_t sumError;
} pidParams_t;

void pidInit(float Kp, float Ki, float Kd, float Ks, int32_t maxIntegral,
    pidParams_t *params);
int16_t pidControl(int16_t setPoint, int16_t processValue, pidParams_t *params);

#endif
