// ** HEADER FILES ** //

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdbool.h>
#include <math.h>

// ** GLOBAL VARIABLES ** //

char **gameboard;
int size_of_board, symbol_num, num_removed, time_elapsed;
int i, j;

// ** PROTOTYPES ** //

void reset_color();
void get_parameters();
void allocate_memory();
bool row_seq(int row, int col);
bool col_seq(int row, int col);
void create_board();
void print_board();
bool check_range(int x1, int y1, int x2, int y2);
bool check_adjacent(int x1, int y1, int x2, int y2);
void remove_crosses();
void remove_rows();
void remove_cols();
void move_down();
void swap(int x1, int y1, int x2, int y2);
void save_board();
void load_file();
void free_gameboard();

// ** MAIN FUNCTION ** //

int main(void)
{
start:
    // ** START-UP PHASE ** //
    // Welcome Message (\033[1;34m colors message blue
    printf("\033[1;34m*** Welcome To Cascade! ***\n");
    printf("\033[1;34mWould You Like To Load A Previous Game? (y/n)\n");
    reset_color(); // Call function to reset color back to default after each prompt/message

    srand(time(NULL)); // Seed random numbers to time

    char saveprompt;
    scanf(" %c", &saveprompt);
    // Create a bool that, while true, asks for the save prompts
    bool load = true;
    while (load)
    {
        if (saveprompt == 'y')
        {
            load_file();
            print_board();
            break;
        }

        else if (saveprompt == 'n')
        {
            printf("\033[1;36mThen, Please Choose Board Settings\n");
            reset_color();
            get_parameters();
            if (size_of_board <= 3 || symbol_num <= 2 || symbol_num > 6) // Validity check to make sure board is playable
            {
                printf("\033[0;31mYou Have Entered An Invalid Value! Enter Valid Parameters Please\n");
                reset_color();
                goto start; // If an invalid value was entered, go to label 'retry' to re-enter proper variables
            }
            allocate_memory();
            create_board();
            print_board();
            break;
        }

        else if (saveprompt != 'n' || saveprompt != 'y')
        {
            printf("\033[0;31mInvalid Inputs - Please Try Again...\n");
            reset_color();
            goto start;
        }
    }

    // ** GAMEPLAY PHASE ** //
    int starttime = time(NULL);
    bool playing = true;
    int x1, y1, x2, y2; // x = row, y = column
    char command;

    // Give command instructions
    printf("\n");
    printf("\033[1;34mTo Swap Symbols, Enter: s x1 y1 x2 y2\n");
    printf("To Quit/Save The Game, Enter: q\n");
    printf("Create as many matches >3 as you can~ Time is against you!!!\n");

playing:
    printf("\033[1;36mEnter A Command: \n");
    reset_color();
    while (playing)
    {
        // While in gameplay phase, await user input
        scanf(" %c", &command);

        // When is entered, call the swap function
        if (command == 's')
        {
            scanf("%d %d %d %d", &x1, &y1, &x2, &y2);
            swap(x1, y1, x2, y2);
            printf("\033[1;36mCurrent Score: %d\n", num_removed * 10);
            printf("\033[1;36mEnter A Command: \n");
            reset_color();
        }

        // **TERMINATION PHASE** //
        // When q is entered commence termination phase
        else if (command == 'q')
        {
            printf("\033[0;31mPress 'y' if you would like to quit. Press any other key otherwise.\n");
            reset_color();

            char command2;
            scanf(" %c", &command2);
            if (command2 == 'y')
            {
                int endtime = time(NULL);
                int score = 0;
                if (saveprompt == 'y')
                {
                    score = 10 * ((num_removed * 10) / (time_elapsed + endtime - starttime));
                    time_elapsed = time_elapsed + endtime - starttime;
                }
                else if (saveprompt == 'n')
                {
                    time_elapsed = endtime - starttime;
                    score = 10 * ((num_removed * 10) / (time_elapsed));
                }

                printf("\033[0;32mCongratz! Here is your final score, scaled against time! \n");
                printf("\033[0;36m%d Points, In %d Seconds\n", score, time_elapsed);

                printf("\033[0;32mPress 'y' if you would like to save the game. Press any other key otherwise\n");
                reset_color();
                char savegame;
                scanf(" %c", &savegame);

                if (savegame == 'y')
                    save_board();

                // Since gameplay has finished and the board has been saved, memory can now be freed safely
                free_gameboard();

                printf("\033[0;32mPress y if you want to play again. Press any other key otherwise\n");
                reset_color();
                char playagain;
                scanf(" %c", &playagain);
                if (playagain == 'y')
                    goto start;
                else
                    break;
            }

            else
                goto playing;
        }
        else
        {
            printf("\033[1;36mInvalid Command. Try Again\n");
            reset_color();
        }
    }

    // Game has ended, and program can be exited
    printf("\033[0;34mThanks For Playing!\n");
    reset_color();
    exit(0);
}

