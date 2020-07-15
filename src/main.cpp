#include <Arduino.h>

#define CONFIG_AC 0
#define CONFIG_ADC 1

volatile uint8_t analogComp = 0;

// Analog comparator interrupt handler
ISR(ANALOG_COMP_vect)
{
    analogComp++;
}

void powerDownConfigAnalog(bool config)
{
    // Configure both the ADC (Analog-to-Digital converter) and the AC (Analog Comparator)
    // We want the ADC to work in normal mode, and the AC to work in Idle (just to be able to wake up the sustem)

    // ACD: AC power disable (not disabled)
    // ACIE: ACD interrupt should be disabled before enabling the ACD
    // ACIC: Analog comparator not connected to Timer/Counter 1
    // ACIS1, ACIS0: Interrupt mode select on Output toggle (rising edge would be enough but toggle works as well for us)
    ACSR = 0;

    if (config == CONFIG_AC)
    {
        // Select A0 as the analog comparator input
        ADMUX &= ~(bit(MUX3) | bit(MUX2) | bit(MUX1) | bit(MUX0));
        // Disable ADC
        ADCSRA &= ~bit(ADEN);
        // Enable Analog Comparator Multiplexer
        ADCSRB |= bit(ACME);

        // ACBG: Fixed bandgap reference voltage replaces the positive input to the Analog Comparator (VBG=1.1V)
        // ACI: ACI is cleared by writing a logic one to the flag
        // ACIE: Enable AC interrupt
        ACSR |= bit(ACBG) | bit(ACI) | bit(ACIE);
    }
    else
    {
        // AC disabled (ACSR = 0)
        // Enable ADC
        ADCSRA |= bit(ADEN);
    }
}

void setup()
{
    // Logs init
    while (!Serial)
        ;
    Serial.begin(9600);
    delay(100);

    powerDownConfigAnalog(CONFIG_AC);

    // Enable interrupts
    interrupts();

    // put your setup code here, to run once:
    pinMode(LED_BUILTIN, OUTPUT);
    //pinMode(A0, INPUT);
}

void loop()
{
    Serial.print("Analog Comp value: ");
    Serial.println(analogComp);

    if (ACSR & (1 << ACO)) // Check status of comparator output flag
        digitalWrite(LED_BUILTIN, HIGH);
    else
        digitalWrite(LED_BUILTIN, LOW);

    delay(1000);
}