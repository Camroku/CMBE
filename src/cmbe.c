/*
    This file is part of Cell Machine: Bread Edition.

    Cell Machine: Bread Edition is free software: you can redistribute it and/or
    modify it under the terms of the GNU General Public License
    as published by the Free Software Foundation, either version
    3 of the License, or (at your option) any later version.

    Cell Machine: Bread Edition is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty
    of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
    See the GNU General Public License for more details.

    You should have received a copy of the GNU General Public
    License along with Cell Machine: Bread Edition. If not, see
    <https://www.gnu.org/licenses/>.
*/

/*
TODO: Parse V1 levels

Example:
V1;20;20;;0.0.1.19,3.0.2.19,1.0.3.19,2.0.4.19,5.0.5.19,4.0.6.19,7.0.7.19,8.0.8.19,6.0.9.19;;
(20x20, first cell is empty, the other 9 cells go as GENERATOR_CELL, MOVER_CELL, ROTATOR_CELL_CW,
ROTATOR_CELL_CCW, PUSH_CELL, SLIDE_CELL, ENEMY_CELL, TRASH_CELL, IMMOBILE_CELL, the other cells
are empty)

Explanation for V1:
version;GRID_WIDTH;GRID_HEIGHT;placeable_data;level_data;level_title;
version: Version of level code (V1 in this case)
placeable_data: X and Y coordinations of placeables (the +): X.Y,X.Y,X.Y,...
level_data: Data for the cells: CELL_ID.ROTATION.X.Y,CELL_ID.ROTATION.X.Y,...
level_title: Title of the level
*/

#include <stdio.h>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_timer.h>
#include <stdlib.h>
#include <stdbool.h>
#include "cmbe.h"
#include <unistd.h>

SDL_Rect cell;
SDL_Rect character;

int WINDOW_WIDTH = 640;
int WINDOW_HEIGHT = 480;