// ** FUNCTIONS ** //

const char symbols[] = {"#@?&!$"}; // Available symbols

void reset_color()
{
    // Function To Reset The Color Of Text
    printf("\033[0m");
}

void get_parameters()
{
    printf("\033[1;36mEnter Gameboard Size (Value must be >3): \n");
    reset_color();
    scanf("%d", &size_of_board); // Store board size
    printf("\033[1;36mEnter Number Of Symbols (Value must be between 3 and 6): \n");
    reset_color();
    scanf("%d", &symbol_num); // Store symbol number
}

void allocate_memory()
{
    gameboard = (char **)malloc((size_of_board) * sizeof(char *)); // Allocate memory for the board
    if (gameboard == NULL)
    {
        // If memory could not be allocated successfully, warn user and exit program
        printf("\033[0;31mNot Enough Memory!\n");
        reset_color();
        exit(0);
    }

    else
    {
        for (i = 0; i < size_of_board; i++)
        {
            gameboard[i] = (char *)malloc((size_of_board) * sizeof(char)); // Allocate memory for rows
            if (gameboard[i] == NULL)
            {
                break;
            }
        }
    }
}

bool row_seq(int row, int col)
{
    // For matches of 3 in the board's rows
    return (col >= 2) && gameboard[row][col] == gameboard[row][col - 1] && gameboard[row][col] == gameboard[row][col - 2];
}

bool col_seq(int row, int col)
{
    // For matches of 3 in the board's columns
    return (row >= 2) && gameboard[row][col] == gameboard[row - 1][col] && gameboard[row][col] == gameboard[row - 2][col];
}

void create_board()
{
    for (i = 0; i < size_of_board; i++)
    {
        for (j = 0; j < size_of_board; j++)
        {
            int rand_index = rand() % symbol_num;
            gameboard[i][j] = symbols[rand_index]; // Populate gameboard with symbols
            while (row_seq(i, j) || col_seq(i, j)) // While theres matches of 3
            {
                // Replace matching symbols with new symbols until board is match-free
                gameboard[i][j] = symbols[rand() % symbol_num];
            }
        }
    }
}

void print_board()
{
    printf("  ");
    for (i = 0; i < size_of_board; i++) // Create column numbers at the top
    {
        printf("\033[1;32m");
        // When double digits are reached, adjust number spacing accordingly
        if (size_of_board >= 10)
        {
            if (i < 9)
            {
                printf(" %d ", i + 1);
            }
            else
            {
                printf("%d ", i + 1);
            }
        }
        else
        {
            printf("%d  ", i + 1);
        }
    }
    printf("\n");

    for (i = 0; i < size_of_board; i++)
    {
        if (size_of_board >= 10 && i < 9)
        {
            printf("%d  ", i + 1);
        }
        else
        {
            printf("%d ", i + 1);
        }

        for (j = 0; j < size_of_board; j++)
        {
            // Print out the gameboard's characters which were generate in create_board() function
            printf("\033[0;37m");
            printf("%c  ", gameboard[i][j]);
            printf("\033[1;32m");
        }
        printf("\n");
    }
    reset_color();
}

bool check_range(int x1, int y1, int x2, int y2)
{
    // Check the range of the coordinates is within parameters of board
    return (x1 >= 1 && x1 <= size_of_board && y1 >= 1 && y1 <= size_of_board) && (x2 >= 1 && x2 <= size_of_board && y2 >= 1 && y2 <= size_of_board);
}

bool check_adjacent(int x1, int y1, int x2, int y2)
{
    // Check that symbols are next to each other
    return ((x1 == x2 && abs(y1 - y2) == 1) || (y1 == y2 && abs(x1 - x2) == 1));
}

