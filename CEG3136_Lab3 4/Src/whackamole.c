#include "whackamole.h"
#include "gpio.h"
#include "display.h"
#include "systick.h"
#include <stdlib.h>


static GameState_t gameState = GAME_TITLE;
static GameSpeed_t currentSpeed = SPEED_MEDIUM;
static int score = 0;
static uint8_t activeMoles = 0;  // LEDs for active moles (bits 0-7)
static Time_t moleTimers[8];     // Timer for each mole (allows us to track when the mole is active longer than it should be)
static Time_t gameStartTime;
static Time_t lastSpawnTime;
static Time_t lastTitleUpdate;

//We use a latch based system in order to trach button presses
static uint8_t buttonLatched = 0;

static GameSpeed_t lastShownSpeed = (GameSpeed_t)(-1);
static int lastShownScore = -1;

void Init_WhackAMole(void) {
    GPIO_PortEnable(GPIOX);
    gameState = GAME_TITLE;
    score = 0;
    activeMoles = 0;
    currentSpeed = SPEED_MEDIUM;
    lastTitleUpdate = TimeNow();
    buttonLatched = 0;
    srand(TimeNow());
}

void Reset_Game(void) {
    gameState = GAME_TITLE;
    score = 0;
    activeMoles = 0;
    lastTitleUpdate = TimeNow();
    buttonLatched = 0;
    lastShownSpeed = (GameSpeed_t)(-1);
    lastShownScore = -1;
}

void Task_WhackAMole(void) {
    uint8_t switches = (GPIO_PortInput(GPIOX) >> 8) & 0xFF;  // SW1..SW8 & has masks for the LEDS as well


    switch (gameState) {
        case GAME_TITLE:
            //Random LEDs based on the currentSpeed of the game
            if (TimePassed(lastTitleUpdate) >= currentSpeed) {
                GPIO_PortOutput(GPIOX, rand() & 0xFF);
                lastTitleUpdate = TimeNow();
            }

            //We look at what happens when the SW5 button is pressed
            if (!buttonLatched && (switches & (1 << 4))) {
                if (currentSpeed == SPEED_SLOW) currentSpeed = SPEED_MEDIUM;
                else if (currentSpeed == SPEED_MEDIUM) currentSpeed = SPEED_FAST;
                else currentSpeed = SPEED_SLOW;
                // Helps to keep the animations all smooth
                lastTitleUpdate = TimeNow();
                buttonLatched = 1;
            }
            // Show speed on the display (only when it changes)
            if (lastShownSpeed != currentSpeed) {
                const char *label = (currentSpeed == SPEED_SLOW) ? "SLOW" :
                                    (currentSpeed == SPEED_MEDIUM) ? "MEDIUM" : "FAST";
                //DisplayPrint(ALARM, 0, "SPEED: %s", label);
                //DisplayPrint(CALC, 1, "SW4 to start");
                lastShownSpeed = currentSpeed;
            }

            // SW4 starts game — once per press
            if (!buttonLatched && (switches & (1 << 3))) {
                gameState = GAME_PLAYING;
                gameStartTime = TimeNow();
                lastSpawnTime = TimeNow();
                activeMoles = 0;
                score = 0;
                GPIO_PortOutput(GPIOX, 0);
                lastShownScore = -1; // force score update
                buttonLatched = 1;
            }
            if (!switches) buttonLatched = 0; // release when all buttons up
            break;

        case GAME_PLAYING:
            // End game after 1 minute no matter what
            if (TimePassed(gameStartTime) >= GAME_DURATION_MS) {
                gameState = GAME_SCORE_DISPLAY;
                break;
            }

            //Mole spawning algorithm
            if (TimePassed(lastSpawnTime) >= currentSpeed) {
                int pos = rand() % 8;  // Try random position
                if (!(activeMoles & (1 << pos))) {  // If empty, spawn
                    activeMoles |= (1 << pos);
                    moleTimers[pos] = TimeNow();
                }
                lastSpawnTime = TimeNow();
            }

            //This loop removes moles that are spawned for longer than 4 seconds
            for (int i = 0; i < 8; i++) {
                if ((activeMoles & (1 << i)) && (TimePassed(moleTimers[i]) >= (currentSpeed * 4))) {
                    activeMoles &= ~(1 << i);
                }
            }

            //What happens when a button is pressed during the game
            if (!buttonLatched && switches) {
                int pressed = -1;
                // Loops until it finds a SW that has been pressed
                for (int i = 7; i >= 0; i--) {
                    if (switches & (1 << i)) { pressed = i; break; }
                }

                if (pressed >= 0) {
                    if (activeMoles & (1 << pressed)) {
                        activeMoles &= ~(1 << pressed); //Clears the mole bit once that switch is pressed
                        score++;
                    } else if (score > 0) {  //What happens when a user misses
                        score--;
                    }
                    buttonLatched = 1; //Variable set to one until switch is released.
                }
            }

            // Shows active moles
            GPIO_PortOutput(GPIOX, activeMoles);
            // Shows the score when it changes
            if (lastShownScore != score) {
                //DisplayPrint(0, "SCORE: %d", score);
                lastShownScore = score;
            }
            if (!switches) buttonLatched = 0; // release when all buttons up
            break;

        case GAME_SCORE_DISPLAY:
            // Show score until any button press
            GPIO_PortOutput(GPIOX, score & 0xFF);
            if (switches) Reset_Game();
            break;
    }

}