int main(void)
{
    sleep(1); // Sleep 1 seconds, because the game starts so fast that I can't change the workspace.
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
    SDL_Texture *chartex;

    character.w = CELL_SIZE;
    character.h = CELL_SIZE;
    character.x = GRID_CENTER_X * CELL_SIZE;
    character.y = GRID_CENTER_Y * CELL_SIZE;

    // Cells
    surface = IMG_Load("textures/NOTHING.png");
    SDL_Texture *tex_NOTHING = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/MOVER_CELL.png");
    SDL_Texture *tex_MOVER_CELL = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/GENERATOR_CELL.png");
    SDL_Texture *tex_GENERATOR_CELL = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/ROTATOR_CELL_CW.png");
    SDL_Texture *tex_ROTATOR_CELL_CW = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/ROTATOR_CELL_CCW.png");
    SDL_Texture *tex_ROTATOR_CELL_CCW = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/PUSH_CELL.png");
    SDL_Texture *tex_PUSH_CELL = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/SLIDE_CELL.png");
    SDL_Texture *tex_SLIDE_CELL = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/ENEMY_CELL.png");
    SDL_Texture *tex_ENEMY_CELL = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/IMMOBILE_CELL.png");
    SDL_Texture *tex_IMMOBILE_CELL = SDL_CreateTextureFromSurface(rend, surface);
    surface = IMG_Load("textures/TRASH_CELL.png");
    SDL_Texture *tex_TRASH_CELL = SDL_CreateTextureFromSurface(rend, surface);
    SDL_FreeSurface(surface);

    SDL_Texture *cell_map[10] = {
        tex_NOTHING,
        tex_MOVER_CELL,
        tex_GENERATOR_CELL,
        tex_ROTATOR_CELL_CW,
        tex_ROTATOR_CELL_CCW,
        tex_PUSH_CELL,
        tex_SLIDE_CELL,
        tex_ENEMY_CELL,
        tex_IMMOBILE_CELL,
        tex_TRASH_CELL,
    };

    // Current cell
    cell.x = 0;
    cell.y = 0;
    cell.w = CELL_SIZE;
    cell.h = CELL_SIZE;

    // Grid
    char grid[GRID_WIDTH * GRID_HEIGHT];
    memset(grid, 0, GRID_WIDTH * GRID_HEIGHT * sizeof(char));

    // For testing
    grid[0] = 0;
    grid[1] = 1;
    grid[2] = 2;
    grid[3] = 3;
    grid[4] = 4;
    grid[5] = 5;
    grid[6] = 6;
    grid[7] = 7;
    grid[8] = 8;
    grid[9] = 9;

    bool close = false;
    bool grabbing = false;
    int grx, gry;
    int mouse_x, mouse_y;
    char grabbed_cell = 0;
    char grabbed_rot = 0;
    char grabbed_old_index = 0;

    while (!close)
    {
        SDL_GetMouseState(&mouse_x, &mouse_y);
        SDL_Event event;
        while (SDL_PollEvent(&event))
        {
            switch (event.type)
            {
            case SDL_QUIT:
                close = true;
                break;
            case SDL_MOUSEBUTTONDOWN:
                if (grid[GET_CI_XY(mouse_x, mouse_y)] != 0)
                {
                    grabbing = true;
                    grx = mouse_x - FIT_POS(mouse_x);
                    gry = mouse_y - FIT_POS(mouse_y);
                    grabbed_old_index = GET_CI_XY(mouse_x, mouse_y);
                    grabbed_cell = grid[(int)grabbed_old_index] & 0b00111111;
                    grabbed_rot = grid[(int)grabbed_old_index] >> 6;
                    chartex = cell_map[(int)grabbed_cell];
                    grid[(int)grabbed_old_index] = 0;
                }
                break;
            case SDL_MOUSEBUTTONUP:
                if (grabbing)
                {
                    if (grid[GET_CI_XY(mouse_x, mouse_y)] != 0)
                    {
                        grid[(int)grabbed_old_index] = grid[GET_CI_XY(mouse_x, mouse_y)];
                    }
                    grid[GET_CI_XY(mouse_x, mouse_y)] = grabbed_cell | (grabbed_rot << 6);
                    grabbed_cell = 0;
                    grabbed_old_index = 0;
                    grabbed_rot = 0;
                    chartex = NULL;
                }
                grabbing = 0;
                break;
            case SDL_KEYDOWN:
                if (event.key.keysym.scancode == SDL_SCANCODE_Q)
                {
                    if (grabbing)
                    {
                        grabbed_rot--;
                        if (grabbed_rot == -1)
                            grabbed_rot = 3;
                    }
                }
                if (event.key.keysym.scancode == SDL_SCANCODE_E)
                {
                    if (grabbing)
                    {
                        grabbed_rot++;
                        if (grabbed_rot == 4)
                            grabbed_rot = 0;
                    }
                }
                break;
            }
        }

        if (grabbing)
        {
            character.x = mouse_x - grx;
            character.y = mouse_y - gry;
        }

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

        SDL_SetRenderDrawColor(rend, 41, 41, 41, 255);
        SDL_RenderClear(rend);

        // Render cells
        int i, j;
        cell.y = 0;
        for (i = 0; i < GRID_HEIGHT; i++)
        {
            cell.x = 0;
            for (j = 0; j < GRID_WIDTH; j++)
            {
                SDL_RenderCopyEx(rend, cell_map[(int)grid[j + i * GRID_WIDTH] & 0b00111111], NULL, &cell, 90 * (grid[j + i * GRID_WIDTH] >> 6), NULL, SDL_FLIP_NONE);
                cell.x += cell.w;
            }
            cell.y += cell.h;
        }

        SDL_RenderCopyEx(rend, chartex, NULL, &character, 90 * grabbed_rot, NULL, SDL_FLIP_NONE);
        SDL_RenderPresent(rend);
        if (FPS_LIMIT)
            SDL_Delay(1000 / FPS_LIMIT); // FPS limit
    }

    SDL_DestroyTexture(chartex);
    SDL_DestroyRenderer(rend);
    SDL_DestroyWindow(win);
    SDL_Quit();
    return 0;
}