void remove_crosses()
{
    for (i = 0; i < size_of_board; i++)
    {
        for (j = 0; j < size_of_board; j++)
        {
            // If there is a horizontal AND vertical set of three (making a cross or L shape)
            if (row_seq(i, j) && col_seq(i, j))
            {
                char symbol = gameboard[i][j];
                gameboard[i - 1][j] = ' ';
                gameboard[i - 2][j] = ' ';
                num_removed += 2;
                // Only increment by two for this sequence or an extra symbol will be counted as removed

                for (int x = j + 1; x < size_of_board; x++) // For further characters right
                {
                    if (gameboard[i][x] == symbol)
                    {
                        gameboard[i][x] = ' ';
                        num_removed++;
                    }
                    else
                    {
                        break;
                    }
                }

                for (int x = j - 3; x >= 0; x--) // For further characters left
                {
                    if (gameboard[i][x] == symbol)
                    {
                        gameboard[i][x] = ' ';
                        num_removed++;
                    }
                    else
                    {
                        break;
                    }
                }

                gameboard[i][j - 1] = ' ';
                gameboard[i][j - 2] = ' ';
                num_removed += 3;

                for (int z = j + 1; z < size_of_board; z++) // for further characters right
                {
                    if (gameboard[z][j] == symbol)
                    {
                        gameboard[z][j] = ' ';
                        num_removed++;
                    }
                    else
                    {
                        break;
                    }
                }

                for (int z = j - 3; z >= 0; z--) // for further characters left
                {
                    if (gameboard[z][j] == symbol)
                    {
                        gameboard[z][j] = ' ';
                        num_removed++;
                    }
                    else
                    {
                        break;
                    }
                }
                // Remove the original swapped symbol last, so that the row_seq/col_seq is not broken
                gameboard[i][j] = ' ';
            }
        }
    }
}

