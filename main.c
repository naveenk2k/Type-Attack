#include <ncurses.h>
#include <stdio.h>
#include <string.h>
#include <time.h>
#include <stdlib.h>

#define numberOfWords 18
#define arraySize(arr) sizeof(arr) / sizeof(arr[0])

int getRandom(int, int);
int checkOverlap(int, int *, int);
int stringMatch(char[], char[][20], int);
//void draw_borders(WINDOW *);

//global variables
char allWords[][20] = {"leash", "race", "competition", "flat", "wife", "door", "establish", "hell", "whip", "last", "young", "owner", "work", "jail", "range", "remedy", "minister", "wash", "draw", "electron", "motif", "marsh", "mass", "qualification", "loop", "screen", "dealer", "folk", "stain", "conspiracy", "wisecrack", "manufacturer", "present", "complete", "legend", "thread", "speed", "hostile", "active", "chemistry", "rabbit", "remain", "wheat", "expectation", "rumor", "information", "consciousness", "art", "brink", "social", "cattle", "mechanical", "veil", "grass", "notorious", "self", "inspector", "accountant", "sin", "hierarchy", "familiar", "vertical", "package", "joystick", "moral", "carbon", "echo", "user", "ground", "arrangement", "carriage", "gossip", "confront", "bulb", "treasurer", "ignorant", "bomb", "content", "fruit", "hammer", "steep", "transparent", "minority", "brick", "presidency", "accurate", "bundle", "restrain", "choice", "mild", "nervous", "partnership", "trace", "image", "peak", "spider", "budge", "knit", "flag", "member", "bubble", "bottle", "peep", "cloudy", "volleyball", "axiomatic", "fresh", "include", "far", "psychedelic", "scary", "free", "attractive", "study", "shape", "features", "moon", "needless", "pocket", "sofa", "far-flung", "zipper", "perpetual", "crooked", "promise", "maddening", "army", "library", "fact", "side", "precious", "dangerous", "gleaming", "substantial", "bait", "cluttered", "ultra", "languid", "look", "nut", "day", "unpack", "alcoholic", "sheep", "great", "demonic", "divide", "pleasure", "thin", "irate", "finger", "glossy", "knotty", "male", "six", "deafening", "defeated", "bitter", "ski", "spotty", "prick", "meeting", "compete", "war", "object", "vessel", "slave", "expert", "purpose", "crown", "juicy", "hair", "boorish", "island", "untidy", "powder", "vulgar", "adorable", "memorise", "pin", "order", "unit", "tail", "open", "yummy", "claim", "scary", "tin", "smoggy", "lake", "push", "soda", "appreciate", "sneeze", "faulty", "terrific", "best", "attraction", "substantial", "supply", "value", "gainful", "bike", "knee", "slope", "ice", "laborer", "probable", "satisfying", "untidy", "innocent", "mouth", "tightfisted", "representative", "hospital", "tiger", "obey", "robin", "flaky", "old-fashioned", "rock", "excuse", "brief", "drown", "sea", "aloof", "face", "women", "hum", "divergent", "envious", "reach"};

char wordsOnScreen[numberOfWords][20]; //to store all words moving on screen

// Structures for every word
struct word
{
    char text[20]; // Actual word
    int y, x;      // The (y, x) coordinates on the ncurses window
    int length;    // Total number of characters
    int speed;
} w[numberOfWords];

