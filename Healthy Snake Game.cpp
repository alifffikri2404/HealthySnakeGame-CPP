// Original source code written by ThePoorEngineer
// Improvise by Group 4 SKR3306
/*	Ahmad Baihaqi bin Ahmad Rifae (206906)
    Muhammad Aliff Fikri bin Anuar Hidayat (207532)
    Muhammad Arif Daniel bin Yusoff (208034)
    He Kai (208810)
*/

#include <stdlib.h>
#include <iostream>
#include <time.h>
#include <cstdlib>
#include <windows.h>
#include <process.h>
#include <conio.h>
#include <fstream>
using namespace std;

#define MAX 100
#define WIDTH 77
#define HEIGHT 22
#define INIT_SNAKE_LENGTH 4
#define FOOD 1
#define JFOOD 1

// negative values represent areas that cannot be touched or else game over
#define WALL -2
#define SNAKE -1
#define NOTHING 0

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_LEFT 75
#define KEY_RIGHT 77

#define RIGHT 0
#define UP 1
#define LEFT 2
#define DOWN 3
#define EXIT -1
static int dx[5] = {1, 0, -1, 0};
static int dy[5] = {0, -1, 0, 1};
// direction array, for use with RIGHT, UP, LEFT, DOWN constants
//(1, 0) is RIGHT
//(0, -1) is UP (because the row number increase from top to bottom)
//(-1, 0) is LEFT
//(0, 1) is DOWN

int input = RIGHT; // global variable to take in the user's input
int item = NOTHING;

void gotoxy(int column, int row)
{
    HANDLE hStdOut;
    COORD coord;

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE)
        return;

    coord.X = column;
    coord.Y = row;
    SetConsoleCursorPosition(hStdOut, coord);
}

void clearScreen()
{
    HANDLE hStdOut;
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    DWORD count;
    DWORD cellCount;
    COORD homeCoords = {0, 0};

    hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hStdOut == INVALID_HANDLE_VALUE)
        return;

    /* Get the number of cells and cell attributes in the current buffer */
    if (!GetConsoleScreenBufferInfo(hStdOut, &csbi))
        return;
    cellCount = csbi.dwSize.X * csbi.dwSize.Y;

    /* Fill the entire buffer with spaces */
    if (!FillConsoleOutputCharacter(
            hStdOut,    // handle to console screen buffer
            (TCHAR)' ', // character to write to the buffer
            cellCount,  // number of cells to write to
            homeCoords, // coordinates of first cell
            &count      // receives the number of characters written
            ))
        return;

    /* Fill the entire buffer with the current colors and attributes */
    if (!FillConsoleOutputAttribute(
            hStdOut,          // handle to console screen buffer
            csbi.wAttributes, // Character attributes to use
            cellCount,        // Number of cells to set attribute
            homeCoords,       // Coordinate of first cell
            &count            // receives the number of characters written
            ))
        return;

    /* Move the cursor home */
    SetConsoleCursorPosition(hStdOut, homeCoords);
}

// check if 2 directions are opposite
int oppositeDirection(int input1, int input2)
{
    if (input1 == LEFT && input2 == RIGHT)
        return 1;
    if (input1 == RIGHT && input2 == LEFT)
        return 1;
    if (input1 == UP && input2 == DOWN)
        return 1;
    if (input1 == DOWN && input2 == UP)
        return 1;

    return 0;
}

struct Coordinate
{
    int x, y;
};

class snake
{
private:
    int length;
    Coordinate body[WIDTH * HEIGHT];
    int direction;
    int ground[MAX][MAX];
    int foodCounter;
    int jfoodCounter;

public:
    void initGround();
    void initSnake();
    void updateSnake(int delay);
    void updateFood();
    void updateJFood();
    void firstDraw();
    int getFoodCounter();
    int getJFoodCounter();
};

