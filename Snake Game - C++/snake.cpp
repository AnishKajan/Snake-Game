#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <unistd.h>     // For usleep()
#include <termios.h>    // For keyboard input
#include <fcntl.h>      // For non-blocking I/O

using namespace std;

bool gameOver;
const int width = 20;
const int height = 20;
int x, y, foodX, foodY, score;
enum Direction { STOP = 0, LEFT, RIGHT, UP, DOWN };
Direction dir;
vector<pair<int, int>> tail;
int tailLength = 0;

// Set terminal to non-blocking input
void enableRawMode() {
    termios term;
    tcgetattr(0, &term);
    term.c_lflag &= ~ICANON; // Disable buffering
    term.c_lflag &= ~ECHO;   // Disable echo
    tcsetattr(0, TCSANOW, &term);
}

// Check if a key is pressed
bool kbhit() {
    termios oldt, newt;
    int ch;
    int oldf;
    
    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;
    newt.c_lflag &= ~(ICANON | ECHO);
    tcsetattr(STDIN_FILENO, TCSANOW, &newt);
    oldf = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, oldf | O_NONBLOCK);
    
    ch = getchar();
    
    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);
    fcntl(STDIN_FILENO, F_SETFL, oldf);
    
    if (ch != EOF) {
        ungetc(ch, stdin);
        return true;
    }

    return false;
}

void Setup() {
    gameOver = false;
    dir = STOP;
    x = width / 2;
    y = height / 2;
    foodX = rand() % width;
    foodY = rand() % height;
    score = 0;
    tail.clear();
    tailLength = 0;
}

void Draw() {
    system("clear"); // Use "cls" on Windows
    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << endl;

    for (int i = 0; i < height; i++) {
        for (int j = 0; j < width; j++) {
            if (j == 0)
                cout << "#";

            if (i == y && j == x)
                cout << "O";
            else if (i == foodY && j == foodX)
                cout << "*";
            else {
                bool print = false;
                for (auto t : tail) {
                    if (t.first == j && t.second == i) {
                        cout << "o";
                        print = true;
                        break;
                    }
                }
                if (!print)
                    cout << " ";
            }

            if (j == width - 1)
                cout << "#";
        }
        cout << endl;
    }

    for (int i = 0; i < width + 2; i++)
        cout << "#";
    cout << "\nScore: " << score << endl;
}

void Input() {
    if (kbhit()) {
        char ch = getchar();
        switch (ch) {
            case 'a':
                dir = LEFT;
                break;
            case 'd':
                dir = RIGHT;
                break;
            case 'w':
                dir = UP;
                break;
            case 's':
                dir = DOWN;
                break;
            case 'x':
                gameOver = true;
                break;
        }
    }
}

void Logic() {
    pair<int, int> prev = { x, y };

    if (tailLength > 0)
        tail.insert(tail.begin(), prev);
    if (tail.size() > tailLength)
        tail.pop_back();

    switch (dir) {
        case LEFT:  x--; break;
        case RIGHT: x++; break;
        case UP:    y--; break;
        case DOWN:  y++; break;
        default:    break;
    }

    if (x < 0 || x >= width || y < 0 || y >= height)
        gameOver = true;

    for (auto t : tail)
        if (t.first == x && t.second == y)
            gameOver = true;

    if (x == foodX && y == foodY) {
        score += 10;
        foodX = rand() % width;
        foodY = rand() % height;
        tailLength++;
    }
}

int main() {
    srand(time(0));
    enableRawMode();
    Setup();

    while (!gameOver) {
        Draw();
        Input();
        Logic();
        usleep(100000); // 100 ms
    }

    cout << "\nGame Over! Final Score: " << score << endl;
    return 0;
}
