#include "cw3.h"

/*Implementations for declared functions in cw3.h*/

/* C implementation of mode set for GPIO pin
 * INPUT = 0, OUTPUT = 1
 * calculates the correct GPSEL Register, and the shift required.
 */

/*
void setMode (int pin, int mode) {

    int fSel    = pin/10;
    int shift   = (pin % 10) * 3;

    if (mode == INPUT)
        *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) ; // Sets bits to zero = input
    else if (mode == OUTPUT)
        *(gpio + fSel) = (*(gpio + fSel) & ~(7 << shift)) | (1 << shift) ;
}
*/

/*ASM implementation of setMode*/
void setMode (int pin, int mode) {

    int fsel  = (pin / 10) * 4;
    int shift = (pin % 10) * 3;

    if(mode == OUTPUT){
        printf("Pin %i, OUT", pin);
        asm(
            "\tLDR R0, %[gpi]\n"
           	 "\tMOV R1, R0\n"
            "\tADD R1, %[fsel]\n"
            "\tLDR R1, [R1]\n"
            "\tMOV R2, #0b111\n"
            "\tLSL R2, %[shift]\n"
            "\tBIC R2, R2\n"
            "\tAND R2, R1\n"
            "\tMOV R1, #1\n"
            "\tLSL R1, %[shift]\n"
            "\tORR R2, R1\n"
            "\tMOV R1, R0\n"
            "\tADD R1, %[fsel]\n"
            "\tSTR R2, [R1]\n"
            :: [gpi] "m" (gpio),
               [fsel] "r" (fsel),
               [shift] "r" (shift)
            : "r0", "r1", "r2", "cc", "memory"
        );
    }
    else
    {
        asm(
            "\tLDR R0, %[gpi]\n"
            	"\tMOV R1, R0\n"
            "\tADD R1, %[fsel]\n"
            "\tLDR R1, [R1]\n"
            "\tMOV R2, #0b111\n"
            "\tLSL R2, %[shift]\n"
            "\tBIC R2, R2\n"
            "\tAND R2, R1\n"
            "\tMOV R1, R0\n"
            "\tADD R1, %[fsel]\n"
            "\tSTR R2, [R1]\n"
            :: [gpi] "m" (gpio),
               [fsel] "r" (fsel),
               [shift] "r" (shift)
            : "r0", "r1", "r2", "cc", "memory"
        );
    }
}

/* Writes to correct GPSET register for HIGH output
 * Writes to correct GPCLR register for LOW output
 */

/*
void writePin (int pin, int value) {
    // if write high = GPSET
    // if write low = GPCLR
    int offset = pin / 32;
    if (value == HIGH)
    {
        offset = offset + 7;    // 7 = GPSET0
        *(gpio + offset) = 1 << (pin & 31) ;
    }
    else
    {
        offset = offset + 10;   // 10 = GPCLR0
        *(gpio + offset) = 1 << (pin & 31) ;
    }
}
*/

/*ASM Implementation of writePin*/
void writePin (int pin, int value) {
    int offset = pin % 32;
    int offset1 = pin / 32;
    if (value == HIGH)
    {
        offset1 = (offset1 + 7) * 4;    // 7 = GPSET0
        asm(
            "\tLDR R0, %[gpi]\n"
            "\tMOV R1, %[offset1]\n"
            "\tMOV R2, %[offset]\n"
            "\tMOV R3, R0\n"
            "\tADD R3, R1\n"
            "\tMOV R4, #1\n"
            "\tLSL R4, R2\n"
            "\tSTR R4, [R3]\n"
            :: [gpi] "m" (gpio),
               [offset] "r" (offset),
               [offset1] "r" (offset1)
            : "r0", "r1", "r2", "r3", "r4", "cc", "memory"
        );
    }
    else
    {
        offset1 = (offset1 + 10) * 4;    // 10 = GPSET0
        asm(
            "\tLDR R0, %[gpi]\n"
            "\tMOV R1, %[offset1]\n"
            "\tMOV R2, %[offset]\n"
            "\tMOV R3, R0\n"
            "\tADD R3, R1\n"
            "\tMOV R4, #1\n"
            "\tLSL R4, R2\n"
            "\tSTR R4, [R3]\n"
            :: [gpi] "m" (gpio),
               [offset] "r" (offset),
               [offset1] "r" (offset1)
            : "r0", "r1", "r2", "r3", "r4", "cc", "memory"
        );
    }
}

/* Returns 0 or 1 depending on input value
 *
 */

