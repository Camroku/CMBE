/*
    This file is part of Toryus.

    Toryus is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version
    3 of the License, or (at your option) any later version.

    Toryus is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with Toryus. If not, see
    <https://www.gnu.org/licenses/>.
*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <stdlib.h>
#include "cmbe.h"

#define CELL_SIZE (64)
#define GRID_WIDTH ((int)(WINDOW_WIDTH / CELL_SIZE))
#define GRID_HEIGHT ((int)(WINDOW_HEIGHT / CELL_SIZE))
#define GRID_CENTER_X ((int)(WINDOW_WIDTH / 128) - 1)
#define GRID_CENTER_Y ((int)(WINDOW_HEIGHT / 128) - 1)
#define FIT_POS(pos) ((int)((pos) / CELL_SIZE) * CELL_SIZE)

SDL_Rect map;
SDL_Rect character;

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;

int main(void)
{
    if (SDL_Init(SDL_INIT_EVERYTHING) != 0)
    {
        printf("error initializing SDL: %s\n", SDL_GetError());
    }
    SDL_Window *win = SDL_CreateWindow("Cell Machine: Bread Edition", 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, 0);

    SDL_Renderer *rend = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED);
    SDL_Surface *surface;

    SDL_DisplayMode DM;
    SDL_GetCurrentDisplayMode(0, &DM);
    WINDOW_WIDTH = DM.w;
    WINDOW_HEIGHT = DM.h;

    SDL_SetWindowSize(win, WINDOW_WIDTH, WINDOW_HEIGHT);
    SDL_SetWindowFullscreen(win, SDL_WINDOW_FULLSCREEN);

    // Character
    surface = IMG_Load("textures/test.png");

    SDL_Texture *chartex = SDL_CreateTextureFromSurface(rend, surface);

    character.w = CELL_SIZE;
    character.h = CELL_SIZE;
    character.x = GRID_CENTER_X * CELL_SIZE;
    character.y = GRID_CENTER_Y * CELL_SIZE;

    // Background cell
    surface = IMG_Load("textures/cell.png");

    SDL_Texture *maptex = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    map.x = 0;
    map.y = 0;
    map.w = CELL_SIZE;
    map.h = CELL_SIZE;

    // Grid
    cmbe_cell_t *grid = malloc(sizeof(cmbe_cell_t) * GRID_WIDTH * GRID_HEIGHT);
    memset(grid, 0, sizeof(cmbe_cell_t) * GRID_WIDTH * GRID_HEIGHT);

    int close = 0;

    while (!close)
    {
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                close = 1;
                break;
            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode)
                {
                case SDL_SCANCODE_W:
                case SDL_SCANCODE_UP:
                    character.y -= CELL_SIZE;
                    break;
                case SDL_SCANCODE_A:
                case SDL_SCANCODE_LEFT:
                    character.x -= CELL_SIZE;
                    break;
                case SDL_SCANCODE_S:
                case SDL_SCANCODE_DOWN:
                    character.y += CELL_SIZE;
                    break;
                case SDL_SCANCODE_D:
                case SDL_SCANCODE_RIGHT:
                    character.x += CELL_SIZE;
                    break;
                default:
                    break;
                }
            }
        }

        // move map if character is out of screen and map is not at the end of the screen
        if (character.x < 0)
        {
            character.x = 0;
        }
        else if (character.x + character.w > WINDOW_WIDTH)
        {
            character.x = WINDOW_WIDTH - character.w;
        }
        if (character.y < 0)
        {
            character.y = 0;
        }
        else if (character.y + character.h > WINDOW_HEIGHT)
        {
            character.y = WINDOW_HEIGHT - character.h;
        }

        character.x = FIT_POS(character.x);
        character.y = FIT_POS(character.y);

        SDL_RenderClear(rend);

        // Render map
        int i, j;
        map.x = 0;
        for (i = 0; i < GRID_WIDTH * CELL_SIZE; i += 1)
        {
            map.y = 0;
            for (j = 0; j < GRID_HEIGHT * CELL_SIZE; j += 1)
            {
                SDL_RenderCopy(rend, maptex, NULL, &map);
                map.y += map.h;
            }
            map.x += map.w;
        }
        SDL_RenderCopy(rend, chartex, NULL, &character);
        SDL_RenderPresent(rend);
        SDL_Delay(1000 / 60);
    }

    SDL_DestroyTexture(chartex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
