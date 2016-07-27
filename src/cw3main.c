#include "cw3.h"


int main(int argc, char const *argv[]) {

    if(argc > 1)
    {
        if(strcmp("-dv", argv[1]) == 0)
        {
            debug = 2;
            printf("RUNNING IN VERBOSE DEBUG MODE.\n");
        }

        if(strcmp("-d", argv[1]) == 0)
        {
            debug = 1;
            printf("RUNNING IN DEBUG MODE.\n");
        }


    }


    mmap_setup();
    gpio_setup();
    lcd = getLCD();

    lcdClear(lcd);
    lcdPuts(lcd, "   MASTERMIND   ");
    delay(500);

    printf("\nBeginning Game\n");

    int g = game();

    if(g)
    {
        printf("You win!\n");
        // display win LED
        writePin(YELLOW_LED, HIGH);
        blink(RED_LED, 3, 300);
        writePin(YELLOW_LED, LOW);

        lcdWin();
    }
    else
    {
        printf("You Lose, Sorry!\n");
        lcdLose();
    }

    delay(5000);
    lcdClear(lcd);

    unmap();
    return 0;
}
