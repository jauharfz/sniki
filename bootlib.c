#include "bootlib.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

#undef malloc
#undef free
#undef calloc
#undef realloc

typedef struct {
    void *ptr;
    size_t size;
    bool is_freed;
} AllocRecord;

#define MAX_RECORDS 20000
static AllocRecord records[MAX_RECORDS];
static size_t record_count = 0;

void *boot_malloc(size_t size) {
    void *p = malloc(size);
    if (p && record_count < MAX_RECORDS) {
        records[record_count++] = (AllocRecord){ .ptr = p, .size = size, .is_freed = false };
    }
    return p;
}

void *boot_calloc(size_t num, size_t size) {
    void *p = calloc(num, size);
    if (p && record_count < MAX_RECORDS) {
        records[record_count++] = (AllocRecord){ .ptr = p, .size = num * size, .is_freed = false };
    }
    return p;
}

void *boot_realloc(void *ptr, size_t size) {
    void *p = realloc(ptr, size);
    if (ptr) {
        for (size_t i = 0; i < record_count; i++) {
            if (records[i].ptr == ptr) {
                records[i].is_freed = true;
            }
        }
    }
    if (p && record_count < MAX_RECORDS) {
        records[record_count++] = (AllocRecord){ .ptr = p, .size = size, .is_freed = false };
    }
    return p;
}

void boot_free(void *ptr) {
    if (!ptr) return;
    for (size_t i = 0; i < record_count; i++) {
        if (records[i].ptr == ptr && !records[i].is_freed) {
            records[i].is_freed = true;
            break;
        }
    }
    free(ptr);
}

bool boot_is_freed(void *ptr) {
    if (!ptr) return true;
    for (size_t i = 0; i < record_count; i++) {
        if (records[i].ptr == ptr) {
            return records[i].is_freed;
        }
    }
    return true;
}

bool boot_all_freed(void) {
    bool all_freed = true;
    size_t leak_count = 0;

    for (size_t i = 0; i < record_count; i++) {
        if (!records[i].is_freed) {
            printf("\n [MEMORY LEAK] Pointer %p (size %zu bytes) has not been free",
                   records[i].ptr, records[i].size);
            all_freed = false;
            leak_count++;
        }
    }

    if (!all_freed) {
        printf("\n❌ Total %zu memory leaks!\n", leak_count);
    }

// Reset tracking so the next test starts from zero
    record_count = 0;
    return all_freed;
}
