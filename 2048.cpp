#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>

#include <iostream>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>
#include <fstream>
#include <algorithm>
#include <cmath>

using namespace std;

// ============================================================
// 基本設定
// ============================================================

const int WINDOW_W = 620;
const int WINDOW_H = 800;

const int SIZE = 4;

const int BOARD_X = 65;
const int BOARD_Y = 210;

const int TILE = 110;
const int GAP = 10;

const string BEST_FILE = "2048_best.txt";

int board[SIZE][SIZE] = {};

int score = 0;
int bestScore = 0;

bool running = true;
bool won = false;

// ============================================================
// アイテム
// ============================================================

int hammerCount = 2;
int shuffleCount = 1;
int undoCount = 3;

// ============================================================
// Undo用
// ============================================================

int previousBoard[SIZE][SIZE] = {};
int previousScore = 0;

bool hasUndoData = false;

// ============================================================
// SDL
// ============================================================

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

TTF_Font* fontHuge = nullptr;
TTF_Font* fontBig = nullptr;
TTF_Font* fontMedium = nullptr;
TTF_Font* fontSmall = nullptr;


// ============================================================
// 方向
// ============================================================

enum Direction {
    LEFT,
    RIGHT,
    UP,
    DOWN
};


// ============================================================
// 色
// ============================================================

SDL_Color getTileColor(int value) {

    switch (value) {

        case 0:
            return {205, 193, 180, 255};

        case 2:
            return {238, 228, 218, 255};

        case 4:
            return {237, 224, 200, 255};

        case 8:
            return {242, 177, 121, 255};

        case 16:
            return {245, 149, 99, 255};

        case 32:
            return {246, 124, 95, 255};

        case 64:
            return {246, 94, 59, 255};

        case 128:
            return {237, 207, 114, 255};

        case 256:
            return {237, 204, 97, 255};

        case 512:
            return {237, 200, 80, 255};

        case 1024:
            return {237, 197, 63, 255};

        case 2048:
            return {237, 194, 46, 255};

        case 4096:
            return {190, 100, 210, 255};

        case 8192:
            return {150, 70, 190, 255};

        default:
            return {60, 58, 50, 255};
    }
}


SDL_Color getTextColor(int value) {

    if (value <= 4) {
        return {119, 110, 101, 255};
    }

    return {249, 246, 242, 255};
}


// ============================================================
// キーキュー削除
// ============================================================

void clearQueuedKeys() {

    SDL_FlushEvent(SDL_KEYDOWN);
    SDL_FlushEvent(SDL_KEYUP);
}


// ============================================================
// 文字表示
// ============================================================

void drawText(
    const string& text,
    int x,
    int y,
    TTF_Font* font,
    SDL_Color color,
    bool center = false
) {

    SDL_Surface* surface =
        TTF_RenderUTF8_Blended(
            font,
            text.c_str(),
            color
        );

    if (!surface) {
        return;
    }

    SDL_Texture* texture =
        SDL_CreateTextureFromSurface(
            renderer,
            surface
        );

    if (!texture) {

        SDL_FreeSurface(surface);
        return;
    }

    SDL_Rect rect;

    rect.w = surface->w;
    rect.h = surface->h;

    if (center) {

        rect.x = x - rect.w / 2;
        rect.y = y - rect.h / 2;

    } else {

        rect.x = x;
        rect.y = y;
    }

    SDL_RenderCopy(
        renderer,
        texture,
        nullptr,
        &rect
    );

    SDL_FreeSurface(surface);
    SDL_DestroyTexture(texture);
}


// ============================================================
// 四角
// ============================================================

void drawRect(
    SDL_Rect rect,
    SDL_Color color
) {

    SDL_SetRenderDrawColor(
        renderer,
        color.r,
        color.g,
        color.b,
        color.a
    );

    SDL_RenderFillRect(
        renderer,
        &rect
    );
}


// ============================================================
// タイル
// ============================================================

