#include <iostream>
#include <string>
#include <sstream>
#include "include/SDL.h"
#include "include/SDL_image.h"
#include "include/SDL_ttf.h"

#define SCREEN_WIDTH   600 // Kích thước màn hình
#define SCREEN_HEIGHT  600
#define BOARD_SIZE     10   // Kích thước bàn cờ 20x20
#define CELL_SIZE      (SCREEN_WIDTH / BOARD_SIZE) // Kích thước mỗi ô

#undef main
struct App {
    SDL_Renderer *renderer;
    SDL_Window *window;
};

App app;
SDL_Texture *xTexture = nullptr;
SDL_Texture *oTexture = nullptr;
TTF_Font* font = nullptr;

int board[BOARD_SIZE][BOARD_SIZE] = {0}; // 0: ô trống, 1: đánh X, 2: đánh O
int currentPlayer = 1; // Người chơi 1 đánh X, Người chơi 2 đánh O
bool gameEnded = false;

bool checkWin(int player) {
    // Kiểm tra hàng ngang
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j <= BOARD_SIZE - 5; ++j) {
            bool win = true;
            for (int k = 0; k < 5; ++k) {
                if (board[i][j + k] != player) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }

    // Kiểm tra hàng dọc
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j <= BOARD_SIZE - 5; ++j) {
            bool win = true;
            for (int k = 0; k < 5; ++k) {
                if (board[j + k][i] != player) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }

    // Kiểm tra đường chéo chính
    for (int i = 0; i <= BOARD_SIZE - 5; ++i) {
        for (int j = 0; j <= BOARD_SIZE - 5; ++j) {
            bool win = true;
            for (int k = 0; k < 5; ++k) {
                if (board[i + k][j + k] != player) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }

    // Kiểm tra đường chéo phụ
    for (int i = 0; i <= BOARD_SIZE - 5; ++i) {
        for (int j = BOARD_SIZE - 1; j >= 4; --j) {
            bool win = true;
            for (int k = 0; k < 5; ++k) {
                if (board[i + k][j - k] != player) {
                    win = false;
                    break;
                }
            }
            if (win) return true;
        }
    }

    return false;
}

bool checkDraw() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == 0) {
                return false; // Còn ô trống
            }
        }
    }
    return true; // Hòa
}

void loadTextures() {
    SDL_Surface *xSurface = IMG_Load("close.png");
    if (!xSurface) {
        std::cerr << "Không thể tải hình ảnh X: " << IMG_GetError() << std::endl;
        exit(1);
    }
    xTexture = SDL_CreateTextureFromSurface(app.renderer, xSurface);
    SDL_FreeSurface(xSurface);

    SDL_Surface *oSurface = IMG_Load("circle-ring.png");
    if (!oSurface) {
        std::cerr << "Không thể tải hình ảnh O: " << IMG_GetError() << std::endl;
        exit(1);
    }
    oTexture = SDL_CreateTextureFromSurface(app.renderer, oSurface);
    SDL_FreeSurface(oSurface);
}

void initSDL() {
    int rendererFlags, windowFlags;

    rendererFlags = SDL_RENDERER_ACCELERATED;
    windowFlags = 0;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Không thể khởi tạo SDL: " << SDL_GetError() << std::endl;
        exit(1);
    }

    app.window = SDL_CreateWindow("Gomoku Game", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, windowFlags);
    if (!app.window) {
        std::cerr << "Không thể tạo cửa sổ " << SCREEN_WIDTH << " x " << SCREEN_HEIGHT << ": " << SDL_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "linear");

    app.renderer = SDL_CreateRenderer(app.window, -1, rendererFlags);
    if (!app.renderer) {
        std::cerr << "Không thể tạo renderer: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(app.window);
        SDL_Quit();
        exit(1);
    }

    if (!(IMG_Init(IMG_INIT_PNG) & IMG_INIT_PNG)) {
        std::cerr << "Không thể khởi tạo SDL_image: " << IMG_GetError() << std::endl;
        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        SDL_Quit();
        exit(1);
    }

    if (TTF_Init() == -1) {
        std::cerr << "Không thể khởi tạo SDL_ttf: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        SDL_Quit();
        exit(1);
    }

    font = TTF_OpenFont("flowdeco.ttf", 24); // Đường dẫn và kích thước font
    if (!font) {
        std::cerr << "Không thể tải font: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(app.renderer);
        SDL_DestroyWindow(app.window);
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    loadTextures();
}