void remove_rows()
{
    for (i = 0; i < size_of_board; i++)
    {
        for (j = 0; j < size_of_board; j++)
        {
            // If there is a horizontal set of three, replace that symbol with a blank ' ' space
            if (row_seq(i, j))
            {
                char symbol = gameboard[i][j];
                gameboard[i][j] = ' ';
                gameboard[i][j - 1] = ' ';
                gameboard[i][j - 2] = ' ';
                num_removed += 3;

                for (int x = j + 1; x < size_of_board; x++) // For further characters right
                {
                    if (gameboard[i][x] == symbol)
                    {
                        gameboard[i][x] = ' ';
                        num_removed++;
                    }
                    else
                    {
                        break;
                    }
                }

                for (int x = j - 3; x >= 0; x--) // For further characters left
                {
                    if (gameboard[i][x] == symbol)
                    {
                        gameboard[i][x] = ' ';
                        num_removed++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}

void remove_cols()
{
    for (i = 0; i < size_of_board; i++)
    {
        for (j = 0; j < size_of_board; j++)
        {
            // If there is a vertical set of 3, replace that symbol with a blank ' ' space
            if (col_seq(i, j))
            {
                char symbol = gameboard[i][j];
                gameboard[i][j] = ' ';
                gameboard[i - 1][j] = ' ';
                gameboard[i - 2][j] = ' ';
                num_removed += 3;

                for (int x = i + 1; x < size_of_board; x++)
                {
                    // For further matches of the same characters going up
                    if (gameboard[x][j] == symbol)
                    {
                        gameboard[x][j] = ' ';
                        num_removed++;
                    }
                    else
                    {
                        break;
                    }
                }

                for (int x = i - 3; x >= 0; x--)
                {
                    // For further characters down
                    if (gameboard[x][j] == symbol)
                    {
                        gameboard[x][j] = ' ';
                        num_removed++;
                    }
                    else
                    {
                        break;
                    }
                }
            }
        }
    }
}

void move_down()
{
    for (int x = 0; x < size_of_board; x++)
    {
        for (i = 0; i < size_of_board - 1; i++)
        {
            for (j = 0; j < size_of_board; j++)
            {
                // If specific place is empty (because of a remove_rows or remove_cols function)
                if (gameboard[i + 1][j] == ' ')
                {
                    gameboard[i + 1][j] = gameboard[i][j];
                    gameboard[i][j] = ' '; // Cascade symbols down
                }
            }
        }
    }

    // Check through gameboard for blank spaces
    for (i = 0; i < size_of_board; i++)
    {
        for (j = 0; j < size_of_board; j++)
        {
            int rand_index = rand() % symbol_num;
            if (gameboard[i][j] == ' ') // Replace numbers that cascaded down from the top
            {
                gameboard[i][j] = symbols[rand_index];
            }
            while (row_seq(i, j) || col_seq(i, j)) // While theres new matches of 3 created
            {
                num_removed += 3; // Increment score by for each match 3 identified
                // Replace matching characters with new ones until board is match-free
                gameboard[i][j] = symbols[rand() % symbol_num];
            }
        }
    }
}

void swap(int x1, int y1, int x2, int y2)
{
    // First check to ensure the entered coordinates are valid
    if (check_range(x1, y1, x2, y2) && check_adjacent(x1, y1, x2, y2))
    {
        // If they are, swap the two specified symbols
        int temp = gameboard[x1 - 1][y1 - 1];
        gameboard[x1 - 1][y1 - 1] = gameboard[x2 - 1][y2 - 1];
        gameboard[x2 - 1][y2 - 1] = temp;

        // Create a variable that stores the number of symbols removed BEFORE the match removing algorithm occurs
        int originalnum = num_removed;

        // Remove columns, rows, and cascade down values until board is match-3 free
        remove_crosses();
        remove_cols();
        remove_rows();
        move_down();

        // The number of removed symbols has increased, this means that a swap that results in a match has occurred
        if (originalnum < num_removed)
        {
            print_board();
            printf("\033[0;32mSwapped Row %d, Column %d with Row %d, Column %d\n", x1, y1, x2, y2);
            reset_color();
        }

        // If the number of removed symbols has stayed the same, a valid swap hasn't occured
        else if (originalnum == num_removed)
        {
            // Swap back the two symbols as the swap was unsuccessful
            temp = gameboard[x2 - 1][y2 - 1];
            gameboard[x2 - 1][y2 - 1] = gameboard[x1 - 1][y1 - 1];
            gameboard[x1 - 1][y1 - 1] = temp;
            printf("\033[0;31mInvalid Swap! No Matches Are Created... Try Again!\n");
            reset_color();
        }
    }

    // If entered coordinates are invalid, warn user
    else
    {
        printf("\033[0;31mInvalid Swap! These Symbols Are Too Far Away... Try Again!\n");
        reset_color();
    }
}

void save_board()
{
    FILE *saveinput = NULL;
    char filename[30];

invalid_save:
    printf("\033[1;36mEnter Save Name\n");
    reset_color();
    // Use 'filename' directly without an '&' as filename is an array that can be evaluated as a pointer

    int bad_name = 0;
    scanf("%29s", filename);

    while (1)
    {
        if (getchar() == '\n')
        {
            break;
        }
        bad_name = 1;
    }
    if (bad_name)
    {
        printf("\033[0;31mThis name is invalid!\nMake sure there are no spaces and you entered less than 30 characters!\nTry again...\n");
        reset_color();
        goto invalid_save;
    }

    // Open file for writing
    saveinput = fopen(filename, "w");

    if (saveinput == NULL)
    {
        printf("\033[0;31mFailed to Create File\n");
        reset_color();
        goto invalid_save; // If an invalid file name was entered, re-prompt user
    }

    // Write in important variables
    fprintf(saveinput, "%d %d %d %d ", num_removed, symbol_num, size_of_board, time_elapsed);

    // Write in the board's current configuration
    for (i = 0; i <= size_of_board - 1; i++)
    {
        for (j = 0; j <= size_of_board - 1; j++)
        {
            fprintf(saveinput, " %c", gameboard[i][j]);
        }
    }

    // Close file
    fclose(saveinput);
}

void load_file()
{

    FILE *readfrom = NULL;
    char savename[30];

invalid_load:
    printf("\033[1;36mEnter Save File's Name\n");
    reset_color();

    int bad_name = 0;
    scanf("%29s", savename);

    while (1)
    {
        if (getchar() == '\n')
        {
            break;
        }
        bad_name = 1;
    }
    if (bad_name)
    {
        printf("\033[0;31mThis name is invalid!\nMake sure there are no spaces and you entered less than 30 characters!\nTry again...\n");
        reset_color();
        goto invalid_load;
    }

    // Open file for reading
    readfrom = fopen(savename, "r");

    // Read in important variables
    fscanf(readfrom, "%d %d %d %d ", &num_removed, &symbol_num, &size_of_board, &time_elapsed);

    // Allocate memory for the game board
    gameboard = (char **)malloc((size_of_board + 1) * sizeof(char *));
    for (i = 0; i <= size_of_board - 1; i++)
    {
        gameboard[i] = (char *)malloc((size_of_board + 1) * sizeof(char));
        for (j = 0; j <= size_of_board - 1; j++)
        {
            fscanf(readfrom, " %c", &gameboard[i][j]); // Read in the board's symbols
        }
    }
    fclose(readfrom);
}

void free_gameboard()
{
    // For the rows and columns of the gameboard, free allocated memory once not needed anymore
    for (i = 0; i < size_of_board; i++)
    {
        free(gameboard[i]);
    }
    free(gameboard);
}
