#include <iostream>
#include <iomanip>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <conio.h>

using namespace std;

const int SIZE = 4;

int board[SIZE][SIZE] = {};
int score = 0;

// 画面クリア
void clearScreen() {
    system("cls");
}

// 盤面表示
void printBoard() {
    clearScreen();

    cout << "===== 2048 =====\n";
    cout << "Score: " << score << "\n\n";

    for (int y = 0; y < SIZE; y++) {
        cout << "+------+------+------+------+\n";

        for (int x = 0; x < SIZE; x++) {
            cout << "|";

            if (board[y][x] == 0) {
                cout << setw(6) << " ";
            } else {
                cout << setw(6) << board[y][x];
            }
        }

        cout << "|\n";
    }

    cout << "+------+------+------+------+\n\n";

    cout << "矢印キーで移動\n";
    cout << "ESCで終了\n";
}

// 空いているマスに2か4を追加
void addRandomTile() {
    vector<pair<int, int>> emptyCells;

    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] == 0) {
                emptyCells.push_back({y, x});
            }
        }
    }

    if (emptyCells.empty()) {
        return;
    }

    int index = rand() % emptyCells.size();

    int y = emptyCells[index].first;
    int x = emptyCells[index].second;

    // 90%で2、10%で4
    if (rand() % 10 == 0) {
        board[y][x] = 4;
    } else {
        board[y][x] = 2;
    }
}

// 1列分を左に詰めて合体
vector<int> mergeLine(vector<int> line) {
    vector<int> temp;

    // 0を削除
    for (int value : line) {
        if (value != 0) {
            temp.push_back(value);
        }
    }

    vector<int> result;

    for (int i = 0; i < (int)temp.size(); i++) {
        if (i + 1 < (int)temp.size() &&
            temp[i] == temp[i + 1]) {

            int mergedValue = temp[i] * 2;

            result.push_back(mergedValue);

            score += mergedValue;

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

// 左移動
bool moveLeft() {
    bool changed = false;

    for (int y = 0; y < SIZE; y++) {
        vector<int> oldLine;

        for (int x = 0; x < SIZE; x++) {
            oldLine.push_back(board[y][x]);
        }

        vector<int> newLine = mergeLine(oldLine);

        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] != newLine[x]) {
                changed = true;
            }

            board[y][x] = newLine[x];
        }
    }

    return changed;
}

// 右移動
bool moveRight() {
    bool changed = false;

    for (int y = 0; y < SIZE; y++) {
        vector<int> oldLine;

        for (int x = SIZE - 1; x >= 0; x--) {
            oldLine.push_back(board[y][x]);
        }

        vector<int> newLine = mergeLine(oldLine);

        for (int i = 0; i < SIZE; i++) {
            int x = SIZE - 1 - i;

            if (board[y][x] != newLine[i]) {
                changed = true;
            }

            board[y][x] = newLine[i];
        }
    }

    return changed;
}

// 上移動
bool moveUp() {
    bool changed = false;

    for (int x = 0; x < SIZE; x++) {
        vector<int> oldLine;

        for (int y = 0; y < SIZE; y++) {
            oldLine.push_back(board[y][x]);
        }

        vector<int> newLine = mergeLine(oldLine);

        for (int y = 0; y < SIZE; y++) {
            if (board[y][x] != newLine[y]) {
                changed = true;
            }

            board[y][x] = newLine[y];
        }
    }

    return changed;
}

// 下移動
bool moveDown() {
    bool changed = false;

    for (int x = 0; x < SIZE; x++) {
        vector<int> oldLine;

        for (int y = SIZE - 1; y >= 0; y--) {
            oldLine.push_back(board[y][x]);
        }

        vector<int> newLine = mergeLine(oldLine);

        for (int i = 0; i < SIZE; i++) {
            int y = SIZE - 1 - i;

            if (board[y][x] != newLine[i]) {
                changed = true;
            }

            board[y][x] = newLine[i];
        }
    }

    return changed;
}

// 2048があるか
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

// まだ動けるか
bool canMove() {
    // 空きマスがある
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] == 0) {
                return true;
            }
        }
    }

    // 横に同じ数字がある
    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE - 1; x++) {
            if (board[y][x] == board[y][x + 1]) {
                return true;
            }
        }
    }

    // 縦に同じ数字がある
    for (int y = 0; y < SIZE - 1; y++) {
        for (int x = 0; x < SIZE; x++) {
            if (board[y][x] == board[y + 1][x]) {
                return true;
            }
        }
    }

    return false;
}

// ゲーム初期化
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
    srand((unsigned int)time(nullptr));

    initializeGame();

    bool wonMessageShown = false;

    while (true) {
        printBoard();

        if (hasWon() && !wonMessageShown) {
            cout << "2048達成！！\n";
            cout << "そのまま続けられます。\n\n";

            wonMessageShown = true;
        }

        if (!canMove()) {
            cout << "GAME OVER\n";
            cout << "最終スコア: " << score << "\n";
            break;
        }

        int key = _getch();

        // ESC
        if (key == 27) {
            break;
        }

        bool moved = false;

        // 矢印キー
        if (key == 0 || key == 224) {
            key = _getch();

            switch (key) {
                case 72: // ↑
                    moved = moveUp();
                    break;

                case 80: // ↓
                    moved = moveDown();
                    break;

                case 75: // ←
                    moved = moveLeft();
                    break;

                case 77: // →
                    moved = moveRight();
                    break;
            }
        }

        // 実際に盤面が変わった時だけ新しい数字を追加
        if (moved) {
            addRandomTile();
        }
    }

    cout << "\n終了しました。\n";

    return 0;
}
