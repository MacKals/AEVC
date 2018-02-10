/*

AEVC Code for Teensy platform for interfacing with stepper motors and other
perepherals.

*/

#include "core/Arduino.h"

#include "constants.h"
#include "Stepper.hpp"

volatile uint32_t time; // ftm0_cnt_long

bool ledValue = false;
bool stepping = false;

// Link hardware

DrivetrainStepper rightMotor = DrivetrainStepper(M2_DIR, M2_STEP, M2_EN, M2_CHOP, M2_TX, M2_RX);
DrivetrainStepper leftMotor = DrivetrainStepper(M3_DIR, M3_STEP, M3_EN, M3_CHOP, M3_TX, M3_RX);

//Stepper turnMotor = Stepper(M4_DIR, M4_STEP, M4_EN, M4_CHOP, M4_TX, M4_RX);
//Stepper heightMotor = Stepper(M5_DIR, M5_STEP, M5_EN, M5_CHOP, M5_TX, M5_RX);

void step() {
    rightMotor.step(&time);
    leftMotor.step(&time);
}


const String popArgument(String &in) {
    if (in.length() < 2) return NULL;
    for (int i = 1; i < in.length(); i++) {
        if (in.charAt(i) == ' ') {
            if (in.length()-1 > i) in = "";
            in = in.substring(i+1, in.length()-1);
            return in.substring(0, i-1);
        }
    }
}

bool executeCommand(String s) {

    String command, arg1, arg2;

    command = popArgument(s);
    arg1 = popArgument(s);
    arg2 = popArgument(s);

    if (command == "MF") {
        int distance = arg1.toFloat();
        leftMotor.setRelativeTarget(distance);
        rightMotor.setRelativeTarget(distance);
    } else

    if (command == "S") {
        int distance = arg1.toFloat() * WHEELBASE_RADIUS;
        leftMotor.setRelativeTarget(-distance);
        rightMotor.setRelativeTarget(distance);
    } else

    {
        return false;
    }


    return true;
}

// Interrupt service routines
void ftm0_isr(void) {

    // Counter overflow
	if (FTM0_SC & 0x80) {
		time++;  // Increment long counter
		FTM0_SC &= ~0x80; // Reset flag

        if (!time%100000) {
            ledValue = !ledValue;
            digitalWriteFast(LED, ledValue);
        }

        if (stepping) step();
	}

    // Falling edge interrupt
	//if (FTM0_STATUS & 0x20) {
		// Read values and prepare FTM0 for new capture
	//	uint32_t rise = FTM0_C4V;
	//uint32_t fall = FTM0_C5V;
	//	FTM0_STATUS = 0;
}

int main(void) {
	Serial.begin(115200);
	delay(1000);

	/* --- Setup the analog comparator (CMP1) --- */
	SIM_SCGC4 |= SIM_SCGC4_CMP;
	CMP1_CR1 = 0x00;
	CMP1_CR0 = 0x00;

	PORTC_PCR3 = 0x0000; // PC3 alternative 0
	PORTC_PCR4 = 0x0600; // PC4 alternative 6

	CMP1_MUXCR = 0x0F; // Positive input 1 - Negative input DAC
	CMP1_DACCR = 0xF0; // Enable DAC - Vref = Vin1 - DAC voltage 49/64 * Vin
	// The filter will add a delay to the captured pulses, but shouldnt matter as it is the relative time which makes the difference
	CMP1_FPR = 0x01; // Set filter period (of bus clock)
	CMP1_CR0 = 0x73; // Filter 7 samples - 30mV hysteresis
	CMP1_CR1 = 0x13; // Filtered output - Output enable - Comparator enable

	/* --- Setup the flextimer to count pulse lengths --- */
	SIM_SCGC6 |= SIM_SCGC6_FTM0;
	FTM0_SC = 0x00;

	PORTD_PCR4 = 0x0400; // PD4 alternative 4

	FTM0_CNTIN = 0x00; // Set counter to 0
	FTM0_MOD = US_TO_CLOCK(STEP_INTERRUPT_PERIOD); // Free running mode 0xFFFF
	FTM0_MODE=0x05; // Set FTMEN bit

	FTM0_C4SC = 0x14; // Capture rising edge - and continuous capture - on channel 4
	FTM0_C5SC = 0x48; // Capture falling edge - and enable interrupt - on channel 5
	FTM0_COMBINE = 0x00040000; // Enable dual capture mode on channel 4+5

	FTM0_STATUS = 0x00; // Clear any old interrupts
	NVIC_ENABLE_IRQ(IRQ_FTM0); // Enable interrupts for FTM0

	time = 0; // We can count to 32-bits if we do it manually
	FTM0_SC |= 0x40; // Interrupt on overflow

	FTM0_SC |= 0x08; // Set FTM0 clock to system clock
	FTM0_COMBINE |= 0x00080000; // Set DECAP2 to start capture


	/* --- Turn on LED --- */
	PORTC_PCR5  = 0x0100; // GPIO
	GPIOC_PDDR |= 0x0020; // Output
	GPIOC_PSOR  = 0x0020; // Turn on

	PORTC_PCR6  = 0x0100; // GPIO
	PORTC_PCR7  = 0x0100; // GPIO
	GPIOC_PDDR |= 0x00C0; // Output
	GPIOC_PSOR  = 0x00C0; // Turn on

    pinMode(LED, OUTPUT);

    // Main execution loop
	while (1) {

        if (Serial.available()) {
            String a;
            while (Serial.available()) {
                a += Serial.readString();
                if (a.length() << 4) {
                    String last = a.substring(a.length()-2, a.length()-1);
                    if (last == "\r\n") {
                        bool understood = executeCommand(a.substring(0, a.length()-2));
                        Serial.write(understood);
                        a = "";
                    }
                }
            }

        }

        rightMotor.updateStepPeriod();
        leftMotor.updateStepPeriod();
    }

	return 0;
}