/*
int readPin (int pin) {
    int offset = (pin / 32) + 13; // 13 = GPLEV0

    if ((*(gpio + offset) & (1 << (pin & 31))) != 0)
      return 1;
    return 0;
}
*/

/*ASM Implementation of readPin*/
int readPin (int pin) {
    int offset = ((pin / 32) + 13) * 4;
    int pinSet = pin % 32;
    int r;
    asm(
        "\tLDR R0, %[gpi]\n"
        "\tMOV R1, R0\n"
        "\tADD R1, %[offset]\n"
        "\tLDR R1, [R1]\n"
        "\tMOV R2, #1\n"
        "\tLSL R2, %[pinShift]\n"
        "\tAND %[r], R2, R1\n"
        : [r] "=r" (r)
        : [gpi] "m" (gpio),
          [offset] "r" (offset),
          [pinShift] "r" (pinSet)
        : "r0", "r1", "r2", "cc", "memory"
    );

    if (r != 0)
      return 1;
    return 0;
}

volatile uint32_t getTime() {
    return *(timer + 1);
}

void delay(long milli) {
    volatile uint32_t time_now = getTime();
    while((getTime() - time_now) < (milli * 1000)) {}
}

void delayMicroSec(long micro)
{
    volatile uint32_t time_now = getTime();
    while((getTime() - time_now) < (micro)) {}
}

void blink(int pin, int j, long delay_milli) {
    int i;
    for(i=0;i<j;i++)
    {
        writePin(pin, HIGH);
        delay(delay_milli);
        writePin(pin, LOW);
        delay(delay_milli);
    }
}

int getButtonInput() {
    int in = 0;
    uint32_t times = getTime();
    if(debug == 2)
        printf("\tDEBUG: getting input\n");
    while ((getTime() - times) < INPUT_WAIT * 1000) {
        if(readPin(16))
        {
            in++;
            if(debug == 2)
                printf("\tDEBUG: BUTTON PRESSED (%i)\n", in);
            blink(RED_LED, 1, 250);
        }
    }
    return in;
}

void mmap_setup() {
    /* setting up access to rpi memory */
    gpiobase  = 0x3F200000;
    timerbase = 0x3F003000;

    // setting up mmap
    if ((fd = open ("/dev/mem", O_RDWR | O_SYNC | O_CLOEXEC) ) < 0) {
      printf("setup: Unable to open /dev/mem: %s\n", strerror (errno));
      exit(1);
    }

    // GPIO:
    gpio = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, gpiobase);
    if ((int32_t)gpio == -1) {
      printf("setup: mmap (GPIO) failed: %s\n", strerror (errno));
      exit(1);
    }

    // TIMER:
    timer = (uint32_t *)mmap(0, BLOCK_SIZE, PROT_READ|PROT_WRITE, MAP_SHARED, fd, timerbase);
    if ((int32_t)timer == -1) {
      printf("setup: mmap (TIMER) failed: %s\n", strerror (errno));
      exit(1);
    }
}

void unmap() {
    if(munmap((void *)gpio, BLOCK_SIZE) == -1)
        printf("error munmap gpio\n");

    if(munmap((void *)timer, BLOCK_SIZE) == -1)
        printf("error munmap timer\n");

    close(fd);
}

/* Input must be between 1 and 3
 * Returns int
 *
 */

int getInput() {
    int input = getButtonInput();

    while(input > COLOURS || input < 1)       // checks if input is valid
    {
        if(debug == 2)
            printf("DEBUG: input rejected (%i)\n", input);
        writePin(RED_LED, HIGH);
        writePin(YELLOW_LED, HIGH);
        delay(2000);
        writePin(RED_LED, LOW);
        writePin(YELLOW_LED, LOW);
        input = getButtonInput();
    }
    if(debug == 2)
        printf("DEBUG: Input Accepted (%i)\n", input);

    blink(RED_LED, 1, 500);
    blink(YELLOW_LED, input, 300);

    return input;
}


/* Function to populate a pegs struct with user choices
 * Used for guesses/secret
 * return a struct pegs *
 */

struct pegs * getColors() {
    struct pegs *c = malloc(sizeof(struct pegs));       // malloc pegs
    int i;
    for(i = 0; i < PEGS; i++)                           // for each peg
    {
        // for all pegs, get user input.
        int input = getInput();                         // get user input
        c->colors[i] = input;                           // store it in the pegs struct
    }

    return c;
}

/* Function to compare the guess a player has made agaianst the secret combinat$
 * takes 2 struct pegs *, first for guess, second for secret
 * return number of correct and half correct pegs in int[2]
 *
 */

