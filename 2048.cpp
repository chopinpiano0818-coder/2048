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

const int WINDOW_W = 560;
const int WINDOW_H = 720;

const int SIZE = 4;

const int BOARD_X = 40;
const int BOARD_Y = 190;
const int TILE = 110;
const int GAP = 10;

int board[SIZE][SIZE] = {};
int score = 0;
int bestScore = 0;

bool running = true;
bool won = false;

SDL_Window* window = nullptr;
SDL_Renderer* renderer = nullptr;

TTF_Font* fontBig = nullptr;
TTF_Font* fontMedium = nullptr;
TTF_Font* fontSmall = nullptr;


// =====================================
// 色
// =====================================

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


// =====================================
// 文字表示
// =====================================

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

    SDL_Rect rect;

    rect.w = surface->w;
    rect.h = surface->h;

    if (center) {
        rect.x = x - rect.w / 2;
        rect.y = y - rect.h / 2;
    }
    else {
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


// =====================================
// 角丸っぽい四角
// =====================================

void drawRoundedRect(
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


// =====================================
// タイル描画
// =====================================

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

    int size =
        static_cast<int>(
            TILE * scale
        );

    SDL_Rect rect;

    rect.w = size;
    rect.h = size;

    rect.x =
        static_cast<int>(
            cx - size / 2.0f
        );

    rect.y =
        static_cast<int>(
            cy - size / 2.0f
        );

    drawRoundedRect(
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


// =====================================
// 背景
// =====================================

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
        40,
        35,
        fontBig,
        {119, 110, 101, 255}
    );

    SDL_Rect scoreBox =
        {300, 35, 100, 70};

    SDL_Rect bestBox =
        {415, 35, 105, 70};

    drawRoundedRect(
        scoreBox,
        {187, 173, 160, 255}
    );

    drawRoundedRect(
        bestBox,
        {187, 173, 160, 255}
    );

    drawText(
        "SCORE",
        350,
        50,
        fontSmall,
        {238, 228, 218, 255},
        true
    );

    drawText(
        to_string(score),
        350,
        78,
        fontMedium,
        {255, 255, 255, 255},
        true
    );

    drawText(
        "BEST",
        467,
        50,
        fontSmall,
        {238, 228, 218, 255},
        true
    );

    drawText(
        to_string(bestScore),
        467,
        78,
        fontMedium,
        {255, 255, 255, 255},
        true
    );

    SDL_Rect boardBack =
        {
            BOARD_X - GAP,
            BOARD_Y - GAP,
            SIZE * TILE +
            (SIZE + 1) * GAP,
            SIZE * TILE +
            (SIZE + 1) * GAP
        };

    drawRoundedRect(
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

    drawText(
        "Arrow keys: move",
        40,
        670,
        fontSmall,
        {119, 110, 101, 255}
    );

    drawText(
        "R: restart   ESC: quit",
        300,
        670,
        fontSmall,
        {119, 110, 101, 255}
    );
}


// =====================================
// 普通の盤面描画
// =====================================

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


// =====================================
// ベストスコア保存
// =====================================

void loadBest() {

    ifstream file("2048_best.txt");

    if (file) {
        file >> bestScore;
    }
}

void saveBest() {

    if (score > bestScore) {

        bestScore = score;

        ofstream file(
            "2048_best.txt"
        );

        file << bestScore;
    }
}


// =====================================
// 新しいタイル
// =====================================

void addRandomTile(bool animate = true) {

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
            rand() %
            empty.size()
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

    for (int i = 0; i <= 8; i++) {

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

                    float scale =
                        0.2f +
                        i / 8.0f * 0.8f;

                    drawTile(
                        board[yy][xx],
                        yy,
                        xx,
                        scale
                    );
                }

                else {

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


// =====================================
// 動くタイル情報
// =====================================

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


// =====================================
// 方向
// =====================================

enum Direction {

    LEFT,
    RIGHT,
    UP,
    DOWN
};


// =====================================
// 座標変換
// =====================================

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


// =====================================
// 移動
// =====================================

bool moveBoard(Direction dir) {

    int oldBoard[SIZE][SIZE];

    for (int y = 0; y < SIZE; y++) {
        for (int x = 0; x < SIZE; x++) {
            oldBoard[y][x] = board[y][x];
            board[y][x] = 0;
        }
    }

    vector<MovingTile> moving;
    vector<MergeTile> merges;

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
                i + 1 <
                (int)items.size()
                &&
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

                board[
                    target.first
                ][
                    target.second
                ] =
                    items[i].value * 2;

                score +=
                    items[i].value * 2;

                merges.push_back({
                    target.first,
                    target.second
                });

                changed = true;

                i++;
            }

            else {

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

                board[
                    target.first
                ][
                    target.second
                ] =
                    items[i].value;

                if (
                    from.first != target.first
                    ||
                    from.second != target.second
                ) {
                    changed = true;
                }
            }

            targetPos++;
        }
    }

    if (!changed) {

        for (int y = 0; y < SIZE; y++) {

            for (int x = 0; x < SIZE; x++) {

                board[y][x] =
                    oldBoard[y][x];
            }
        }

        return false;
    }

    // =================================
    // スライドアニメーション
    // =================================

    const int FRAMES = 10;

    for (int frame = 0; frame <= FRAMES; frame++) {

        float t =
            frame /
            (float)FRAMES;

        // ease-out
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

        SDL_Delay(12);
    }

    // =================================
    // 合体時ボヨン
    // =================================

    if (!merges.empty()) {

        for (int frame = 0; frame < 8; frame++) {

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

                            if (frame < 4) {

                                scale =
                                    1.0f +
                                    frame * 0.05f;
                            }

                            else {

                                scale =
                                    1.2f -
                                    (frame - 4)
                                    * 0.05f;
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

            SDL_Delay(15);
        }
    }

    saveBest();

    addRandomTile(true);

    return true;
}


// =====================================
// ゲームオーバー
// =====================================

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


// =====================================
// 勝利
// =====================================

void checkWin() {

    if (won) {
        return;
    }

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            if (board[y][x] >= 2048) {

                won = true;
                return;
            }
        }
    }
}