void drawTile(
    int value,
    float row,
    float col,
    float scale = 1.0f
) {

    float cx =
        BOARD_X +
        col * (TILE + GAP) +
        TILE / 2.0f;

    float cy =
        BOARD_Y +
        row * (TILE + GAP) +
        TILE / 2.0f;

    int tileSize =
        static_cast<int>(
            TILE * scale
        );

    SDL_Rect rect;

    rect.w = tileSize;
    rect.h = tileSize;

    rect.x =
        static_cast<int>(
            cx - tileSize / 2.0f
        );

    rect.y =
        static_cast<int>(
            cy - tileSize / 2.0f
        );

    drawRect(
        rect,
        getTileColor(value)
    );

    if (value == 0) {
        return;
    }

    TTF_Font* useFont = fontBig;

    if (value >= 1000) {
        useFont = fontMedium;
    }

    if (value >= 10000) {
        useFont = fontSmall;
    }

    drawText(
        to_string(value),
        rect.x + rect.w / 2,
        rect.y + rect.h / 2,
        useFont,
        getTextColor(value),
        true
    );
}


// ============================================================
// アイテムUI
// ============================================================

void drawItemBar() {

    SDL_Color dark =
        {119, 110, 101, 255};

    SDL_Color itemColor =
        {187, 173, 160, 255};

    SDL_Rect hammerBox =
        {65, 710, 150, 55};

    SDL_Rect shuffleBox =
        {235, 710, 150, 55};

    SDL_Rect undoBox =
        {405, 710, 150, 55};

    drawRect(
        hammerBox,
        itemColor
    );

    drawRect(
        shuffleBox,
        itemColor
    );

    drawRect(
        undoBox,
        itemColor
    );

    drawText(
        "H  HAMMER",
        140,
        727,
        fontSmall,
        {255, 255, 255, 255},
        true
    );

    drawText(
        "x" + to_string(hammerCount),
        140,
        749,
        fontSmall,
        {255, 255, 255, 255},
        true
    );

    drawText(
        "S  SHUFFLE",
        310,
        727,
        fontSmall,
        {255, 255, 255, 255},
        true
    );

    drawText(
        "x" + to_string(shuffleCount),
        310,
        749,
        fontSmall,
        {255, 255, 255, 255},
        true
    );

    drawText(
        "U  UNDO",
        480,
        727,
        fontSmall,
        {255, 255, 255, 255},
        true
    );

    drawText(
        "x" + to_string(undoCount),
        480,
        749,
        fontSmall,
        {255, 255, 255, 255},
        true
    );
}


// ============================================================
// 背景
// ============================================================

void drawBackground() {

    SDL_SetRenderDrawColor(
        renderer,
        250,
        248,
        239,
        255
    );

    SDL_RenderClear(renderer);

    drawText(
        "2048",
        65,
        35,
        fontHuge,
        {119, 110, 101, 255}
    );

    SDL_Rect scoreBox =
        {340, 35, 100, 70};

    SDL_Rect bestBox =
        {455, 35, 100, 70};

    drawRect(
        scoreBox,
        {187, 173, 160, 255}
    );

    drawRect(
        bestBox,
        {187, 173, 160, 255}
    );

    drawText(
        "SCORE",
        390,
        50,
        fontSmall,
        {238, 228, 218, 255},
        true
    );

    drawText(
        to_string(score),
        390,
        80,
        fontMedium,
        {255, 255, 255, 255},
        true
    );

    drawText(
        "BEST",
        505,
        50,
        fontSmall,
        {238, 228, 218, 255},
        true
    );

    drawText(
        to_string(bestScore),
        505,
        80,
        fontMedium,
        {255, 255, 255, 255},
        true
    );

    drawText(
        "Arrow keys: Move",
        65,
        145,
        fontSmall,
        {119, 110, 101, 255}
    );

    drawText(
        "R: Restart   ESC: Quit",
        350,
        145,
        fontSmall,
        {119, 110, 101, 255}
    );

    SDL_Rect boardBack =
        {
            BOARD_X - GAP,
            BOARD_Y - GAP,
            SIZE * TILE + (SIZE + 1) * GAP,
            SIZE * TILE + (SIZE + 1) * GAP
        };

    drawRect(
        boardBack,
        {187, 173, 160, 255}
    );

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            drawTile(
                0,
                y,
                x
            );
        }
    }

    drawItemBar();
}


// ============================================================
// 盤面
// ============================================================

void drawBoard() {

    drawBackground();

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            if (board[y][x] != 0) {

                drawTile(
                    board[y][x],
                    y,
                    x
                );
            }
        }
    }

    SDL_RenderPresent(renderer);
}


