// Alarm system app

#include <stdio.h>
#include "alarm.h"
#include "systick.h"
#include "gpio.h"
#include "display.h"

// GPIO pins
static const Pin_t GreenLED = {GPIOC, 7};  // Pin PC7 -> User LD1
static const Pin_t BlueLED  = {GPIOB, 7};  // Pin PB7 -> User LD2
static const Pin_t RedLED   = {GPIOA, 9};  // Pin PA9 -> User LD3
static const Pin_t Motion   = {GPIOB, 9};  // Pin PB9 <- Motion sensor
static const Pin_t Button   = {GPIOB, 2};  // Pin PB2 <- E-STOP button

static enum {DISARMED, ARMED, TRIGGERED} state;

// Constants
#define LED_TOGGLE_TIME  1000  // Toggle blue/green LEDs every second
#define DEBOUNCE_TIME      50  // Ignore button releases within 50ms
#define SHORT_PRESS_TIME 2000  // Press button for less than 2 seconds
#define LONG_PRESS_TIME  3000  // Hold button for at least 3 seconds

// Variables to record the system time of events
static Time_t toggleTime;  // When blue/green LEDs were last toggled
static Time_t pressTime;   // When the button was last pressed

// Flags to communicate from interrupt callbacks to task state machine
static int motionDetected;   // Motion detected since flag last cleared
static int shortPress;       // Button pressed and released within 3 seconds

// Forward declaration for interrupt callback functions
static void CallbackMotionDetect();
static void CallbackButtonPress();
static void CallbackButtonRelease();

// Initialization code
void Init_Alarm (void) {
	GPIO_Enable(GreenLED);
	GPIO_Mode(GreenLED, OUTPUT);
	GPIO_Output(GreenLED, LOW);

	GPIO_Enable(RedLED);
	GPIO_Mode(RedLED, OUTPUT);
	GPIO_Output(RedLED, LOW);

	GPIO_Enable(BlueLED);
	GPIO_Mode(BlueLED, OUTPUT);
	GPIO_Output(BlueLED, LOW);

	GPIO_Enable(Motion);
	GPIO_Mode(Motion, INPUT);
	GPIO_Callback(Motion, CallbackMotionDetect, RISE);

	GPIO_Enable(Button);
	GPIO_Mode(Button, INPUT);
	GPIO_Callback(Button, CallbackButtonPress, RISE);
	GPIO_Callback(Button, CallbackButtonRelease, FALL);

	motionDetected = 0;
	shortPress = 0;
	state = DISARMED;

	// Initialize display
	DisplayEnable();
	DisplayColor(ALARM, WHITE);
	DisplayPrint(ALARM, 0, "DISARMED");
}

// Task code (state machine)
void Task_Alarm (void) {
	switch (state) {

	case DISARMED:
		GPIO_Output(GreenLED, LOW);
		GPIO_Output(BlueLED, LOW);
		GPIO_Output(RedLED, LOW);

		if (shortPress) {
			shortPress = 0;  // Clear flag
			state = ARMED;
			printf("Short button press at time %u | DISARMED -> ARMED\n", TimeNow());

			// Update display for ARMED state
			DisplayColor(ALARM, GREEN);
			DisplayPrint(ALARM, 0, "ARMED");

			// Set up green/blue LEDs for toggling
			GPIO_Output(GreenLED, HIGH);
			GPIO_Output(BlueLED, LOW);
			toggleTime = TimeNow();
		}

		// Ignore detected motion when system is disarmed
		motionDetected = 0;
		break;

	case ARMED:
		if (GPIO_Input(Button) == HIGH && TimePassed(pressTime) >= LONG_PRESS_TIME) {
			state = DISARMED;
			printf("Long button press at time %u | ARMED -> DISARMED\n", TimeNow());

			// Update display for DISARMED state
			DisplayColor(ALARM, WHITE);
			DisplayPrint(ALARM, 0, "DISARMED");
		}
		else if (motionDetected) {
			motionDetected = 0;  // Clear flag
			state = TRIGGERED;
			printf("Motion detected at time %u | ARMED -> TRIGGERED\n", TimeNow());

			// Update display for TRIGGERED state
			DisplayColor(ALARM, RED);
			DisplayPrint(ALARM, 0, "TRIGGERED");
		}
		else if (TimePassed(toggleTime) >= LED_TOGGLE_TIME) {
			GPIO_Toggle(GreenLED);
			GPIO_Toggle(BlueLED);
			toggleTime = TimeNow();
		}

		// Ignore short button presses when system is already armed
		shortPress = 0;
		break;

	case TRIGGERED:
		GPIO_Output(GreenLED, LOW);
		GPIO_Output(BlueLED, LOW);
		GPIO_Output(RedLED, HIGH);

		if (shortPress) {
			shortPress = 0;  // Clear flag
			state = ARMED;
			printf("Short button press at time %u | TRIGGERED -> ARMED\n", TimeNow());

			// Update display for ARMED state
			DisplayColor(ALARM, YELLOW);
			DisplayPrint(ALARM, 0, "ARMED");

			// Set up green/blue LEDs for toggling
			GPIO_Output(RedLED, LOW);
			GPIO_Output(GreenLED, HIGH);
			GPIO_Output(BlueLED, LOW);
			toggleTime = TimeNow();
		}
		else if (GPIO_Input(Button) == HIGH && TimePassed(pressTime) >= LONG_PRESS_TIME) {
			state = DISARMED;
			printf("Long button press at time %u | TRIGGERED -> DISARMED\n", TimeNow());

			// Update display for DISARMED state
			DisplayColor(ALARM, WHITE);
			DisplayPrint(ALARM, 0, "DISARMED");
		}

		// Ignore further motion detection when already triggered
		motionDetected = 0;
		break;
	}
}

void CallbackMotionDetect (void) {
	motionDetected = 1;  // Set flag
}

void CallbackButtonPress (void) {
	pressTime = TimeNow();
}

void CallbackButtonRelease (void) {
	Time_t heldTime = TimePassed(pressTime);
	if (heldTime > DEBOUNCE_TIME && heldTime < SHORT_PRESS_TIME) {
		shortPress = 1;  // Set flag
	}
}