int * checkGuess(struct pegs * guess, struct pegs * secret) {
    // accuracy[0] holds number of fully correct guesses.
    // accuracy[1] holds number of half correct guesses.
    int *accuracy = malloc(2 * sizeof(int));
    accuracy[0] = 0;
    accuracy[1] = 0;

    int guess_checker[PEGS];                // tracks pegs in guess accounted for already
    int secret_checker[PEGS];               // tracks pegs in secret accounted for

    // check for fully correct pegs.
    int i, j;
    for(i=0;i<PEGS;i++)                             // iterate over guess AND secret
    {
        guess_checker[i] = 0;                       // initialise
        secret_checker[i] = 0;                      
        if(secret->colors[i] == guess->colors[i])   // if color & position fully correct
        {
            accuracy[0]++;                          // increment the value for fully correct
            guess_checker[i] = 1;                   // set this peg to accounted for in guess
            secret_checker[i] = 1;                  // set this peg to accounted for in secret
        }
    }

    //check not fully correct pegs, but right color.

    for(i=0;i<PEGS;i++) {			//iterate over secret
        if(secret_checker[i] != 1) {         // skip this pegs already accounted for.
            for(j=0;j<PEGS;j++){                 // iterate over guess
						//If guess is not accounted for and color matches
                if(guess_checker[j] != 1 && secret->colors[i] == guess->colors[j]){
                    accuracy[1]++; // increment half correct counter.
                    secret_checker[i] = 1;
                    guess_checker[j] = 1;
                }
            }
        }
    }

    return accuracy;
}

void printPegs(struct pegs *p) {
    int i;
    for(i=0;i<PEGS;i++)
        printf("%d ", p->colors[i]);
    printf("\n");
}

void gpio_setup() {
    setMode(RED_LED, OUTPUT);
    setMode(YELLOW_LED, OUTPUT);
    setMode(BUTTON, INPUT);

    if(debug == 2)
    {
        printf("DEBUG: Checking GPIO setup.\n");
        writePin(RED_LED, HIGH);
        writePin(YELLOW_LED, HIGH);
        while(1)
        {
            char c;
            printf("DEBUG: Please confirm BOTH LEDs are lit (Y/N): ");
            scanf("%s", &c);

            if(c == 'Y' || c == 'y')
            {
                writePin(RED_LED, LOW);
                writePin(YELLOW_LED, LOW);
                break;
            }
            else if(c == 'N' || c == 'n')
            {
                writePin(RED_LED, LOW);
                writePin(YELLOW_LED, LOW);
                printf("DEBUG: error. Please check your wiring, or update the RED_LED and YELLOW_LED GPIO values defined in the attached header file.\n");
                exit(1);
            }
        }

        printf("DEBUG: Press the button within the next 5s to check its functionality.\n");
        int b = 1;
        uint32_t t = getTime();
        while ((getTime() - t) < 5000000) {
            if(readPin(BUTTON))
            {
                b = 0;
                printf("DEBUG:Button Pressed.\n");
                break;
            }
        }

        if(b)
        {
            printf("DEBUG: error Button failed. Please check wiring or update the BUTTON GPIO value defined at the top of the src file.\n");
            exit(0);
        }

        printf("\n\nDEBUG: GPIO Setup Sucessful... starting game\n");
    }
}


/* ------ LCD taken from lcdhello.c ------ */


int failure (int fatal, const char *message, ...)
{
  va_list argp ;
  char buffer [1024] ;

  if (!fatal) //  && wiringPiReturnCodes)
    return -1 ;

  va_start (argp, message) ;
  vsnprintf (buffer, 1023, message, argp) ;
  va_end (argp) ;

  fprintf (stderr, "%s", buffer) ;
  exit (EXIT_FAILURE) ;

  return 0 ;
}

void strobe (const struct lcdDataStruct *lcd)
{

  // Note timing changes for new version of delayMicroSec ()
  digitalWrite (gpio, lcd->strbPin, 1) ; delayMicroSec (50) ;
  digitalWrite (gpio, lcd->strbPin, 0) ; delayMicroSec (50) ;
}

void sendDataCmd (const struct lcdDataStruct *lcd, unsigned char data)
{
  register unsigned char myData = data ;
  unsigned char          i, d4 ;

  if (lcd->bits == 4)
  {
    d4 = (myData >> 4) & 0x0F;
    for (i = 0 ; i < 4 ; ++i)
    {
      digitalWrite (gpio, lcd->dataPins [i], (d4 & 1)) ;
      d4 >>= 1 ;
    }
    strobe (lcd) ;

    d4 = myData & 0x0F ;
    for (i = 0 ; i < 4 ; ++i)
    {
      digitalWrite (gpio, lcd->dataPins [i], (d4 & 1)) ;
      d4 >>= 1 ;
    }
  }
  else
  {
    for (i = 0 ; i < 8 ; ++i)
    {
      digitalWrite (gpio, lcd->dataPins [i], (myData & 1)) ;
      myData >>= 1 ;
    }
  }
  strobe (lcd) ;
}

