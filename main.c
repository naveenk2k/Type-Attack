// TODO: Map function, 3 lives (i.e 3 chances to miss a word), split code into multiple files (add a makefile),

//NOTE: getch() and wgetch() will refresh the screen before getting a char input from that screen

#include <ncurses.h> // includes stdio.h
#include <unistd.h>
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
    noecho();
    cbreak();
    curs_set(TRUE); // Whether to display the position of cursor or not

    // set up initial windows and get the number of rows and columns
    getmaxyx(stdscr, row, col);
    WINDOW *scoreAndLives = newwin(scoreAndInputSize, col, 0, 0);
    WINDOW *movingWords = newwin(row - (2 * scoreAndInputSize), col, scoreAndInputSize, 0);
    WINDOW *userInput = newwin(scoreAndInputSize, col, row - scoreAndInputSize, 0);

    box(scoreAndLives, 0, 0);
    // draw_borders(movingWords);
    box(userInput, 0, 0);

    // To initialize all the words (can control how many words there are using global variable 'numberOfWords')
    for (int i = 0; i < numberOfWords; i++)
    {
        w[i].y = getRandom(0, row - (2 * scoreAndInputSize) - 1); // last 2 rows will be for user input

        //recalculate y coordinates of the current word until it doesn't overlap with that of other words
        while (checkOverlap(w[i].y, trackY, arraySize(trackY)) == 1)
        {
            w[i].y = getRandom(0, row - (2 * scoreAndInputSize) - 1);
        }
        trackY[i] = w[i].y; //append y coordinates

        w[i].x = getRandom(-100, 0);

        strcpy(w[i].text, allWords[getRandom(0, arraySize(allWords) - 1)]);
        w[i].length = strlen(w[i].text);
        strcpy(wordsOnScreen[i], w[i].text); //keeps track of all moving words
        //speed should change according to time elapsed since game started (so implement clock for that)
        w[i].speed = 2; //getRandom(1, 10); // how many characters it jumps in each frame (so should be around 1 - 10)
    }

    while (playerLives > 0 && numberOfWordsUsed < numberOfWords) // Main Game loop   //change it to loop while player still has lives left
    {
        getmaxyx(stdscr, newRow, newCol);

        if (newRow != row || newCol != col)
        {
            col = newCol;
            row = newRow;

            wresize(scoreAndLives, scoreAndInputSize, newCol);
            wresize(movingWords, newRow - (2 * scoreAndInputSize), newCol);
            wresize(userInput, scoreAndInputSize, newCol);

            mvwin(scoreAndLives, 0, 0);
            mvwin(movingWords, scoreAndInputSize, 0);
            mvwin(userInput, newRow - scoreAndInputSize, 0);

            wclear(stdscr);
            wclear(scoreAndLives);
            wclear(movingWords);
            wclear(userInput);

            box(scoreAndLives, 0, 0);
            // draw_borders(movingWords);
            box(userInput, 0, 0);
        }

        // draw to our windows
        wattron(scoreAndLives, A_STANDOUT);
        mvwprintw(scoreAndLives, 1, 2, "Score: %d", playerScore);
        mvwprintw(scoreAndLives, 1, col - 10, "Lives: %d", playerLives);
        wattroff(scoreAndLives, A_STANDOUT);

        mvwprintw(userInput, 1, 1, "Enter word: ");

        nodelay(userInput, TRUE);
        int ch;

        if ((ch = mvwgetch(userInput, 1, charX)) == ERR) //gets input character by character; try getting string directly
        {
            for (int i = 0; i < numberOfWords; i++) // for each word, do the following
            {
                // 'j' loop to clear the trailing characters that are left behind. We need to go back 'speed' characters and add blanks ' ' there.
                for (int j = 0; j < w[i].speed; j++)
                {
                    mvwaddch(movingWords, w[i].y, w[i].x + j, ' ');
                }

                w[i].x += w[i].speed;

                // if there's enough space to display the whole word, display it; else don't
                if (w[i].x + w[i].length < col)
                {
                    if (stringMatch(w[i].text, wordsOnScreen, 0) == 1) //to check if word is present in wordsOnScreen array; if not present, dont print the word
                        mvwprintw(movingWords, w[i].y, w[i].x, w[i].text);
                    else
                    {
                        strcpy(w[i].text, " ");
                        numberOfWordsUsed++;
                    }
                }
                else
                {
                    if (strcmp(w[i].text, " ") != 0)
                    {
                        playerLives--; //lose a life every time a word reaches right margin
                        numberOfWordsUsed++;
                    }
                    strcpy(w[i].text, " ");

                    wattron(scoreAndLives, A_STANDOUT);
                    mvwprintw(scoreAndLives, 1, col - 10, "Lives: %d", playerLives); //print new player lives left
                    wattroff(scoreAndLives, A_STANDOUT);
                }
                delay_output(25); //changed delay_output to make it smoother
            }
        }
        else
        {
            if (ch != 127)
            {
                enteredWord[charX - 13] = ch;             //concatenating entered characters into one string
                mvwprintw(userInput, 1, charX, "%c", ch); //printing entered characters
                charX++;                                  //incrementing x coordinate to print next character

                wmove(userInput, 1, charX);                          // Try to move the cursor according to the input
                if (stringMatch(enteredWord, wordsOnScreen, 1) == 1) //if word entered by user matches a word moving on the screen (also removes string from array if matched)
                {
                    playerScore++;

                    wattron(scoreAndLives, A_STANDOUT);
                    mvwprintw(scoreAndLives, 1, 2, "Score: %d", playerScore); //update playerscore
                    wattroff(scoreAndLives, A_STANDOUT);

                    for (int i = 0; i < 20; i++)
                    {
                        enteredWord[i] = '\0';
                    }
                    mvwprintw(userInput, 1, 13, "                    "); //prints 20 spaces; to clear input
                    charX = 13;
                }
            }
            else
            {
                if (charX - 1 >= 13) // to avoid moving the cursor back beyond the start of input
                    charX--;
                enteredWord[charX - 13] = '\0';
                mvwprintw(userInput, 1, charX, " ");
            }
        }

        wrefresh(scoreAndLives); // refreshes all words at the same time cause it's outside the 'i'-for loop
        wrefresh(movingWords);
        wrefresh(userInput);
    }

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

int getRandom(int lower, int upper)
{
    return (rand() % (upper - lower + 1)) + lower;
}

//if y is already present in arr, return 1 else 0
int checkOverlap(int y, int *arr, int len)
{
    for (int i = 0; i < len; i++)
    {
        if (arr[i] == y)
        {
            return 1;
        }
    }
    return 0;
}

//to check if a string is present in an array of strings; return 1 if found else 0; if pop = 1, removes matched element from array
int stringMatch(char s[], char arr[][20], int pop)
{
    for (int i = 0; i < numberOfWords; i++)
    {
        if (strcmp(s, arr[i]) == 0)
        {
            if (pop == 1)
                strcpy(arr[i], " ");
            return 1;
        }
    }
    return 0;
}
