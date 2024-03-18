/* This file is part of std.
 *
 * std is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * std is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with std.  If not, see <https://www.gnu.org/licenses/>.
 *
 * For further information about std you can visit
 * http://cdelord.fr/std
 */

#include "arena.h"

#ifdef USE_MALLOC

t_arena *new_arena(size_t capacity) {
    t_arena *arena = (t_arena *)malloc(sizeof(t_arena) + capacity);
    if (arena != NULL) {
        arena->capacity = capacity;
        arena->position = 0;
    }
    return arena;
}

void free_arena(t_arena *arena) {
    free(arena);
}

#endif

t_arena *init_arena(char * __attribute__((aligned(sizeof(t_arena)))) buffer, size_t size) {
    if (buffer == NULL || size <= sizeof(t_arena)) { return NULL; }
    t_arena *arena = (t_arena *)__builtin_assume_aligned(buffer, sizeof(t_arena));
    arena->capacity = size - sizeof(t_arena);
    arena->position = 0;
    return arena;
}

void *arena_alloc_bytes(t_arena *arena, size_t item_size) {
    if (arena == NULL) { return NULL; }
    if (item_size == 0) { return NULL; }
    const size_t position = (arena->position + (ARENA_ALIGNMENT-1)) & ~(ARENA_ALIGNMENT-1);
    const size_t next_position = (size_t)position + item_size;
    if (next_position > arena->capacity) { return NULL; }
    arena->position = next_position;
    return &arena->memory[position];
}

void arena_reset(t_arena *arena) {
    if (arena != NULL) {
        arena->position = 0;
    }
}
