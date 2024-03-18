# Arena allocator

An arena allocator is a memory allocator where items can be allocated
many times but the whole allocated memory is freed once.

The allocator takes memory from a statically or dynamically allocated
buffer.

``` c
t_arena *new_arena(size_t capacity); // only if USE_MALLOC is defined
```

`new_arena` creates a new arena allocator with an internal buffer of
`capacity` bytes. The buffer is dynamically allocated with `malloc` and
shall be freed with `free_arena`.

**Note**: `new_arena` requires `malloc` and is not defined by default.
To enable it, the preprocessor symbol `USE_MALLOC` shall be defined.

``` c
void free_arena(t_arena *arena); // only if USE_MALLOC is defined
```

`free_arena` frees an arena allocator created with `new_arena`.

``` c
t_arena *init_arena(char *buffer, size_t size);
```

`init_arena` initializes an arena allocator inside an existing buffer
(e.g. a statically allocated buffer). The actual capacity of the
allocator is `size - 16` (16 bytes are reserved at the beginning of
`buffer` to store the state of the allocator).

``` c
item_type *arena_alloc(t_arena *arena, item_type);
```

`arena_alloc` allocates `sizeof(item_type)` bytes in the arena allocator
buffer. If returns a pointer the allocated item or `NULL` if there is
not enough free space. The item address is a multiple of 16.

**Note**: `arena_alloc` is not a function but a macro that calls
`arena_alloc_bytes` and forces to returned pointer type to `item_type*`
instead of `void*`.

``` c
void *arena_alloc_bytes(t_arena *arena, size_t item_size);
```

`arena_alloc_bytes` allocates `item_size` bytes in the arena allocator
buffer. If returns a pointer the allocated item or `NULL` if there is
not enough free space. The item address is a multiple of 16.

``` c
void arena_reset(t_arena *arena);
```

`arena_reset` frees the whole buffer at once. The new empty allocator
can be reused to allocate new items.
