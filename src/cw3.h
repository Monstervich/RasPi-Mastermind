#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdint.h>
#include <ctype.h>
#include <poll.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <fcntl.h>
#include <pthread.h>
#include <sys/time.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/ioctl.h>

#ifndef TRUE
#define TRUE    (1==1)
#define FALSE   (1==2)
#endif

#define PAGE_SIZE               (4*1024)
#define BLOCK_SIZE              (4*1024)

#define INPUT   0
#define OUTPUT  1

#define LOW     0
#define HIGH    1

#define INPUT_WAIT 8000		//how long player has to input each number in millisecs

#define COLOURS 3
#define PEGS    3
#define TURNS   3

#define RED_LED         26
#define YELLOW_LED      12
#define BUTTON          16

#define LCD_RS		25
#define LCD_EN		24
#define LCD_DAT4	23
#define LCD_DAT5	17
#define LCD_DAT6	27
#define LCD_DAT7	22

#define LCD_CLEAR 	0x01
#define LCD_HOME 	0x02
#define LCD_ENTRY 	0x04
#define LCD_CTRL 	0x08
#define LCD_CDSHIFT 	0x10
#define LCD_FUNC 	0x20
#define LCD_CGRAM 	0x40
#define LCD_DGRAM 	0x80

#define LCD_ENTRY_SH	0x01
#define LCD_ENTRY_ID	0x02

#define LCD_BLINK_CTRL	0x01
#define LCD_CURSOR_CTRL	0x02
#define LCD_DISPLAY_CTRL 0x04

#define LCD_FUNC_F	0x04
#define LCD_FUNC_N	0x08
#define LCD_FUNC_DL	0x10

#define LCD_CDSHIFT_RL	0x04

static volatile unsigned int gpiobase;
static volatile uint32_t *gpio;

static volatile unsigned int timerbase;
static volatile uint32_t *timer;
int fd;
int debug;
static int lcdControl;
struct lcdDataStruct *lcd;

/* struct to hold guesses and secret
 *
 */
typedef struct pegs {
        int colors[PEGS];
} pegs;


/* struct to hold lcd information */

typedef struct lcdDataStruct{
	int bits, rows, cols;
	int rsPin, strbPin;
	int dataPins [8];
	int cx, cy;
} lcdDataStruct;

/* Declaration of function. Implementation made in cw3fun.c */

void setMode (int pin, int mode);

void writePin (int pin, int value);

int readPin(int pin);

volatile uint32_t getTime();

void delay(long milli);

void delayMicroSec(long micro);

void blink(int pin, int j, long delay_milli);

int getButtonInput();

void mmap_setup();

void unmap();

int getInput(); //function that gets users input

struct pegs * getColors(); //function to populate pegs with user choices

int * checkGuess(struct pegs * guess, struct pegs * secret); //function to comp$

void printPegs(struct pegs *p);

void gpio_setup();

// LCD taken from lcd-hello.c

int failure (int fatal, const char *message, ...);

void strobe (const struct lcdDataStruct *lcd);

void sendDataCmd (const struct lcdDataStruct *lcd, unsigned char data);

void lcdPutCommand (const struct lcdDataStruct *lcd, unsigned char command);

void lcdPut4Command (const struct lcdDataStruct *lcd, unsigned char command);

void lcdHome (struct lcdDataStruct *lcd);

void lcdClear (struct lcdDataStruct *lcd);

void lcdPosition (struct lcdDataStruct *lcd, int x, int y);

void lcdDisplay (struct lcdDataStruct *lcd, int state);

void lcdCursor (struct lcdDataStruct *lcd, int state);

void lcdCursorBlink (struct lcdDataStruct *lcd, int state);

void lcdPutchar (struct lcdDataStruct *lcd, unsigned char data);

void lcdPuts (struct lcdDataStruct *lcd, const char *string);

struct lcdDataStruct * getLCD();

//

void lcdWin();

void lcdLose();

void lcdTurn (struct pegs *p, int* answer);

int game();