// ============================================================
// BEST
// ============================================================

void loadBest() {

    ifstream file(BEST_FILE);

    if (file) {
        file >> bestScore;
    }
}


void saveBest() {

    if (score > bestScore) {

        bestScore = score;

        ofstream file(BEST_FILE);

        if (file) {
            file << bestScore;
        }
    }
}


// ============================================================
// Undo保存
// ============================================================

void saveUndoState() {

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            previousBoard[y][x] =
                board[y][x];
        }
    }

    previousScore = score;

    hasUndoData = true;
}


// ============================================================
// Undo
// ============================================================

void useUndo() {

    if (
        undoCount <= 0 ||
        !hasUndoData
    ) {
        return;
    }

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            board[y][x] =
                previousBoard[y][x];
        }
    }

    score = previousScore;

    undoCount--;

    hasUndoData = false;

    drawBoard();

    clearQueuedKeys();
}


// ============================================================
// 新規タイル
// ============================================================

void addRandomTile(
    bool animate = true
) {

    vector<pair<int, int>> empty;

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            if (board[y][x] == 0) {

                empty.push_back(
                    {y, x}
                );
            }
        }
    }

    if (empty.empty()) {
        return;
    }

    auto pos =
        empty[
            rand() % empty.size()
        ];

    int y = pos.first;
    int x = pos.second;

    board[y][x] =
        rand() % 10 == 0
        ? 4
        : 2;

    if (!animate) {
        return;
    }

    const int frames = 9;

    for (
        int i = 0;
        i <= frames;
        i++
    ) {

        drawBackground();

        for (int yy = 0; yy < SIZE; yy++) {

            for (int xx = 0; xx < SIZE; xx++) {

                if (board[yy][xx] == 0) {
                    continue;
                }

                if (
                    yy == y &&
                    xx == x
                ) {

                    float t =
                        i /
                        (float)frames;

                    float scale =
                        0.15f +
                        0.85f * t;

                    drawTile(
                        board[yy][xx],
                        yy,
                        xx,
                        scale
                    );

                } else {

                    drawTile(
                        board[yy][xx],
                        yy,
                        xx
                    );
                }
            }
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }
}


// ============================================================
// 座標
// ============================================================

pair<int, int> getCell(
    Direction dir,
    int line,
    int pos
) {

    switch (dir) {

        case LEFT:
            return {line, pos};

        case RIGHT:
            return {
                line,
                SIZE - 1 - pos
            };

        case UP:
            return {pos, line};

        case DOWN:
            return {
                SIZE - 1 - pos,
                line
            };
    }

    return {0, 0};
}


// ============================================================
// 動くタイル
// ============================================================

struct MovingTile {

    int value;

    float fromRow;
    float fromCol;

    float toRow;
    float toCol;
};


struct MergeTile {

    int row;
    int col;
};


// ============================================================
// 移動
// ============================================================

