/*
Magnetic levitation on an AVR microcontroller
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

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "util.h"
#include "pid.h"

#define PW_MIN 0
#define PW_MAX 255
#define PW_MIDDLE 60

#define POT_FACTOR 4
#define POT_OFFSET 256

#define KP 24.0
#define KI 0.0
#define KD 24.0
#define KS 0.0
#define MAX_INTEGRAL 1000

#define MUX_HALL (_BV(MUX1) | _BV(MUX0))
#define MUX_POT _BV(MUX1)

volatile bool gDoControl = false;

void setupIO() {
    /* Port B pin 0 is an output */
    DDRB |= _BV(DDB0);
}

void setupPWM() {
    /* Timer0 output A in fast PWM mode */
    TCCR0A |= _BV(COM0A1) | _BV(WGM01) | _BV(WGM00);
    /* Use the system clock (8 MHz) with no prescaling */
    TCCR0B |= _BV(CS00);
}

void setPulseWidth(int16_t pw) {
    OCR0A = (uint8_t)CLAMP(pw, PW_MIN, PW_MAX);
}

void setupADC() {
    /* Enable the ADC with sufficient prescaling to get a ~60 kHz frequency */
    ADCSRA |= _BV(ADEN) | _BV(ADPS2) | _BV(ADPS1) | _BV(ADPS0);
}

int16_t readADC() {
    /* Start the conversion */
    ADCSRA |= _BV(ADSC);
    /* Wait for the conversion to finish */
    while (ADCSRA & _BV(ADSC));
    /* Return the conversion result */
    return ADCW;
}

int16_t readHallSensor() {
    ADMUX = MUX_HALL;
    return readADC();
}

int16_t readPotentiometer() {
    ADMUX = MUX_POT;
    return POT_OFFSET + readADC() / POT_FACTOR;
}

void setupTimer() {
    /* Prescaler CK/16, timer period ~0.5 ms at F_CPU = 8 MHz */
    TCCR1 |= _BV(CS12) | _BV(CS10);
    /* Interrupt on overflow */
    TIMSK |= _BV(TOIE1);
}

ISR(TIM1_OVF_vect) {
    gDoControl = true;
}

void loop(pidParams_t *params) {
    int16_t setPoint, hallValue;

    while (!gDoControl);
    gDoControl = false;

    setPoint = readPotentiometer();
    hallValue = readHallSensor();

    setPulseWidth(PW_MIDDLE - pidControl(setPoint, hallValue, params));
}

int main() {
    pidParams_t params;

    pidInit(KP, KI, KD, KS, MAX_INTEGRAL, &params);
    setupIO();
    setupPWM();
    setupADC();
    setupTimer();
    sei();

    for (;;) loop(&params);
}
