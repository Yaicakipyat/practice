#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

int main()
{
    std::cout << " ██████╗  █████╗ ██╗      █████╗  ██████╗  █████╗ " << std::endl;
    std::cout << "██╔════╝ ██╔══██╗██║     ██╔══██╗██╔════╝ ██╔══██╗" << std::endl;
    std::cout << "██║  ███╗███████║██║     ███████║██║  ███╗███████║" << std::endl;
    std::cout << "██║   ██║██╔══██║██║     ██╔══██║██║   ██║██╔══██║" << std::endl;
    std::cout << "╚██████╔╝██║  ██║███████╗██║  ██║╚██████╔╝██║  ██║" << std::endl;
    std::cout << " ╚═════╝ ╚═╝  ╚═╝╚══════╝╚═╝  ╚═╝ ╚═════╝ ╚═╝  ╚═╝ " << std::endl;

    std::cout << std::endl << std::endl;

    std::cout << "Добро пожаловать в text-galaga!" << std::endl;
    std::cout << "_________________________" << std::endl;
    std::cout << "Используйте W для движения вперёд," << std::endl;
    std::cout << "A для движения влево, S для движения назад," << std::endl;
    std::cout << "D для движения вправо и J для стрельбы" << std::endl;
    std::cout << "_________________________" << std::endl;

    std::cout << "Удачи!" << std::endl << std::endl;
        // 1. Инициализация SDL и SDL_ttf
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        std::cout << "SDL_Init Error: " << SDL_GetError() << std::endl;
        return 1;
    }
    if (TTF_Init() == -1) {
        std::cout << "TTF_Init Error: " << TTF_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    // 2. Создание окна и рендерера
    SDL_Window* window = SDL_CreateWindow("Текст в SDL2",
                                          SDL_WINDOWPOS_CENTERED,
                                          SDL_WINDOWPOS_CENTERED,
                                          800, 600,
                                          SDL_WINDOW_SHOWN);
    if (!window) {
        std::cout << "SDL_CreateWindow Error: " << SDL_GetError() << std::endl;
        TTF_Quit(); SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        std::cout << "SDL_CreateRenderer Error: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window); TTF_Quit(); SDL_Quit();
        return 1;
    }

    // 3. Загрузка шрифта (укажите путь к вашему .ttf файлу)
    TTF_Font* font = TTF_OpenFont("ARCADECLASSIC.TTF", 48); // Здесь путь к шрифту и размер
    if (!font) {
        std::cout << "TTF_OpenFont Error: " << TTF_GetError() << std::endl;
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); TTF_Quit(); SDL_Quit();
        return 1;
    }

    // 4. Создание текста
    SDL_Color textColor = {0, 255, 0, 255}; // Зелёный цвет (R,G,B,A)
    SDL_Surface* textSurface = TTF_RenderText_Blended(font, "WELCOME    TO  DIMA     STEPA   MAXIM", textColor);
    if (!textSurface) {
        std::cout << "TTF_RenderText_Blended Error: " << TTF_GetError() << std::endl;
        TTF_CloseFont(font);
        SDL_DestroyRenderer(renderer); SDL_DestroyWindow(window); TTF_Quit(); SDL_Quit();
        return 1;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_FreeSurface(textSurface); // Поверхность больше не нужна

    // Определяем размеры и позицию текста
    int texW = 0, texH = 0;
    SDL_QueryTexture(textTexture, NULL, NULL, &texW, &texH);
    SDL_Rect textRect = { (800 - texW) / 2, (600 - texH) / 2, texW, texH };

    // 5. Главный цикл
    bool quit = false;
    SDL_Event event;
    while (!quit) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) quit = true;
        }

        // Рисуем фон (синий) и текст
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, textTexture, NULL, &textRect);
        SDL_RenderPresent(renderer);
    }

    // 6. Очистка ресурсов
    SDL_DestroyTexture(textTexture);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;

    return 0;
}