bool moveBoard(Direction dir) {

    int oldBoard[SIZE][SIZE];

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            oldBoard[y][x] =
                board[y][x];
        }
    }

    int oldScore = score;

    vector<MovingTile> moving;
    vector<MergeTile> merges;

    int newBoard[SIZE][SIZE] = {};

    bool changed = false;

    for (int line = 0; line < SIZE; line++) {

        struct Item {

            int value;
            int pos;
        };

        vector<Item> items;

        for (int pos = 0; pos < SIZE; pos++) {

            auto cell =
                getCell(
                    dir,
                    line,
                    pos
                );

            int value =
                oldBoard[
                    cell.first
                ][
                    cell.second
                ];

            if (value != 0) {

                items.push_back(
                    {value, pos}
                );
            }
        }

        int targetPos = 0;

        for (
            int i = 0;
            i < (int)items.size();
            i++
        ) {

            if (
                i + 1 < (int)items.size() &&
                items[i].value ==
                items[i + 1].value
            ) {

                auto from1 =
                    getCell(
                        dir,
                        line,
                        items[i].pos
                    );

                auto from2 =
                    getCell(
                        dir,
                        line,
                        items[i + 1].pos
                    );

                auto target =
                    getCell(
                        dir,
                        line,
                        targetPos
                    );

                moving.push_back({
                    items[i].value,
                    (float)from1.first,
                    (float)from1.second,
                    (float)target.first,
                    (float)target.second
                });

                moving.push_back({
                    items[i + 1].value,
                    (float)from2.first,
                    (float)from2.second,
                    (float)target.first,
                    (float)target.second
                });

                int mergedValue =
                    items[i].value * 2;

                newBoard[
                    target.first
                ][
                    target.second
                ] =
                    mergedValue;

                score += mergedValue;

                merges.push_back({
                    target.first,
                    target.second
                });

                changed = true;

                i++;

            } else {

                auto from =
                    getCell(
                        dir,
                        line,
                        items[i].pos
                    );

                auto target =
                    getCell(
                        dir,
                        line,
                        targetPos
                    );

                moving.push_back({
                    items[i].value,
                    (float)from.first,
                    (float)from.second,
                    (float)target.first,
                    (float)target.second
                });

                newBoard[
                    target.first
                ][
                    target.second
                ] =
                    items[i].value;

                if (
                    from.first != target.first ||
                    from.second != target.second
                ) {

                    changed = true;
                }
            }

            targetPos++;
        }
    }

    if (!changed) {

        score = oldScore;

        return false;
    }

    // Undo用に移動前を保存

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            previousBoard[y][x] =
                oldBoard[y][x];
        }
    }

    previousScore = oldScore;
    hasUndoData = true;


    // ========================================================
    // スライドアニメーション
    // ========================================================

    const int FRAMES = 12;

    for (
        int frame = 0;
        frame <= FRAMES;
        frame++
    ) {

        float t =
            frame /
            (float)FRAMES;

        float smooth =
            1.0f -
            pow(
                1.0f - t,
                3.0f
            );

        drawBackground();

        for (
            const auto& tile :
            moving
        ) {

            float r =
                tile.fromRow +
                (
                    tile.toRow -
                    tile.fromRow
                ) * smooth;

            float c =
                tile.fromCol +
                (
                    tile.toCol -
                    tile.fromCol
                ) * smooth;

            drawTile(
                tile.value,
                r,
                c
            );
        }

        SDL_RenderPresent(renderer);

        SDL_Delay(10);
    }

    // 新盤面反映

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            board[y][x] =
                newBoard[y][x];
        }
    }


    // ========================================================
    // 合体ボヨン
    // ========================================================

    if (!merges.empty()) {

        const int BOUNCE_FRAMES = 10;

        for (
            int frame = 0;
            frame < BOUNCE_FRAMES;
            frame++
        ) {

            drawBackground();

            for (int y = 0; y < SIZE; y++) {

                for (int x = 0; x < SIZE; x++) {

                    if (board[y][x] == 0) {
                        continue;
                    }

                    float scale = 1.0f;

                    for (
                        const auto& m :
                        merges
                    ) {

                        if (
                            m.row == y &&
                            m.col == x
                        ) {

                            if (frame < 5) {

                                scale =
                                    1.0f +
                                    frame * 0.045f;

                            } else {

                                scale =
                                    1.18f -
                                    (frame - 5)
                                    * 0.036f;
                            }
                        }
                    }

                    drawTile(
                        board[y][x],
                        y,
                        x,
                        scale
                    );
                }
            }

            SDL_RenderPresent(renderer);

            SDL_Delay(12);
        }
    }

    saveBest();

    addRandomTile(true);

    // アニメーション中の入力を捨てる
    clearQueuedKeys();

    return true;
}


// ============================================================
// HAMMER
// ============================================================