// =====================================
// リスタート
// =====================================

void restart() {

    for (int y = 0; y < SIZE; y++) {

        for (int x = 0; x < SIZE; x++) {

            board[y][x] = 0;
        }
    }

    score = 0;
    won = false;

    addRandomTile(false);
    addRandomTile(false);

    drawBoard();
}


// =====================================
// GAME OVER 表示
// =====================================

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
        200
    );

    SDL_Rect overlay =
        {
            BOARD_X - GAP,
            BOARD_Y - GAP,
            490,
            490
        };

    SDL_RenderFillRect(
        renderer,
        &overlay
    );

    drawText(
        "GAME OVER",
        WINDOW_W / 2,
        390,
        fontBig,
        {119, 110, 101, 255},
        true
    );

    drawText(
        "Press R to restart",
        WINDOW_W / 2,
        450,
        fontSmall,
        {119, 110, 101, 255},
        true
    );

    SDL_RenderPresent(renderer);
}


// =====================================
// 初期化
// =====================================

bool init() {

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {

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
            "2048",
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

        // Raspberry Pi環境によっては
        // accelerated renderer が使えないのでfallback
        renderer =
            SDL_CreateRenderer(
                window,
                -1,
                SDL_RENDERER_SOFTWARE
            );
    }

    string fontPath =
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf";

    fontBig =
        TTF_OpenFont(
            fontPath.c_str(),
            46
        );

    fontMedium =
        TTF_OpenFont(
            fontPath.c_str(),
            28
        );

    fontSmall =
        TTF_OpenFont(
            fontPath.c_str(),
            18
        );

    if (
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


// =====================================
// 終了
// =====================================

void cleanup() {

    saveBest();

    TTF_CloseFont(fontBig);
    TTF_CloseFont(fontMedium);
    TTF_CloseFont(fontSmall);

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);

    TTF_Quit();
    SDL_Quit();
}


// =====================================
// MAIN
// =====================================

int main() {

    srand(time(nullptr));

    loadBest();

    if (!init()) {
        return 1;
    }

    restart();

    bool gameOver = false;

    while (running) {

        SDL_Event event;

        while (SDL_PollEvent(&event)) {

            if (event.type == SDL_QUIT) {

                running = false;
            }

            if (
                event.type ==
                SDL_KEYDOWN
            ) {

                SDL_Keycode key =
                    event.key.keysym.sym;

                if (key == SDLK_ESCAPE) {

                    running = false;
                }

                else if (key == SDLK_r) {

                    restart();
                    gameOver = false;
                }

                else if (!gameOver) {

                    if (key == SDLK_LEFT) {

                        moveBoard(LEFT);
                    }

                    else if (key == SDLK_RIGHT) {

                        moveBoard(RIGHT);
                    }

                    else if (key == SDLK_UP) {

                        moveBoard(UP);
                    }

                    else if (key == SDLK_DOWN) {

                        moveBoard(DOWN);
                    }

                    checkWin();

                    if (!canMove()) {

                        gameOver = true;
                        drawGameOver();
                    }
                }
            }
        }

        if (!gameOver) {

            drawBoard();

            if (won) {

                drawText(
                    "2048!",
                    WINDOW_W / 2,
                    145,
                    fontMedium,
                    {237, 194, 46, 255},
                    true
                );

                SDL_RenderPresent(renderer);
            }
        }

        SDL_Delay(5);
    }

    cleanup();

    return 0;
}
