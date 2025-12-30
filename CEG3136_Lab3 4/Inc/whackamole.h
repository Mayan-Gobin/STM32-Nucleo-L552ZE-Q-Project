#ifndef WHACKAMOLE_H_
#define WHACKAMOLE_H_

#include <stdint.h>

// Game states
typedef enum {
    GAME_TITLE,
    GAME_PLAYING,
    GAME_SCORE_DISPLAY
} GameState_t;

// Speed settings (in milliseconds)
typedef enum {
    SPEED_SLOW = 1000,    // 1 second
    SPEED_MEDIUM = 500,   // 500ms
    SPEED_FAST = 250      // 250ms
} GameSpeed_t;

// Game configuration
#define GAME_DURATION_MS 60000  // 1 minute
#define MOLES_MAX 8             // Number of mole positions
#define SCORE_DISPLAY_TIME 5000 // 5 seconds to display score

// Function declarations
void Init_WhackAMole(void);
void Task_WhackAMole(void);
void Reset_Game(void);

#endif /* WHACKAMOLE_H_ */