void useHammer() {

    if (hammerCount <= 0) {
        return;
    }

    int cursorY = 0;
    int cursorX = 0;

    clearQueuedKeys();

    while (true) {

        drawBoard();

        SDL_SetRenderDrawBlendMode(
            renderer,
            SDL_BLENDMODE_BLEND
        );

        SDL_Rect selection;

        selection.x =
            BOARD_X +
            cursorX * (TILE + GAP);

        selection.y =
            BOARD_Y +
            cursorY * (TILE + GAP);

        selection.w = TILE;
        selection.h = TILE;

        SDL_SetRenderDrawColor(
            renderer,
            255,
            40,
            40,
            150
        );

        SDL_RenderFillRect(
            renderer,
            &selection
        );

        drawText(
            "HAMMER: Choose a tile",
            WINDOW_W / 2,
            185,
            fontSmall,
            {180, 50, 50, 255},
            true
        );

        SDL_RenderPresent(renderer);

        SDL_Event event;

        if (!SDL_WaitEvent(&event)) {
            continue;
        }

        if (event.type == SDL_QUIT) {

            running = false;
            return;
        }

        if (
            event.type != SDL_KEYDOWN ||
            event.key.repeat != 0
        ) {
            continue;
        }

        SDL_Keycode key =
            event.key.keysym.sym;

        if (key == SDLK_LEFT) {

            cursorX =
                max(0, cursorX - 1);
        }

        else if (key == SDLK_RIGHT) {

            cursorX =
                min(
                    SIZE - 1,
                    cursorX + 1
                );
        }

        else if (key == SDLK_UP) {

            cursorY =
                max(0, cursorY - 1);
        }

        else if (key == SDLK_DOWN) {

            cursorY =
                min(
                    SIZE - 1,
                    cursorY + 1
                );
        }

        else if (
            key == SDLK_RETURN ||
            key == SDLK_KP_ENTER
        ) {

            if (
                board[cursorY][cursorX]
                != 0
            ) {

                saveUndoState();

                board[cursorY][cursorX] = 0;

                hammerCount--;

                drawBoard();

                clearQueuedKeys();

                return;
            }
        }

        else if (
            key == SDLK_ESCAPE
        ) {

            clearQueuedKeys();

            return;
        }
    }
}


// ============================================================
// SHUFFLE
// ============================================================

void useShuffle() {

    if (shuffleCount <= 0) {
        return;
    }

    vector<int> values;

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            if (board[y][x] != 0) {

                values.push_back(
                    board[y][x]
                );
            }
        }
    }

    if (values.size() <= 1) {
        return;
    }

    saveUndoState();

    shuffle(
        values.begin(),
        values.end(),
        std::default_random_engine(
            (unsigned)time(nullptr)
        )
    );

    vector<pair<int, int>> positions;

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            positions.push_back(
                {y, x}
            );

            board[y][x] = 0;
        }
    }

    random_shuffle(
        positions.begin(),
        positions.end()
    );

    for (
        int i = 0;
        i < (int)values.size();
        i++
    ) {

        board[
            positions[i].first
        ][
            positions[i].second
        ] =
            values[i];
    }

    shuffleCount--;

    // 簡単なシャッフル演出

    for (int i = 0; i < 4; i++) {

        drawBackground();

        SDL_RenderPresent(renderer);

        SDL_Delay(45);

        drawBoard();

        SDL_Delay(45);
    }

    clearQueuedKeys();
}


// ============================================================
// 動けるか
// ============================================================

bool canMove() {

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            if (board[y][x] == 0) {
                return true;
            }

            if (
                x + 1 < SIZE &&
                board[y][x] ==
                board[y][x + 1]
            ) {

                return true;
            }

            if (
                y + 1 < SIZE &&
                board[y][x] ==
                board[y + 1][x]
            ) {

                return true;
            }
        }
    }

    return false;
}


// ============================================================
// 2048判定
// ============================================================

bool checkWin() {

    if (won) {
        return false;
    }

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            if (board[y][x] >= 2048) {

                won = true;

                return true;
            }
        }
    }

    return false;
}


// ============================================================
// WINメニュー
// ============================================================

enum WinChoice {

    WIN_CONTINUE,
    WIN_QUIT
};


