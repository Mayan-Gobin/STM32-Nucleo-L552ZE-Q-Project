#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include "calc.h"
#include "display.h"
#include "touchpad.h"
#define NMAX 10


static Press_t operation;
static Entry_t operand[NMAX];
static bool done;
static int count;
static Entry_t result;
static enum {MENU, PROMPT, ENTRY, RUN, SHOW, WAIT} state;
static Page_t lastPage = ALARM;
static int menuScreen = 0;
static int displayIndex = 0;
static int subOperation = 0;


uint32_t Increment(uint32_t n);
uint32_t Decrement(uint32_t n);
uint32_t Factorial(uint32_t n);
uint32_t Fibonacci(uint32_t n);
uint32_t Average(uint32_t *array, uint32_t count);
uint32_t GCD(uint32_t a, uint32_t b);
uint32_t Sort(uint32_t *array, uint32_t count);
uint32_t FourFunction(uint32_t op, uint32_t a, uint32_t b);
// Init_Calc function
void Init_Calc (void) {
  DisplayEnable();
  TouchEnable();
}
// Task_Calc - main calculator function that runs every loop
void Task_Calc (void) {
  Page_t currentPage = GetPage();
  if (currentPage != CALC) {
    lastPage = currentPage;
    return;
  }
  if (lastPage != currentPage) {
    state = MENU;
    menuScreen = 0;
    lastPage = currentPage;
  }
  switch (state) {
  case MENU:
    operation = NONE;
    for (int i = 0; i < NMAX; i++)
      operand[i] = 0;
    count = 0;
    result = 0;
    displayIndex = 0;
    subOperation = 0;
    if (menuScreen == 0) {
      DisplayPrint(CALC, 0, "Calculator app");
      DisplayPrint(CALC, 1, "1:INC 2:DEC, 3:FAC");
    } else if (menuScreen == 1) {
      DisplayPrint(CALC, 0, "Calculator app");
      DisplayPrint(CALC, 1, "4:FIB 5:AVG 6:GCD");
    } else if (menuScreen == 2) {
      DisplayPrint(CALC, 0, "Calculator app");
      DisplayPrint(CALC, 1, "7:SORT 8:4FUNC");
    }
    state = PROMPT;
    break;
  case PROMPT:
    if (operation == NONE) {
      Press_t input = TouchInput(CALC);
      if (input == NEXT) {
        menuScreen++;
        if (menuScreen > 2) menuScreen = 0;
        state = MENU;
      } else if (input == SHIFT) {
        menuScreen--;
        if (menuScreen < 0) menuScreen = 2;
        state = MENU;
      } else if (input != NONE) {
        if (menuScreen == 0) {
          if (input >= 1 && input <= 3) {
            operation = input;
          }
        } else if (menuScreen == 1) {
          if (input >= 4 && input <= 6) {
            operation = input;
          }
        } else if (menuScreen == 2) {
          if (input == 7 || input == 8) {
            operation = input;
          }
        }
      }
    }
    if (operation != NONE) {
      if (operation == 8 && subOperation == 0) {
        DisplayPrint(CALC, 0, "1:ADD 2:SUB");
        DisplayPrint(CALC, 1, "3:MUL 4:DIV");
        Press_t input = TouchInput(CALC);
        if (input >= 1 && input <= 4) {
          subOperation = input;
          DisplayPrint(CALC, 0, "Enter a:");
          DisplayPrint(CALC, 1, "");
          operand[1] = 0;
          count = 1;
          ClearTouchpad();
          state = ENTRY;
        }
      } else if (operation != 8 || subOperation != 0) {
        if (menuScreen == 0) {
          switch ((int)operation) {
          case 1:
          case 2:
          case 3:
            if (count == 1)
              state = RUN;
            else {
              DisplayPrint(CALC, 0, "Enter a number:");
              ClearTouchpad();
              state = ENTRY;
            }
            break;
          default:
            break;
          }
        } else if (menuScreen == 1) {
          switch ((int)operation) {
          case 4:
            DisplayPrint(CALC, 0, "Enter n (>=0):");
            DisplayPrint(CALC, 1, "");
            operand[0] = 0;
            ClearTouchpad();
            state = ENTRY;
            break;
          case 5:
            DisplayPrint(CALC, 0, "Enter #1:");
            DisplayPrint(CALC, 1, "");
            operand[0] = 0;
            count = 0;
            ClearTouchpad();
            state = ENTRY;
            break;
          case 6:
            DisplayPrint(CALC, 0, "Enter a:");
            DisplayPrint(CALC, 1, "");
            operand[0] = 0;
            count = 0;
            ClearTouchpad();
            state = ENTRY;
            break;
          default:
            break;
          }
        } else if (menuScreen == 2) {
          switch ((int)operation) {
          case 7:
            DisplayPrint(CALC, 0, "Enter #1:");
            DisplayPrint(CALC, 1, "");
            operand[0] = 0;
            count = 0;
            ClearTouchpad();
            state = ENTRY;
            break;
          case 8:
            break;
          default:
            break;
          }
        }
      }
    }
    break;
  case ENTRY:
    if (menuScreen == 0) {
      done = TouchEntry(CALC, &operand[count]);
      DisplayPrint(CALC, 1, "%u", operand[count]);
      if (done) {
        count++;
        state = PROMPT;
      }
    } else if (menuScreen == 1) {
      if (operation == 4) {
        done = TouchEntry(CALC, &operand[0]);
        DisplayPrint(CALC, 1, "%u", operand[0]);
        if (done) {
          state = RUN;
        }
      } else if (operation == 5) {
        done = TouchEntry(CALC, &operand[count]);
        DisplayPrint(CALC, 1, "%u", operand[count]);
        if (done) {
          count++;
          if (count >= NMAX) {
            state = RUN;
          } else if (count > 1 && operand[count-1] == 0) {
            count--;
            state = RUN;
          } else {
            DisplayPrint(CALC, 0, "Enter #%d:", count + 1);
            DisplayPrint(CALC, 1, "");
            operand[count] = 0;
            ClearTouchpad();
          }
        }
      } else if (operation == 6) {
        if (count == 0) {
          done = TouchEntry(CALC, &operand[0]);
          DisplayPrint(CALC, 1, "%u", operand[0]);
          if (done) {
            count = 1;
            DisplayPrint(CALC, 0, "Enter b:");
            DisplayPrint(CALC, 1, "");
            operand[1] = 0;
            ClearTouchpad();
          }
        } else if (count == 1) {
          done = TouchEntry(CALC, &operand[1]);
          DisplayPrint(CALC, 1, "%u", operand[1]);
          if (done) {
            count = 2;
            state = RUN;
          }
        }
      }
    } else if (menuScreen == 2) {
      if (operation == 7) {
        done = TouchEntry(CALC, &operand[count]);
        DisplayPrint(CALC, 1, "%u", operand[count]);
        if (done) {
          count++;
          if (count >= NMAX) {
            state = RUN;
          } else if (count > 1 && operand[count-1] == 0) {
            count--;
            state = RUN;
          } else {
            DisplayPrint(CALC, 0, "Enter #%d:", count + 1);
            DisplayPrint(CALC, 1, "");
            operand[count] = 0;
            ClearTouchpad();
          }
        }
      } else if (operation == 8) {
        if (count == 1) {
          done = TouchEntry(CALC, &operand[1]);
          DisplayPrint(CALC, 1, "%u", operand[1]);
          if (done) {
            count = 2;
            DisplayPrint(CALC, 0, "Enter b:");
            DisplayPrint(CALC, 1, "");
            operand[2] = 0;
            ClearTouchpad();
          }
        } else if (count == 2) {
          done = TouchEntry(CALC, &operand[2]);
          DisplayPrint(CALC, 1, "%u", operand[2]);
          if (done) {
            count = 3;
            state = RUN;
          }
        }
      }
    }
    break;
  case RUN:
    DisplayPrint(CALC, 0, "Calculating...");
    DisplayPrint(CALC, 1, "");
    state = SHOW;
    if (menuScreen == 0) {
      switch ((int)operation) {
      case 1: result = Increment(operand[0]); break;
      case 2: result = Decrement(operand[0]); break;
      case 3: result = Factorial(operand[0]); break;
      default: break;
      }
    } else if (menuScreen == 1) {
      switch ((int)operation) {
      case 4: result = Fibonacci(operand[0]); break;
      case 5:
        if (count > 0) {
          result = Average(operand, count);
        } else {
          result = 0;
        }
        break;
      case 6:
        if (count >= 2) {
          result = GCD(operand[0], operand[1]);
        } else {
          result = 0;
        }
        break;
      default: break;
      }
    } else if (menuScreen == 2) {
      switch ((int)operation) {
      case 7:
        if (count > 0) {
          Sort(operand, count);
          displayIndex = 0;
        }
        break;
      case 8:
        if (count >= 3 && subOperation > 0) {
          result = FourFunction(subOperation, operand[1], operand[2]);
        }
        break;
      default: break;
      }
    }
    break;
  case SHOW:
    if (menuScreen == 2 && operation == 7) {
      if (displayIndex < count) {
        DisplayPrint(CALC, 0, "Sorted #%d:", displayIndex + 1);
        DisplayPrint(CALC, 1, "%u", operand[displayIndex]);
        if (TouchInput(CALC) == NEXT) {
          displayIndex++;
          ClearTouchpad();
        }
        if (displayIndex >= count) {
          state = WAIT;
        }
      } else {
        state = WAIT;
      }
    } else {
      DisplayPrint(CALC, 0, "Result:");
      if (menuScreen == 1 && operation == 4) {
        DisplayPrint(CALC, 1, "fib(%u)=%u", operand[0], result);
      } else if (menuScreen == 1 && operation == 5) {
        uint32_t whole = result / 10;
        uint32_t decimal = result % 10;
        DisplayPrint(CALC, 1, "avg=%u.%u", whole, decimal);
      } else if (menuScreen == 1 && operation == 6) {
        DisplayPrint(CALC, 1, "gcd(%u,%u)=%u", operand[0], operand[1], result);
      } else if (menuScreen == 2 && operation == 8) {
        char opChar = (subOperation == 1) ? '+' : (subOperation == 2) ? '-' : (subOperation == 3) ? '*' : '/';
        DisplayPrint(CALC, 1, "%u%c%u=%u", operand[1], opChar, operand[2], result);
      } else {
        DisplayPrint(CALC, 1, "%u", result);
      }
      state = WAIT;
    }
    break;
  case WAIT:
    if (TouchInput(CALC) != NONE) {
      state = MENU;
      displayIndex = 0;
      subOperation = 0;
    }
    break;
  }
}
