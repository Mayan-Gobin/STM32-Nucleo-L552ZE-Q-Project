#ifndef TOUCHPAD_H_
#define TOUCHPAD_H_

#include <stdint.h>
#include <stdbool.h>
#include "display.h"

typedef enum {NONE = -1, MIN = 0, N0 = 0, N1 = 1, N2 = 2, N3 = 3, N4 = 4, N5 = 5, N6 = 6, N7 = 7, N8 = 8, N9 = 9, SHIFT = 10, NEXT = 11, MAX = 11} Press_t;
typedef uint32_t Entry_t;

void TouchEnable(void);
Press_t TouchInput(Page_t page);
bool TouchEntry(Page_t page, Entry_t *num);

void ScanTouchpad(void);
void ClearTouchpad(void);

#endif