WinChoice showWinMenu() {

    clearQueuedKeys();

    int selected = 0;

    while (running) {

        drawBoard();

        SDL_SetRenderDrawBlendMode(
            renderer,
            SDL_BLENDMODE_BLEND
        );

        SDL_SetRenderDrawColor(
            renderer,
            238,
            228,
            218,
            235
        );

        SDL_Rect overlay = {
            BOARD_X - GAP,
            BOARD_Y - GAP,
            SIZE * TILE +
            (SIZE + 1) * GAP,
            SIZE * TILE +
            (SIZE + 1) * GAP
        };

        SDL_RenderFillRect(
            renderer,
            &overlay
        );

        drawText(
            "2048 CLEAR!",
            WINDOW_W / 2,
            330,
            fontHuge,
            {119, 110, 101, 255},
            true
        );

        drawText(
            "Continue?",
            WINDOW_W / 2,
            395,
            fontMedium,
            {119, 110, 101, 255},
            true
        );


        SDL_Rect continueButton =
            {165, 440, 290, 60};

        SDL_Rect quitButton =
            {165, 515, 290, 60};


        if (selected == 0) {

            drawRect(
                continueButton,
                {237, 194, 46, 255}
            );

        } else {

            drawRect(
                continueButton,
                {187, 173, 160, 255}
            );
        }


        if (selected == 1) {

            drawRect(
                quitButton,
                {246, 94, 59, 255}
            );

        } else {

            drawRect(
                quitButton,
                {187, 173, 160, 255}
            );
        }


        drawText(
            "CONTINUE",
            WINDOW_W / 2,
            470,
            fontMedium,
            {255, 255, 255, 255},
            true
        );

        drawText(
            "QUIT",
            WINDOW_W / 2,
            545,
            fontMedium,
            {255, 255, 255, 255},
            true
        );

        drawText(
            "Arrow keys + Enter",
            WINDOW_W / 2,
            610,
            fontSmall,
            {119, 110, 101, 255},
            true
        );

        SDL_RenderPresent(renderer);


        SDL_Event event;

        if (!SDL_WaitEvent(&event)) {
            continue;
        }

        if (event.type == SDL_QUIT) {

            return WIN_QUIT;
        }

        if (
            event.type == SDL_KEYDOWN &&
            event.key.repeat == 0
        ) {

            SDL_Keycode key =
                event.key.keysym.sym;

            if (
                key == SDLK_UP ||
                key == SDLK_LEFT
            ) {

                selected = 0;
            }

            else if (
                key == SDLK_DOWN ||
                key == SDLK_RIGHT
            ) {

                selected = 1;
            }

            else if (
                key == SDLK_RETURN ||
                key == SDLK_KP_ENTER
            ) {

                clearQueuedKeys();

                if (selected == 0) {

                    return WIN_CONTINUE;

                } else {

                    return WIN_QUIT;
                }
            }

            else if (
                key == SDLK_ESCAPE
            ) {

                return WIN_QUIT;
            }
        }
    }

    return WIN_QUIT;
}


// ============================================================
// GAME OVER
// ============================================================

void drawGameOver() {

    drawBoard();

    SDL_SetRenderDrawBlendMode(
        renderer,
        SDL_BLENDMODE_BLEND
    );

    SDL_SetRenderDrawColor(
        renderer,
        238,
        228,
        218,
        220
    );

    SDL_Rect overlay = {
        BOARD_X - GAP,
        BOARD_Y - GAP,
        SIZE * TILE +
        (SIZE + 1) * GAP,
        SIZE * TILE +
        (SIZE + 1) * GAP
    };

    SDL_RenderFillRect(
        renderer,
        &overlay
    );

    drawText(
        "GAME OVER",
        WINDOW_W / 2,
        390,
        fontHuge,
        {119, 110, 101, 255},
        true
    );

    drawText(
        "R: Restart",
        WINDOW_W / 2,
        455,
        fontMedium,
        {119, 110, 101, 255},
        true
    );

    drawText(
        "ESC: Quit",
        WINDOW_W / 2,
        500,
        fontSmall,
        {119, 110, 101, 255},
        true
    );

    SDL_RenderPresent(renderer);
}


// ============================================================
// リスタート
// ============================================================

void restartGame() {

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            board[y][x] = 0;
        }
    }

    score = 0;

    won = false;

    hammerCount = 2;
    shuffleCount = 1;
    undoCount = 3;

    hasUndoData = false;

    addRandomTile(false);
    addRandomTile(false);

    drawBoard();

    clearQueuedKeys();
}


// ============================================================
// 初期化
// ============================================================