void lcdPutCommand (const struct lcdDataStruct *lcd, unsigned char command)
{
#ifdef DEBUG
  fprintf(stderr, "lcdPutCommand: digitalWrite(%d,%d) and sendDataCmd(%d,%d)\n", lcd->rsPin,   0, lcd, command);
#endif
  digitalWrite (gpio, lcd->rsPin,   0) ;
  sendDataCmd  (lcd, command) ;
  delay (2) ;
}

void lcdPut4Command (const struct lcdDataStruct *lcd, unsigned char command)
{
  register unsigned char myCommand = command ;
  register unsigned char i ;

  digitalWrite (gpio, lcd->rsPin,   0) ;

  for (i = 0 ; i < 4 ; ++i)
  {
    digitalWrite (gpio, lcd->dataPins [i], (myCommand & 1)) ;
    myCommand >>= 1 ;
  }
  strobe (lcd) ;
}

void lcdHome (struct lcdDataStruct *lcd)
{
#ifdef DEBUG
  fprintf(stderr, "lcdHome: lcdPutCommand(%d,%d)\n", lcd, LCD_HOME);
#endif
  lcdPutCommand (lcd, LCD_HOME) ;
  lcd->cx = lcd->cy = 0 ;
  delay (5) ;
}

void lcdClear (struct lcdDataStruct *lcd)
{
#ifdef DEBUG
  fprintf(stderr, "lcdClear: lcdPutCommand(%d,%d) and lcdPutCommand(%d,%d)\n", lcd, LCD_CLEAR, lcd, LCD_HOME);
#endif
  lcdPutCommand (lcd, LCD_CLEAR) ;
  lcdPutCommand (lcd, LCD_HOME) ;
  lcd->cx = lcd->cy = 0 ;
  delay (5) ;
}

void lcdPosition (struct lcdDataStruct *lcd, int x, int y) {
  // struct lcdDataStruct *lcd = lcds [fd] ;

  if ((x > lcd->cols) || (x < 0))
    return ;
  if ((y > lcd->rows) || (y < 0))
    return ;

  lcdPutCommand (lcd, x + (LCD_DGRAM | (y>0 ? 0x40 : 0x00)  /* rowOff [y] */  )) ;

  lcd->cx = x ;
  lcd->cy = y ;
}

void lcdDisplay (struct lcdDataStruct *lcd, int state)
{
  if (state)
    lcdControl |=  LCD_DISPLAY_CTRL ;
  else
    lcdControl &= ~LCD_DISPLAY_CTRL ;

  lcdPutCommand (lcd, LCD_CTRL | lcdControl) ;
}

void lcdCursor (struct lcdDataStruct *lcd, int state)
{
  if (state)
    lcdControl |=  LCD_CURSOR_CTRL ;
  else
    lcdControl &= ~LCD_CURSOR_CTRL ;

  lcdPutCommand (lcd, LCD_CTRL | lcdControl) ;
}

void lcdCursorBlink (struct lcdDataStruct *lcd, int state)
{
  if (state)
    lcdControl |=  LCD_BLINK_CTRL ;
  else
    lcdControl &= ~LCD_BLINK_CTRL ;

  lcdPutCommand (lcd, LCD_CTRL | lcdControl) ;
}

void lcdPutchar (struct lcdDataStruct *lcd, unsigned char data)
{
  digitalWrite (gpio, lcd->rsPin, 1) ;
  sendDataCmd  (lcd, data) ;

  if (++lcd->cx == lcd->cols)
  {
    lcd->cx = 0 ;
    if (++lcd->cy == lcd->rows)
      lcd->cy = 0 ;

    lcdPutCommand (lcd, lcd->cx + (LCD_DGRAM | (lcd->cy>0 ? 0x40 : 0x00)   /* rowOff [lcd->cy] */  )) ;
  }
}

void lcdPuts (struct lcdDataStruct *lcd, const char *string)
{
  while (*string)
    lcdPutchar (lcd, *string++) ;
}

struct lcdDataStruct * getLCD() {

    int i ;
    struct lcdDataStruct *lcd = (struct lcdDataStruct *)malloc (sizeof (struct lcdDataStruct));
    int bits, rows, cols ;
    unsigned char func ;

