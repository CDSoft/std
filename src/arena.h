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

#pragma once

/*@@@
# Arena allocator

An arena allocator is a memory allocator where items can be allocated many times
but the whole allocated memory is freed once.

The allocator takes memory from a statically or dynamically allocated buffer.
@@@*/

#include <stdlib.h>

#define ARENA_ALIGNMENT (2*sizeof(size_t))

typedef struct {
    size_t capacity;
    size_t position;
    char memory[];      /* must be aligned according to ARENA_ALIGNMENT */
} t_arena;

#ifdef USE_MALLOC

t_arena *new_arena(size_t capacity);
/*@@@
``` c
t_arena *new_arena(size_t capacity); // only if USE_MALLOC is defined
```
`new_arena` creates a new arena allocator with an internal buffer of `capacity` bytes.
The buffer is dynamically allocated with `malloc` and shall be freed with `free_arena`.

**Note**: `new_arena` requires `malloc` and is not defined by default.
To enable it, the preprocessor symbol `USE_MALLOC` shall be defined.
@@@*/

void free_arena(t_arena *arena);
/*@@@
``` c
void free_arena(t_arena *arena); // only if USE_MALLOC is defined
```
`free_arena` frees an arena allocator created with `new_arena`.
@@@*/

#endif

t_arena *init_arena(char * __attribute__((aligned(sizeof(t_arena)))) buffer, size_t size);
/*@@@
``` c
t_arena *init_arena(char *buffer, size_t size);
```
`init_arena` initializes an arena allocator inside an existing buffer (e.g. a statically allocated buffer).
The actual capacity of the allocator is `size - 16`
(16 bytes are reserved at the beginning of `buffer` to store the state of the allocator).
@@@*/

#define arena_alloc(arena, item_type) ((item_type*)arena_alloc_bytes((arena), sizeof(item_type)))
/*@@@
``` c
item_type *arena_alloc(t_arena *arena, item_type);
```
`arena_alloc` allocates `sizeof(item_type)` bytes in the arena allocator buffer.
If returns a pointer the allocated item or `NULL` if there is not enough free space.
The item address is a multiple of 16.

**Note**: `arena_alloc` is not a function but a macro that calls `arena_alloc_bytes`
and forces to returned pointer type to `item_type*`{.c} instead of `void*`{.c}.
@@@*/

void *arena_alloc_bytes(t_arena *arena, size_t item_size);
/*@@@
``` c
void *arena_alloc_bytes(t_arena *arena, size_t item_size);
```
`arena_alloc_bytes` allocates `item_size` bytes in the arena allocator buffer.
If returns a pointer the allocated item or `NULL` if there is not enough free space.
The item address is a multiple of 16.
@@@*/

void arena_reset(t_arena *arena);
/*@@@
``` c
void arena_reset(t_arena *arena);
```
`arena_reset` frees the whole buffer at once.
The new empty allocator can be reused to allocate new items.
@@@*/
