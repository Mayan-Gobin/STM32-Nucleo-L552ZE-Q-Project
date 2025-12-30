
#ifndef DISPLAY_H_
#define DISPLAY_H_

typedef enum{ ALARM=0, CALC = 1, ENVIRO =2, MOTOR=3} Page_t;
#define PAGES 4

typedef enum { RED=0xFF0000, GREEN=0x00FF00, BLUE=0x0000FF, YELLOW=0xFFFF00,
               ORANGE=0xFFA500, CYAN=0x00FFFF, MAGENTA=0xFF00FF, WHITE=0xFFFFFF, OFF=0x000000
} Color_t;

void DisplayEnable(void);
void DisplayPrint(const Page_t page, const int line, const char *msg, ...);
void DisplayColor(const Page_t, Color_t color);

void UpdateDisplay(void);
Page_t GetPage(void);
#endif /* DISPLAY_H_ */
