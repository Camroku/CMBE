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

#pragma once

// The FPS limit, 0 for disabled.
#define FPS_LIMIT 144
// Size of every cell
#define CELL_SIZE (64)
// Size and center of the grid
#define GRID_WIDTH ((int)(WINDOW_WIDTH / CELL_SIZE))
#define GRID_HEIGHT ((int)(WINDOW_HEIGHT / CELL_SIZE))
#define GRID_CENTER_X ((int)(WINDOW_WIDTH / 128) - 1)
#define GRID_CENTER_Y ((int)(WINDOW_HEIGHT / 128) - 1)
// Fit a position to the grid
#define FIT_POS(pos) ((int)((pos) / CELL_SIZE) * CELL_SIZE)
// Macros to check collisions
#define CHECKCOL_XY_XYXY(x1, y1, x2, y2, x3, y3) (x1 >= x2 && y1 >= y2 && x1 <= x3 && y1 <= y3)
// Get cell index by x & y collision
#define GET_CI_XY(x, y) ((IS_IN_GRID_X(x) ? (int)((x) / CELL_SIZE) : (int)((x) / CELL_SIZE - 1)) + (IS_IN_GRID_Y(y) ? (int)((y) / CELL_SIZE) * GRID_WIDTH : (int)((y) / CELL_SIZE - 1) * GRID_WIDTH))
// Check if x and y are in the grid
#define IS_IN_GRID_X(x) (x >= 0 && x < GRID_WIDTH * CELL_SIZE)
#define IS_IN_GRID_Y(y) (y >= 0 && y < GRID_HEIGHT * CELL_SIZE)

typedef enum
{
    NOTHING,
    MOVER_CELL,
    GENERATOR_CELL,
    ROTATOR_CELL_CW,
    ROTATOR_CELL_CCW,
    PUSH_CELL,
    SLIDE_CELL,
    ENEMY_CELL,
    IMMOBILE_CELL,
    TRASH_CELL,
} cmbe_cell_t;
