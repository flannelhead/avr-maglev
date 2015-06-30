#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>
#include <avr/interrupt.h>

#include "util.h"
#include "pid.h"

#define PW_MIN 0
#define PW_MAX 255
#define PW_MIDDLE 60

#define KP 28.0
#define KI 0.005
#define KD 25.0
#define MAX_INTEGRAL 100

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
    return readADC();
}

void setupTimer() {
    /* Prescaler CK/32, period ~1 ms at F_CPU = 8 MHz */
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

    pidInit(KP, KI, KD, MAX_INTEGRAL, &params);
    setupIO();
    setupPWM();
    setupADC();
    setupTimer();
    sei();

    for (;;) loop(&params);
}
