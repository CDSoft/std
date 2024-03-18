#include "arena.h"

#include "test.h"

#include <stdint.h>

#define ARENA_CAPACITY (1024*1024)

static char global_buffer[sizeof(t_arena) + ARENA_CAPACITY] __attribute__((aligned(ARENA_ALIGNMENT)));

static void test_allocs(t_arena *arena, size_t size, size_t max_allocs) {
    arena_reset(arena);
    CHECK(arena->capacity == ARENA_CAPACITY, "wrong arena capacity");
    CHECK(arena->position == 0, "wrong arena initial position");
    size_t nb_allocs = 0;
    for (size_t i = 0; i < ARENA_CAPACITY; i++) {
        CHECK(arena_alloc_bytes(arena, 0) == NULL, "unexpected allocation of zero bytes");
        char *ptr = arena_alloc_bytes(arena, size);
        if (ptr == NULL) { break; }
        nb_allocs++;
        CHECK(ptr == &arena->memory[(size+(ARENA_ALIGNMENT-1))/ARENA_ALIGNMENT*ARENA_ALIGNMENT*i], "invalid allocated address");
        CHECK(ptr <= &arena->memory[ARENA_CAPACITY]-size, "invalid allocated address");
    }
    for (size_t i = 0; i < ARENA_CAPACITY; i++) {
        CHECK(arena_alloc_bytes(arena, size) == NULL, "unexpected allocation in an already full buffer");
    }
    CHECK(nb_allocs == max_allocs, "wrong number of allocations");
}

typedef struct { char foo[18]; } type_1;
typedef struct { char foo[38]; } type_2;
typedef struct { char foo[10]; } type_3;
typedef struct { uint64_t x, y; } type_4;

static void arena_test(t_arena *arena) {
    CHECK(arena != NULL, "can not allocate arena");
    CHECK(arena->capacity == ARENA_CAPACITY, "wrong arena capacity");
    CHECK(arena->position == 0, "wrong arena initial position");

    /* Test allocations of various sizes */

    test_allocs(arena,   1, ARENA_CAPACITY/16);
    test_allocs(arena,   2, ARENA_CAPACITY/16);
    test_allocs(arena,  15, ARENA_CAPACITY/16);
    test_allocs(arena,  16, ARENA_CAPACITY/16);
    test_allocs(arena,  17, ARENA_CAPACITY/32);
    test_allocs(arena,  31, ARENA_CAPACITY/32);
    test_allocs(arena,  32, ARENA_CAPACITY/32);
    test_allocs(arena,  33, ARENA_CAPACITY/48);
    test_allocs(arena,  47, ARENA_CAPACITY/48);
    test_allocs(arena,  48, ARENA_CAPACITY/48);
    test_allocs(arena,  49, ARENA_CAPACITY/64);
    test_allocs(arena,  63, ARENA_CAPACITY/64);
    test_allocs(arena,  64, ARENA_CAPACITY/64);

    test_allocs(arena,  ARENA_CAPACITY/2-1, 2);
    test_allocs(arena,  ARENA_CAPACITY/2,   2);
    test_allocs(arena,  ARENA_CAPACITY/2+1, 1);

    test_allocs(arena,  ARENA_CAPACITY-1,   1);
    test_allocs(arena,  ARENA_CAPACITY,     1);

    arena_reset(arena);
    CHECK(arena_alloc_bytes(arena, 0) == NULL, "unexpected allocation of 0 bytes");
    CHECK(arena_alloc_bytes(arena, ARENA_CAPACITY+1) == NULL, "unexpected allocation of more bytes than available");

    /* Test allocations of items of different sizes from their types */

    type_1 *item_1 = arena_alloc(arena, type_1);
    type_2 *item_2 = arena_alloc(arena, type_2);
    type_3 *item_3 = arena_alloc(arena, type_3);

    CHECK(item_1 != NULL, "unexpected allocation error");
    CHECK(item_2 != NULL, "unexpected allocation error");
    CHECK(item_3 != NULL, "unexpected allocation error");

    CHECK((char*)item_1 == &arena->memory[0], "invalid address of item_1");
    CHECK((char*)item_2 == &arena->memory[32], "invalid address of item_2");
    CHECK((char*)item_3 == &arena->memory[32 + 48], "invalid address of item_3");

    /* Test memory read/writen operations */

    arena_reset(arena);
    type_4 *ptrs[ARENA_CAPACITY/sizeof(type_4)];
    srandom(42);
    for (size_t i = 0; i < ARENA_CAPACITY/sizeof(type_4); i++) {
        ptrs[i] = arena_alloc(arena, type_4);
        CHECK(ptrs[i] != NULL, "unexpected allocation error");
        ptrs[i]->x = (uint64_t)random();
        ptrs[i]->y = (uint64_t)random();
    }
    srandom(42);
    for (size_t i = 0; i < ARENA_CAPACITY/sizeof(type_4); i++) {
        CHECK(ptrs[i]->x == (uint64_t)random(), "unexpected value stored in an allocated data");
        CHECK(ptrs[i]->y == (uint64_t)random(), "unexpected value stored in an allocated data");
    }
}

int main(void) {

    /* Tests with a statically allocated buffer */
    t_arena *static_arena = init_arena(global_buffer, sizeof(global_buffer));
    CHECK(&static_arena->memory[0] == &global_buffer[sizeof(t_arena)], "Wrong memory address");
    CHECK(&static_arena->memory[0] == &((char*)static_arena)[sizeof(t_arena)], "Wrong memory address");
    arena_test(static_arena);

    /* Tests with a dynamically allocated buffer */
    t_arena *dynamic_arena = new_arena(ARENA_CAPACITY);
    CHECK(&dynamic_arena->memory[0] == &((char*)dynamic_arena)[sizeof(t_arena)], "Wrong memory address");
    arena_test(dynamic_arena);
    free_arena(dynamic_arena);

    return test_status();
}
