#include <stdio.h>
#include <string.h>
#include <time.h>
#include "pico/stdlib.h"
#include "includes/seven_segment.h"

#define BUTTON_PIN			16	// Pin 21 (GPIO 16)

// declaring global variables.

// 'pressed' indicates whether the button has been pressed or not.
bool pressed = false;

// startTime will be used to store the time of when the button was pressed or released depending on which is needed.
clock_t startTime = 0;

// Defines a string 'morseCode' which will have dots and dashes added to it.
char morseCode[6];

// Determines the character to display depending on input.
void decoder();

// Checks if the button press is a dot or a dash.
void checkButton();

int main() {
	stdio_init_all();

	// Initialise the 7 segment display.
	seven_segment_init();

	// Turn the 7 segment display off when the program starts.
	seven_segment_off();

	// Displays a welcome message.
	printf("Welcome to the morse code coursework! \n");

	// 7 segment display turns on & off the middle horizontal segment.
	seven_segment_show(0);
	sleep_ms(500);
	seven_segment_off();

	// Initialize the button's GPIO pin.
	gpio_init(BUTTON_PIN);
	gpio_set_dir(BUTTON_PIN, GPIO_IN);
	gpio_pull_down(BUTTON_PIN); // Pull the button pin towards ground (with an internal pull-down resistor).

	while (true) {
		// This returns true if the button is currently being pressed.
		while (gpio_get(BUTTON_PIN)) {
			if (!pressed) {
				// 'pressed' is set to 'true' as we only want the time since the button was first pressed.
				pressed = true;

				// Middle segment of the 7 segment display is on when the button is being pressed.
				seven_segment_off();
				seven_segment_show(0);	

				// clock() returns the number of clock ticks elapsed since the program started.
				startTime = clock();
			}
		} 

        // Checks if the button press is a dot or a dash.
		// This code will only execute if the startTime has been set.
		if (startTime != 0) {
			checkButton();
		}

		// Small delay of 50ms to stop multiple inputs from one button press.
		sleep_ms(50);
	}
}

void decoder(char* morseCode){
	// Turns of all segments of the display.
	seven_segment_off();

	// Arrays store each combination of inputs and the corresponding letter
	char codes[27][10] = {".-","-...","-.-.","-..",".","..-.","--.","....","..",".---","-.-",".-..","--",
	"-.","---",".--.","--.-",".-.","...","-","..-","...-",".--","-..-","-.--","--..","error"};
	char alphabet[26] = {'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q','R','S','T','U','V','W','X','Y','Z'};
	// Boolean variable matched is used to check if there is a match
	bool matched = false;
	// Checks the string 'morseCode' to see if it matches any morse code letters.
	for (int i=0 ; i < 27 ; i++) {
		// strcmp() returns 0 if the two strings are the same.
		if (!strcmp(morseCode, codes[i])){
			seven_segment_show(i+1);
			matched = true;
			if(i<26){
				printf("\n");
				printf("%c",alphabet[i]);
				printf("\n");
	
			}
		}
	}
	// Prints message if there is no match.
	// Displays the number '8' to show an error.
	if(!matched){
		seven_segment_show(27);
		printf("\nNo match found \n");
	}

	// Set the morseCode string to a blank string.
	strcpy(morseCode, "");

	// Reset the startTime to its default value to reset the program.
	startTime = 0;
}

void checkButton(){
	// 'duration' stores the difference between the current clock time and the start time.
	// 	This calculate the time the button was pressed for.
	double duration;
	duration = (double) (clock() - startTime) / CLOCKS_PER_SEC;
		
		// If the button has been pressed then continue.
		if (pressed) {
			// If the button was pressed for less than 700 ms then continue.
			if (duration <= 0.7) {
				if (duration < 0.25) {
					// If the button was pressed for less than 250ms then a dot is added to the morseCode string.
					printf(".");
					strcat(morseCode, ".");
				} else {
					// If the button was pressed for 250ms or more then a dash is added to the morseCode string.
					printf("-");
					strcat(morseCode, "-");
				}
			// If the button was pressed for more than 700 ms then display an error.
			} else {
				printf("\nError: Button was held longer than 700ms \n");
				strcpy(morseCode, "");
				strcat(morseCode, "error");
			}
			// Set the startTime to the time when the button is released.
			// This allows us to check how long its been since the button was pressed.
			startTime = clock();
			pressed = false;
		// If the button has not been pressed for 400ms after having been pressed then display the letter.
		} else {
			if (duration > 0.4) {
				decoder(morseCode);
			}
		}
}