void snake::initGround()
{
    int i, j;
    for (i = 0; i < MAX; i++)
        for (j = 0; j < MAX; j++)
            ground[i][j] = 0;
    for (i = 0; i <= WIDTH + 1; i++)
    {
        // top bottom wall
        ground[0][i] = WALL;
        ground[HEIGHT + 1][i] = WALL;
    }
    for (i = 0; i <= HEIGHT + 1; i++)
    {
        // right left wall
        ground[i][0] = WALL;
        ground[i][WIDTH + 1] = WALL;
    }
}

void snake::initSnake()
{
    length = INIT_SNAKE_LENGTH; // set head of snake to be at the centre
    body[0].x = WIDTH / 2;
    body[0].y = HEIGHT / 2;
    direction = input;
    foodCounter = 0;
    jfoodCounter = 0;

    int i;
    for (i = 1; i < length; i++)
    {
        body[i].x = body[i - 1].x - dx[direction]; // if moving right,
        body[i].y = body[i - 1].y - dy[direction]; // body is on the left
    }
    // let the ground know the snake's position
    for (i = 0; i < length; i++)
        ground[body[i].y][body[i].x] = SNAKE;
}

void snake::updateSnake(int delay)
{
    int i;
    Coordinate prev[WIDTH * HEIGHT];
    for (i = 0; i < length; i++)
    {
        prev[i].x = body[i].x;
        prev[i].y = body[i].y;
    }

    if (input != EXIT && !oppositeDirection(direction, input))
        direction = input;

    body[0].x = prev[0].x + dx[direction]; // head of snake
    body[0].y = prev[0].y + dy[direction]; // follows the direction

    if (ground[body[0].y][body[0].x] < NOTHING)
    {
        item = -1;
        return;
    }
    if (ground[body[0].y][body[0].x] == JFOOD || ground[body[0].y][body[0].x] == 'J')
    {
        jfoodCounter++;
        length++; // length of snake increases when it eats food
        item = JFOOD;
    }
    if (ground[body[0].y][body[0].x] == FOOD || ground[body[0].y][body[0].x] == 'F')
    {
        foodCounter++;
        length++; // length of snake increases when it eats food
        item = FOOD;
    }
    else
    {
        ground[body[length - 1].y][body[length - 1].x] = NOTHING;
        item = NOTHING;
        gotoxy(body[length - 1].x, body[length - 1].y); // if snake does not get food,
        cout << " ";                                    // erase last part because the snake is moving
    }

    for (i = 1; i < length; i++)
    {
        body[i].x = prev[i - 1].x; // body follows the previous
        body[i].y = prev[i - 1].y; // location that it was from
    }

    gotoxy(body[1].x, body[1].y);
    cout << "+"; // change the previous head to a body
    gotoxy(body[0].x, body[0].y);
    cout << "O"; // add a head to the snake

    // let the ground know the snake's position
    for (i = 0; i < length; i++)
        ground[body[i].y][body[i].x] = SNAKE;

    Sleep(delay);

    return;
}

void snake::updateFood()
{
    int x, y;
    do
    {
        x = rand() % WIDTH + 1;
        y = rand() % HEIGHT + 1;
    } while (ground[y][x] != NOTHING);

    ground[y][x] = FOOD;
    gotoxy(x, y);
    cout << "\u0003";
}
void snake::updateJFood()
{
    int x, y;
    do
    {
        x = rand() % WIDTH + 1;
        y = rand() % HEIGHT + 1;
    } while (ground[y][x] != NOTHING);

    ground[y][x] = JFOOD;
    gotoxy(x, y);
    cout << "X";
}

void snake::firstDraw()
{
    clearScreen();
    int i, j;
    for (i = 0; i <= HEIGHT + 1; i++)
    {
        for (j = 0; j <= WIDTH + 1; j++)
        {
            switch (ground[i][j])
            {
            case NOTHING:
                cout << " ";
                break;
            case WALL:
                if ((i == 0 && j == 0) || (i == 0 && j == WIDTH + 1) || (i == HEIGHT + 1 && j == 0) || (i == HEIGHT + 1 && j == WIDTH + 1))
                    cout << "+"; // the 4 corners
                else if (j == 0 || j == WIDTH + 1)
                    cout << "|"; // left/right wall
                else
                    cout << "-"; // top/bottom wall
                break;
            case SNAKE:
                if (i == body[0].y && j == body[0].x)
                    cout << "O";
                else
                    cout << "+";
                break;
            default:
                cout << "\u0003"; // first food
            }
        }
        cout << endl;
    }
}