void cleanup() {
    if (app.renderer) {
        SDL_DestroyRenderer(app.renderer);
    }
    if (app.window) {
        SDL_DestroyWindow(app.window);
    }
    if (xTexture) {
        SDL_DestroyTexture(xTexture);
    }
    if (oTexture) {
        SDL_DestroyTexture(oTexture);
    }
    if (font) {
        TTF_CloseFont(font);
    }
    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
}

void handlePlayerMove(int row, int col) {
    if (board[row][col] == 0 && !gameEnded) { // Ô trống và trò chơi chưa kết thúc
        board[row][col] = currentPlayer;
        currentPlayer = (currentPlayer == 1) ? 2 : 1; // Đổi lượt chơi

        if (checkWin(1)) {
            std::cout << "Người chơi X chiến thắng!" << std::endl;
            gameEnded = true;
        } else if (checkWin(2)) {
            std::cout << "Người chơi O chiến thắng!" << std::endl;
            gameEnded = true;
        } else if (checkDraw()) {
            std::cout << "Trò chơi hòa!" << std::endl;
            gameEnded = true;
        }

        // Kiểm tra chiến thắng, hoặc kiểm tra hòa
        // (Bạn có thể thêm logic kiểm tra chiến thắng ở đây)

        // Ví dụ đơn giản: In ra console báo người chiến thắng
        std::cout << "Đánh " << ((board[row][col] == 1) ? "X" : "O") << " tại ô [" << row << ", " << col << "]" << std::endl;
    }
}

