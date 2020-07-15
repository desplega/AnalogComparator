#include <Arduino.h>

volatile boolean triggered;

ISR(ANALOG_COMP_vect)
{
    triggered = true;
}

void setup()
{
    Serial.begin(9600);
    Serial.println("Started.");
    // Disable ADC
    // Select A0 as the analog comparator input
    ADMUX &= ~(bit(MUX3) || bit(MUX2) || bit(MUX1) || bit(MUX0));
    ADCSRA &= ~bit(ADEN);
    // Enable Analog Comparator Multiplexer
    ADCSRB |= bit(ACME);
    ACSR = bit(ACI)                            // (Clear) Analog Comparator Interrupt Flag
           | bit(ACBG) | bit(ACI) | bit(ACIE); // Analog Comparator Interrupt Enable
} // end of setup

void loop()
{
    if (triggered)
    {
        Serial.println("Triggered!");
        triggered = false;
    }

} // end of loop