int snake::getFoodCounter()
{
    return foodCounter;
}

int snake::getJFoodCounter()
{
    return jfoodCounter;
}

void userInput(void *id)
{
    do
    {
        int c = _getch();
        switch (c)
        {
        case 'W':
        case 'w':
        case KEY_UP:
            input = UP;
            break;
        case 'S':
        case 's':
        case KEY_DOWN:
            input = DOWN;
            break;
        case 'D':
        case 'd':
        case KEY_RIGHT:
            input = RIGHT;
            break;
        case 'A':
        case 'a':
        case KEY_LEFT:
            input = LEFT;
            break;
        case 27:
            input = EXIT;
            break;
        }
    } while (input != EXIT && item >= 0);

    _endthread();
    return;
}

int main()
{
    int delay = 50;
    double score;
    // mainmenu file
    ifstream file_IN("highscore.txt"); // read file
    ofstream file_OUT("score.txt");    // write file
    file_IN >> score;                  // read the input (highest score)
    gotoxy(52, 20);
    gotoxy(WIDTH / 2 - 14, HEIGHT / 2 - 2);
    cout << "Welcome to Healthy Snake Game!\n\n\t\tRules: Eat \u0003 (Fruit) to get more 1 point and \n\t\tif eat X (Junk food) then will deduct 1 point\n\t\t  Do not hit any wall or eat your own tail!";
    gotoxy(WIDTH / 2 - 14, HEIGHT / 2 + 4);
    cout << "Press Enter to start the game";
    gotoxy(WIDTH / 2 - 12, HEIGHT / 2 - 4);
    cout << "The highest score is: " << score;
    int highest;
    highest = score;
    gotoxy(WIDTH / 2 - 12, HEIGHT / 2 + 14);
    cin.get();
    system("cls");
    srand(time(NULL));
    snake nagini;

    nagini.initGround();
    nagini.initSnake();
    nagini.updateFood();
    nagini.updateJFood();
    nagini.firstDraw();
    _beginthread(userInput, 0, (void *)0);

    // if the file doesnt exist
    if (!file_IN)
    {
        cout << "Error opening file. \n";
        cout << "It may not exist where indicated" << endl;
        return 0;
    }

    do
    {
        nagini.updateSnake(delay);
        if (item == FOOD)
            nagini.updateFood();
        if (item == JFOOD)
            nagini.updateJFood();
    } while (item >= 0 && input != EXIT);

    if (score > highest)
        highest = score;
    // display the score
    system("cls");
    gotoxy(WIDTH / 2 - 6, HEIGHT / 2 - 2);
    printf("Game Over!");
    gotoxy(WIDTH / 2 - 14, HEIGHT / 2);
    printf("The highest score is %d", highest);
    gotoxy(WIDTH / 2 - 10, HEIGHT / 2 + 2);
    printf("Your score is: %d", nagini.getFoodCounter() - 4);
    // printf("\n")
    // printf("\nYour Junk score is: %d",nagini.getJFoodCounter() - 1 );
    gotoxy(WIDTH / 2 - 12, HEIGHT / 2 + 14);
    system("pause");

    // display the score in the file
    file_OUT << "+-----------------------------------------------+" << endl;
    file_OUT << "+						+" << endl;
    file_OUT << "+						+" << endl;
    file_OUT << "+		Your score is " << nagini.getFoodCounter() - 1
             << "!		+" << endl;
    file_OUT << "+						+" << endl;
    file_OUT << "+						+" << endl;
    file_OUT << "+	    The highest score is " << score << "		+" << endl;
    file_OUT << "+						+" << endl;
    file_OUT << "+						+" << endl;
    file_OUT << "+-----------------------------------------------+" << endl;

    _getch();
    // close the file
    file_OUT.close();
    file_IN.close();
    return 0;
}