    // hard-coded: 16x2 display, using a 4-bit connection
    bits = 4;
    cols = 16;
    rows = 2;

    // hard-wired GPIO pins
    lcd->rsPin   = LCD_RS ;
    lcd->strbPin = LCD_EN ;
    lcd->bits    = 4 ;
    lcd->rows    = rows ;  // # of rows on the display
    lcd->cols    = cols ;  // # of cols on the display
    lcd->cx      = 0 ;     // x-pos of cursor
    lcd->cy      = 0 ;     // y-pos of curosr

    lcd->dataPins [0] = LCD_DAT4 ;
    lcd->dataPins [1] = LCD_DAT5 ;
    lcd->dataPins [2] = LCD_DAT6 ;
    lcd->dataPins [3] = LCD_DAT7 ;

    digitalWrite (gpio, lcd->rsPin,   0) ; pinMode (gpio, lcd->rsPin,   OUTPUT) ;
    digitalWrite (gpio, lcd->strbPin, 0) ; pinMode (gpio, lcd->strbPin, OUTPUT) ;

    for (i = 0 ; i < bits ; ++i)
    {
      digitalWrite (gpio, lcd->dataPins [i], 0) ;
      pinMode      (gpio, lcd->dataPins [i], OUTPUT) ;
    }
    delay (35) ; // mS

    func = LCD_FUNC | LCD_FUNC_DL ;			// Set 8-bit mode 3 times
    lcdPut4Command (lcd, func >> 4) ; delay (35) ;
    lcdPut4Command (lcd, func >> 4) ; delay (35) ;
    lcdPut4Command (lcd, func >> 4) ; delay (35) ;
    func = LCD_FUNC ;					// 4th set: 4-bit mode
    lcdPut4Command (lcd, func >> 4) ; delay (35) ;
    lcd->bits = 4 ;

    func |= LCD_FUNC_N ;
    lcdPutCommand (lcd, func) ; delay (35) ;


    // Rest of the initialisation sequence
    lcdDisplay     (lcd, TRUE) ;
    lcdCursor      (lcd, FALSE) ;
    lcdCursorBlink (lcd, FALSE) ;
    lcdClear       (lcd) ;

    lcdPutCommand (lcd, LCD_ENTRY   | LCD_ENTRY_ID) ;    // set entry mode to increment address counter after write
    lcdPutCommand (lcd, LCD_CDSHIFT | LCD_CDSHIFT_RL) ;  // set display shift to right-to-left

    return lcd;

}

/*---- Above is taken from lcd-hello.c ----*/


void lcdWin() {
    lcdClear(lcd);
    lcdHome(lcd);
    lcdPuts(lcd, " You Win! :3 ");
}

void lcdLose() {
    lcdClear(lcd);
    lcdHome(lcd);
    lcdPuts(lcd, " You Lost! :( ");

}

void lcdTurn(struct pegs *p, int* answer) {

    lcdClear(lcd);
    lcdHome(lcd);
    lcdPuts(lcd, "G: ");
    int i;
    for(i=0;i<PEGS;i++)
    {
        char c = p->colors[i] + '0';
        lcdPutchar(lcd, c);
        lcdPutchar(lcd, ' ');
    }

    lcdPosition(lcd, 0 , 1);
    lcdPuts(lcd, "A: CP-");
    lcdPutchar(lcd, (answer[0] + '0'));
    lcdPuts(lcd, " CC-");
    lcdPutchar(lcd, (answer[1] + '0'));
}


int game() {

    struct pegs *secret = getColors();
    struct pegs *guess;
    int *answer = malloc(2 * sizeof(int));
    if(debug)
    {
        printf("SECRET: ");
        printPegs(secret);
        printf("==============\n");
    }
    int i;

    for(i=0; i<TURNS; i++) {

        // start round indicator
        blink(RED_LED, 3, 400);
        delay(200);

        guess = getColors();
        blink(RED_LED, 2, 250);               // end of input

        answer = checkGuess(guess, secret);   //compare answers

        lcdTurn(guess, answer);
        if(debug)
        {
            printf("Guess %i: ", i+1);
            printPegs(guess);
            printf("Answer %i:\n\t\tFull Correct: %d \n\t\tHalf Correct: %d\n\n", i+1, answer[0], answer[1]);  //print points
        }

        // display answers
        // TODO ON SCREEN

        blink(YELLOW_LED, answer[0], 300);
        delay(200);
        blink(RED_LED, 1, 500);
        delay(200);
        blink(YELLOW_LED, answer[1], 300);
        delay(200);

        if(answer[0] == PEGS) return 1;
    }
    return 0;
}