void start()
{
    int row, col, newRow, newCol; // of the whole terminal window + in case there's a resize of window
    int scoreAndInputSize = 3;
    int playerScore = 0;
    int playerLives = 3;
    int trackY[numberOfWords]; //to keep track of y coordinates of words in order to prevent overlapping of words
    srand(time(0));            // to seed the random number generator (only once at the start)
    int charX = 13;            //stores position (x coordinate) to take input from userInput window
    char enteredWord[20];      //to store word entered by user
    char username[20];
    char mesg[]="Enter username: ";
    for (int i = 0; i < 20; i++)
    {
        enteredWord[i] = '\0';
    }
    time_t start = time(NULL), end;
    int numberOfWordsUsed = 0;


    initscr();
    clear();
    noecho();
    cbreak(); /* Line buffering disabled. pass on everything */
    startx = (80 - WIDTH) / 2;
    starty = (24 - HEIGHT) / 2;

    start_color();
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(3, COLOR_GREEN, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    init_pair(5, COLOR_CYAN, COLOR_BLACK);
    menu_win = newwin(HEIGHT, WIDTH, starty, startx);
    keypad(menu_win, TRUE);
    //attron(A_BLINK);

    attron(COLOR_PAIR(5));
    T();

    attroff(COLOR_PAIR(5));

    attron(COLOR_PAIR(3));
    A();
    attroff(COLOR_PAIR(3));

    attron(COLOR_PAIR(4));
    Y();
    attroff(COLOR_PAIR(4));

    attron(COLOR_PAIR(2));
    P();
    E();
    attroff(COLOR_PAIR(2));

    attron(COLOR_PAIR(4));
    K();
    attroff(COLOR_PAIR(4));

    //attroff(A_BLINK);

    refresh();
    print_menu(menu_win, highlight);
    while (1)
    {
        c = wgetch(menu_win);
        switch (c)
        {
        case KEY_UP:
            if (highlight == 1)
                highlight = n_choices;
            else
                --highlight;
            break;
        case KEY_DOWN:
            if (highlight == n_choices)
                highlight = 1;
            else
                ++highlight;
            break;
        case 10:
            choice = highlight;
            break;
        default:
            mvprintw(24, 0, "Charcter pressed is = %3d Hopefully it can be printed as '%c'", c, c);
            refresh();
            break;
        }
        print_menu(menu_win, highlight);
        if (choice != 0) /* User did a choice come out of the infinite loop */
            start();
        break;
    }

    return 0;
}

void print_menu(WINDOW *menu_win, int highlight)
{
    int x, y, i;

    x = 2;
    y = 2;
    box(menu_win, 0, 0);
    for (i = 0; i < n_choices; ++i)
    {
        if (highlight == i + 1) /* High light the present choice */
        {
            wattron(menu_win, A_REVERSE);
            mvwprintw(menu_win, y, x, "%s", choices[i]);
            wattroff(menu_win, A_REVERSE);
        }
        else
            mvwprintw(menu_win, y, x, "%s", choices[i]);
        ++y;
    }
    wrefresh(menu_win);
}

void T()
{
    // For first T
    mvprintw(1, 1, "*");
    mvprintw(1, 2, "*");
    mvprintw(1, 3, "*");
    mvprintw(1, 4, "*");
    mvprintw(1, 5, "*");
    mvprintw(2, 5, "*");
    mvprintw(3, 5, "*");
    mvprintw(4, 5, "*");
    mvprintw(5, 5, "*");
    mvprintw(6, 5, "*");
    mvprintw(7, 5, "*");
    mvprintw(1, 6, "*");
    mvprintw(1, 7, "*");
    mvprintw(1, 8, "*");
    mvprintw(1, 9, "*");

    // For 2nd T
    mvprintw(1, 41, "*");
    mvprintw(1, 42, "*");
    mvprintw(1, 43, "*");
    mvprintw(1, 44, "*");
    mvprintw(1, 45, "*");
    mvprintw(2, 45, "*");
    mvprintw(3, 45, "*");
    mvprintw(4, 45, "*");
    mvprintw(5, 45, "*");
    mvprintw(6, 45, "*");
    mvprintw(7, 45, "*");
    mvprintw(1, 46, "*");
    mvprintw(1, 47, "*");
    mvprintw(1, 48, "*");
    mvprintw(1, 49, "*");

    //For 3rd T
    mvprintw(1, 51, "*");
    mvprintw(1, 52, "*");
    mvprintw(1, 53, "*");
    mvprintw(1, 54, "*");
    mvprintw(1, 55, "*");
    mvprintw(2, 55, "*");
    mvprintw(3, 55, "*");
    mvprintw(4, 55, "*");
    mvprintw(5, 55, "*");
    mvprintw(6, 55, "*");
    mvprintw(7, 55, "*");
    mvprintw(1, 56, "*");
    mvprintw(1, 57, "*");
    mvprintw(1, 58, "*");
    mvprintw(1, 59, "*");
}

void Y()
{
    mvprintw(1, 11, "*");
    mvprintw(2, 12, "*");
    mvprintw(3, 13, "*");
    mvprintw(4, 14, "*");
    mvprintw(5, 14, "*");
    mvprintw(6, 14, "*");
    mvprintw(7, 14, "*");
    //mvprintw(7,14, "*");
    mvprintw(3, 15, "*");
    mvprintw(2, 16, "*");
    mvprintw(1, 17, "*");
}

void P()
{
    mvprintw(1, 19, "*");
    mvprintw(1, 20, "*");
    mvprintw(1, 21, "*");
    mvprintw(1, 22, "*");
    mvprintw(1, 23, "*");
    mvprintw(2, 23, "*");
    mvprintw(3, 23, "*");
    mvprintw(4, 23, "*");
    mvprintw(4, 22, "*");
    mvprintw(4, 21, "*");
    mvprintw(4, 20, "*");
    mvprintw(4, 19, "*");
    mvprintw(3, 19, "*");
    mvprintw(2, 19, "*");
    mvprintw(5, 19, "*");
    mvprintw(6, 19, "*");
    mvprintw(7, 19, "*");
}

void E()
{
    int i = 0;
    int j = 25;
    int x = 25;
    int y = 25;
    for (i = 0; i < 8; i++)
    {
        mvprintw(1, j, "*");
        j++;
    }

    for (i = 1; i < 8; i++)
    {
        mvprintw(i, 25, "*");
    }

    for (i = 0; i < 8; i++)
    {
        mvprintw(7, x, "*");
        x++;
    }

    for (i = 0; i < 6; i++)
    {
        mvprintw(4, y, "*");
        y++;
    }
}

void A()
{
    //First A
    int i = 0;
    for (i = 1; i < 8; i++)
    {
        mvprintw(i, 34, "*");
    }

<<<<<<< HEAD
    mvprintw(1, 35, "*");
    mvprintw(1, 36, "*");
    mvprintw(1, 37, "*");
    mvprintw(1, 38, "*");
    mvprintw(1, 39, "*");

    for (i = 2; i < 8; i++)
    {
        mvprintw(i, 39, "*");
    }
    mvprintw(4, 35, "*");
    mvprintw(4, 36, "*");
    mvprintw(4, 37, "*");
    mvprintw(4, 38, "*");

    //Second A
=======
    //Game ends here
    end = time(NULL);
    float min = (float)(end - start) / 60.0;
    float wpm = (float)playerScore / min;
    clear();
    //WINDOW *username = newwin(34,50,3,4);

    //box(username, 0, 0);
    //mvwprintw(username,1,1,"Enter user name: ");


    start_color();
    init_pair(2, COLOR_YELLOW, COLOR_BLACK);
    init_pair(4, COLOR_RED, COLOR_BLACK);
    //attron(COLOR_PAIR(2));
    attron(A_STANDOUT);

    attron(A_STANDOUT | A_BLINK);
    mvprintw((row / 2) - 3, (col / 2) - 4, "GAME OVER"); //game over screen
    attroff(A_STANDOUT | A_BLINK);
    mvprintw((row / 2) - 1, (col / 2) - 5, "WPM = %.2f", wpm);
    mvprintw((row / 2) + 1, (col / 2) - 4, "Score = %d", playerScore);
    mvprintw((row / 2) + 3, (col / 2) - 7, "Lives lost = %d", (playerLives <= 0) ? 3 : 3 - playerLives);
    attroff(A_STANDOUT);
    //attroff(COLOR_PAIR(2));
    getch();
    endwin(); // Restore normal terminal behavior


}
>>>>>>> a60bf97aafe499e0f43275316ea4a89d240eeebd

    for (i = 1; i < 8; i++)
    {
        mvprintw(i, 61, "*");
    }

    mvprintw(1, 62, "*");
    mvprintw(1, 63, "*");
    mvprintw(1, 64, "*");
    mvprintw(1, 65, "*");
    mvprintw(1, 66, "*");

    for (i = 2; i < 8; i++)
    {
        mvprintw(i, 66, "*");
    }
    mvprintw(4, 62, "*");
    mvprintw(4, 63, "*");
    mvprintw(4, 64, "*");
    mvprintw(4, 65, "*");
}

void K()
{
    int i;
    for (i = 1; i < 8; i++)
    {
        mvprintw(i, 69, "*");
    }

    mvprintw(3, 70, "*");
    mvprintw(2, 71, "*");
    mvprintw(1, 72, "*");
    mvprintw(5, 70, "*");
    mvprintw(6, 71, "*");
    mvprintw(7, 72, "*");
}