bool init() {

    if (
        SDL_Init(
            SDL_INIT_VIDEO
        ) < 0
    ) {

        cout
            << "SDL error: "
            << SDL_GetError()
            << endl;

        return false;
    }

    if (TTF_Init() < 0) {

        cout
            << "TTF error: "
            << TTF_GetError()
            << endl;

        return false;
    }

    window =
        SDL_CreateWindow(
            "2048 Items Edition",
            SDL_WINDOWPOS_CENTERED,
            SDL_WINDOWPOS_CENTERED,
            WINDOW_W,
            WINDOW_H,
            SDL_WINDOW_SHOWN
        );

    if (!window) {

        cout
            << SDL_GetError()
            << endl;

        return false;
    }

    renderer =
        SDL_CreateRenderer(
            window,
            -1,
            SDL_RENDERER_ACCELERATED |
            SDL_RENDERER_PRESENTVSYNC
        );

    if (!renderer) {

        renderer =
            SDL_CreateRenderer(
                window,
                -1,
                SDL_RENDERER_SOFTWARE
            );
    }

    if (!renderer) {

        cout
            << SDL_GetError()
            << endl;

        return false;
    }


    string fontPath =
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";


    fontHuge =
        TTF_OpenFont(
            fontPath.c_str(),
            48
        );

    fontBig =
        TTF_OpenFont(
            fontPath.c_str(),
            38
        );

    fontMedium =
        TTF_OpenFont(
            fontPath.c_str(),
            25
        );

    fontSmall =
        TTF_OpenFont(
            fontPath.c_str(),
            16
        );


    if (
        !fontHuge ||
        !fontBig ||
        !fontMedium ||
        !fontSmall
    ) {

        cout
            << "Font error: "
            << TTF_GetError()
            << endl;

        return false;
    }

    return true;
}


// ============================================================
// 終了
// ============================================================

void cleanup() {

    saveBest();

    TTF_CloseFont(fontHuge);
    TTF_CloseFont(fontBig);
    TTF_CloseFont(fontMedium);
    TTF_CloseFont(fontSmall);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}


// ============================================================
// MAIN
// ============================================================

int main() {

    srand(
        (unsigned)time(nullptr)
    );

    loadBest();

    if (!init()) {

        return 1;
    }

    restartGame();

    bool gameOver = false;

    while (running) {

        SDL_Event event;


        if (
            !SDL_WaitEvent(
                &event
            )
        ) {

            continue;
        }


        if (
            event.type ==
            SDL_QUIT
        ) {

            running = false;

            break;
        }


        if (
            event.type != SDL_KEYDOWN ||
            event.key.repeat != 0
        ) {

            continue;
        }


        SDL_Keycode key =
            event.key.keysym.sym;


        // ====================================================
        // ESC
        // ====================================================

        if (key == SDLK_ESCAPE) {

            running = false;

            break;
        }


        // ====================================================
        // RESTART
        // ====================================================

        if (key == SDLK_r) {

            restartGame();

            gameOver = false;

            continue;
        }


        // ====================================================
        // GAME OVER中
        // ====================================================

        if (gameOver) {

            continue;
        }


        // ====================================================
        // アイテム
        // ====================================================

        if (key == SDLK_h) {

            useHammer();

            if (!running) {
                break;
            }

            drawBoard();

            continue;
        }


        if (key == SDLK_s) {

            useShuffle();

            drawBoard();

            continue;
        }


        if (key == SDLK_u) {

            useUndo();

            drawBoard();

            continue;
        }


        // ====================================================
        // 移動
        // ====================================================

        bool moved = false;


        if (key == SDLK_LEFT) {

            moved =
                moveBoard(
                    LEFT
                );
        }


        else if (
            key == SDLK_RIGHT
        ) {

            moved =
                moveBoard(
                    RIGHT
                );
        }


        else if (
            key == SDLK_UP
        ) {

            moved =
                moveBoard(
                    UP
                );
        }


        else if (
            key == SDLK_DOWN
        ) {

            moved =
                moveBoard(
                    DOWN
                );
        }


        // ====================================================
        // 動いたときだけ判定
        // ====================================================

        if (moved) {

            saveBest();


            // 2048完成

            if (checkWin()) {

                WinChoice choice =
                    showWinMenu();

                if (
                    choice ==
                    WIN_QUIT
                ) {

                    running = false;

                    break;
                }
            }


            // GAME OVER

            if (!canMove()) {

                gameOver = true;

                drawGameOver();

                continue;
            }
        }


        drawBoard();
    }


    cleanup();

    return 0;
}
