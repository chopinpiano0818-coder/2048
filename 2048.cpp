#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <termios.h>
#include <unistd.h>
#include <string>

using namespace std;

const int SIZE = 4;

int board[SIZE][SIZE] = {};
int score = 0;

char getKey() {
    termios oldt, newt;

    tcgetattr(STDIN_FILENO, &oldt);
    newt = oldt;

    newt.c_lflag &= ~(ICANON | ECHO);

    tcsetattr(STDIN_FILENO, TCSANOW, &newt);

    char ch = getchar();

    tcsetattr(STDIN_FILENO, TCSANOW, &oldt);

    return ch;
}

void clearScreen() {
    cout << "\033[2J\033[1;1H";
}

string getTileStyle(int value) {
    switch (value) {
        case 0:
            return "\033[48;5;236m\033[37m";

        case 2:
            return "\033[48;5;230m\033[38;5;237m";

        case 4:
            return "\033[48;5;223m\033[38;5;237m";

        case 8:
            return "\033[48;5;215m\033[97m";

        case 16:
            return "\033[48;5;209m\033[97m";

        case 32:
            return "\033[48;5;203m\033[97m";

        case 64:
            return "\033[48;5;196m\033[97m";

        case 128:
            return "\033[48;5;221m\033[97m";

        case 256:
            return "\033[48;5;220m\033[97m";

        case 512:
            return "\033[48;5;214m\033[97m";

        case 1024:
            return "\033[48;5;208m\033[97m";

        case 2048:
            return "\033[48;5;226m\033[30m";

        default:
            return "\033[48;5;93m\033[97m";
    }
}

void printTile(int value) {
    cout << getTileStyle(value);
    cout << "\033[1m";

    if (value == 0) {
        cout << "        ";
    } else {
        cout << setw(8) << value;
    }

    cout << "\033[0m";
}

void printBoard() {
    clearScreen();

    cout << "\033[1m";
    cout << "=========== 2048 ===========\n";
    cout << "Score: " << score << "\n\n";
    cout << "\033[0m";

    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            cout << " ";
            printTile(board[y][x]);
        }

        cout << "\n\n";
    }

    cout << "↑ ↓ ← → で移動\n";
    cout << "Q で終了\n";
}

void addRandomTile() {
    vector<pair<int, int>> emptyCells;

    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] == 0) {
                emptyCells.push_back({y, x});
            }
        }
    }

    if (emptyCells.empty())
        return;

    int index = rand() % emptyCells.size();

    int y = emptyCells[index].first;
    int x = emptyCells[index].second;

    board[y][x] = (rand() % 10 == 0) ? 4 : 2;
}

vector<int> mergeLine(vector<int> line) {
    vector<int> temp;

    for (int value : line) {
        if (value != 0) {
            temp.push_back(value);
        }
    }

    vector<int> result;

    for (int i = 0; i < (int)temp.size(); i++) {
        if (
            i + 1 < (int)temp.size() &&
            temp[i] == temp[i + 1]
        ) {
            int merged = temp[i] * 2;

            result.push_back(merged);
            score += merged;

            i++;
        } else {
            result.push_back(temp[i]);
        }
    }

    while ((int)result.size() < SIZE) {
        result.push_back(0);
    }

    return result;
}

bool moveLeft() {
    bool changed = false;

    for (int y = 0; y < SIZE; y++) {
        vector<int> line;

        for (int x = 0; x < SIZE; x++) {
            line.push_back(board[y][x]);
        }

        vector<int> result = mergeLine(line);

        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] != result[x]) {
                changed = true;
            }

            board[y][x] = result[x];
        }
    }

    return changed;
}

bool moveRight() {
    bool changed = false;

    for (int y = 0; y < SIZE; y++) {
        vector<int> line;

        for (int x = SIZE - 1; x >= 0; x--) {
            line.push_back(board[y][x]);
        }

        vector<int> result = mergeLine(line);

        for (int i = 0; i < SIZE; i++) {
            int x = SIZE - 1 - i;

            if (board[y][x] != result[i]) {
                changed = true;
            }

            board[y][x] = result[i];
        }
    }

    return changed;
}

bool moveUp() {
    bool changed = false;

    for (int x = 0; x < SIZE; x++) {
        vector<int> line;

        for (int y = 0; y < SIZE; y++) {
            line.push_back(board[y][x]);
        }

        vector<int> result = mergeLine(line);

        for (int y = 0; y < SIZE; y++) {
            if (board[y][x] != result[y]) {
                changed = true;
            }

            board[y][x] = result[y];
        }
    }

    return changed;
}

bool moveDown() {
    bool changed = false;

    for (int x = 0; x < SIZE; x++) {
        vector<int> line;

        for (int y = SIZE - 1; y >= 0; y--) {
            line.push_back(board[y][x]);
        }

        vector<int> result = mergeLine(line);

        for (int i = 0; i < SIZE; i++) {
            int y = SIZE - 1 - i;

            if (board[y][x] != result[i]) {
                changed = true;
            }

            board[y][x] = result[i];
        }
    }

    return changed;
}

bool hasWon() {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] >= 2048) {
                return true;
            }
        }
    }

    return false;
}

bool canMove() {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] == 0) {
                return true;
            }
        }
    }

    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE - 1; x++) {
            if (board[y][x] == board[y][x + 1]) {
                return true;
            }
        }
    }

    for (int y = 0; y < SIZE - 1; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] == board[y + 1][x]) {
                return true;
            }
        }
    }

    return false;
}

void initializeGame() {
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            board[y][x] = 0;
        }
    }

    score = 0;

    addRandomTile();
    addRandomTile();
}

int main() {
    srand(time(nullptr));

    initializeGame();

    bool winShown = false;

    while (true) {
        printBoard();

        if (hasWon() && !winShown) {
            cout << "\033[93m\033[1m";
            cout << "★ 2048達成！！ ★\n";
            cout << "\033[0m";
            cout << "そのまま続行できます。\n\n";

            winShown = true;
        }

        if (!canMove()) {
            cout << "\033[91m\033[1m";
            cout << "GAME OVER!\n";
            cout << "\033[0m";

            cout << "最終スコア: " << score << "\n";
            break;
        }

        char key = getKey();

        bool moved = false;

        if (key == 'q' || key == 'Q') {
            break;
        }

        if (key == 27) {
            char second = getKey();

            if (second == '[') {
                char third = getKey();

                if (third == 'A') {
                    moved = moveUp();
                }
                else if (third == 'B') {
                    moved = moveDown();
                }
                else if (third == 'C') {
                    moved = moveRight();
                }
                else if (third == 'D') {
                    moved = moveLeft();
                }
            }
        }

        if (moved) {
            addRandomTile();
        }
    }

    cout << "\033[0m";
    cout << "\n終了しました。\n";

    return 0;
}