void drawBoard() {
    SDL_SetRenderDrawColor(app.renderer, 255, 255, 255, 255); // Màu nền trắng
    SDL_RenderClear(app.renderer);

    // Vẽ lưới caro
    SDL_SetRenderDrawColor(app.renderer, 0, 0, 0, 255); // Màu đen
    for (int i = 0; i <= BOARD_SIZE; ++i) {
        SDL_RenderDrawLine(app.renderer, 0, i * CELL_SIZE, SCREEN_WIDTH, i * CELL_SIZE); // Đường ngang
        SDL_RenderDrawLine(app.renderer, i * CELL_SIZE, 0, i * CELL_SIZE, SCREEN_HEIGHT); // Đường dọc
    }

    // Vẽ các ký hiệu (X và O) đã đánh
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            if (board[i][j] == 1) {
                // Vẽ X
                SDL_Rect destRect = { j * CELL_SIZE + 10, i * CELL_SIZE + 10, CELL_SIZE - 20, CELL_SIZE - 20 };
                SDL_RenderCopy(app.renderer, xTexture, nullptr, &destRect);
            } else if (board[i][j] == 2) {
                // Vẽ O
                SDL_Rect destRect = { j * CELL_SIZE + 10, i * CELL_SIZE + 10, CELL_SIZE - 20, CELL_SIZE - 20 };
                SDL_RenderCopy(app.renderer, oTexture, nullptr, &destRect);
            }
        }
    }

    // Hiển thị lượt chơi của người chơi hiện tại
    std::string currentPlayerMsg = (currentPlayer == 1) ? "Lượt chơi: Người chơi X" : "Lượt chơi: Người chơi O";
    SDL_Color textColor = {0, 0, 0, 255}; // Màu đen
    SDL_Surface *textSurface = TTF_RenderText_Solid(font, currentPlayerMsg.c_str(), textColor);
    SDL_Texture *textTexture = SDL_CreateTextureFromSurface(app.renderer, textSurface);
    SDL_FreeSurface(textSurface);

    SDL_Rect textRect = {20, SCREEN_HEIGHT - 40, 0, 0}; // Vị trí của văn bản
    SDL_QueryTexture(textTexture, NULL, NULL, &textRect.w, &textRect.h);
    SDL_RenderCopy(app.renderer, textTexture, NULL, &textRect);
    SDL_DestroyTexture(textTexture);

    // Nếu trò chơi kết thúc, hiển thị thông báo người chiến thắng và nút chơi lại
    if (gameEnded) {
        std::string winnerMsg;
        if (checkWin(1)) {
            winnerMsg = "Người chơi X chiến thắng!";
        } else if (checkWin(2)) {
            winnerMsg = "Người chơi O chiến thắng!";
        } else {
            winnerMsg = "Trò chơi hòa!";
        }

        SDL_Surface *winSurface = TTF_RenderText_Solid(font, winnerMsg.c_str(), textColor);
        SDL_Texture *winTexture = SDL_CreateTextureFromSurface(app.renderer, winSurface);
        SDL_FreeSurface(winSurface);

        SDL_Rect winRect = {SCREEN_WIDTH / 2 - winSurface->w / 2, SCREEN_HEIGHT / 2 - winSurface->h / 2, winSurface->w, winSurface->h};
        SDL_RenderCopy(app.renderer, winTexture, NULL, &winRect);
        SDL_DestroyTexture(winTexture);

        // Vẽ nút chơi lại
        SDL_Rect restartRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + winSurface->h, 100, 50};
        SDL_SetRenderDrawColor(app.renderer, 0, 255, 0, 255); // Màu xanh lá cây
        SDL_RenderFillRect(app.renderer, &restartRect);

        SDL_Color textColorRestart = {0, 0, 0, 255}; // Màu đen
        std::string restartMsg = "Chơi lại";
        SDL_Surface *restartSurface = TTF_RenderText_Solid(font, restartMsg.c_str(), textColorRestart);
        SDL_Texture *restartTexture = SDL_CreateTextureFromSurface(app.renderer, restartSurface);
        SDL_FreeSurface(restartSurface);

        SDL_Rect textRestartRect = {restartRect.x + restartRect.w / 2 - restartSurface->w / 2, restartRect.y + restartRect.h / 2 - restartSurface->h / 2, restartSurface->w, restartSurface->h};
        SDL_RenderCopy(app.renderer, restartTexture, NULL, &textRestartRect);
        SDL_DestroyTexture(restartTexture);

        SDL_FreeSurface(textSurface);
    }

    SDL_RenderPresent(app.renderer);
}

void doInput() {
    SDL_Event event;

    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_QUIT:
                cleanup(); // Dọn dẹp và thoát chương trình
                exit(0);
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (event.button.button == SDL_BUTTON_LEFT && !gameEnded) {
                    int col = event.button.x / CELL_SIZE;
                    int row = event.button.y / CELL_SIZE;
                    handlePlayerMove(row, col);
                } else if (event.button.button == SDL_BUTTON_LEFT && gameEnded) {
                    // Xử lý nút chơi lại
                    int mouseX = event.button.x;
                    int mouseY = event.button.y;

                    SDL_Rect restartRect = {SCREEN_WIDTH / 2 - 50, SCREEN_HEIGHT / 2 + 50, 100, 50};
                    if (mouseX >= restartRect.x && mouseX <= restartRect.x + restartRect.w &&
                        mouseY >= restartRect.y && mouseY <= restartRect.y + restartRect.h) {
                        // Reset lại trò chơi
                        for (int i = 0; i < BOARD_SIZE; ++i) {
                            for (int j = 0; j < BOARD_SIZE; ++j) {
                                board[i][j] = 0;
                            }
                        }
                        currentPlayer = 1;
                        gameEnded = false;
                    }
                }
                break;
            default:
                break;
        }
    }
}

int main(int argc, char *argv[]) {
    initSDL();
    atexit(cleanup);

    while (true) {
        drawBoard();
        doInput();
        SDL_Delay(16);
    }

    return 0;
}
