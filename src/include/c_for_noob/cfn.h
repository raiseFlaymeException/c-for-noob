#ifndef CFN_H
#define CFN_H

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifndef CFN_CAPTURE_PTR_CAP_BASE
#define CFN_CAPTURE_PTR_CAP_BASE 16
#endif

struct {
    FILE *out;
    bool  print_malloc;
    bool  print_calloc;
    bool  print_realloc;
    bool  print_free;
    bool  capture_ptr;
    bool  check_memory_leak_end;
} cfn_config;

typedef struct {
    void       *mem;
    bool        free;
    const char *file;
    size_t      line;
    size_t      size;
} CdebugPtrInfo;

void *cfn_malloc(size_t size, const char *file, int line);
void *cfn_calloc(size_t size, size_t size_type, const char *file, int line);
void *cfn_realloc(void *old_mem, size_t size, const char *file, int line);
void  cfn_free(void *mem, const char *file, int line);

void cfn_capture_ptr_free(void);

void cfn_print_capture_ptr(void);

void cfn_check_memory_leak(void);

void cfn_init_default_config();
void cfn_quit();

#ifdef CFN_IMPL
#undef malloc
#undef calloc
#undef realloc
#undef free

#include <assert.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static CdebugPtrInfo *cfn_capture_ptr      = NULL;
static size_t         cfn_capture_ptr_size = 0;
static size_t         cfn_capture_ptr_cap  = CFN_CAPTURE_PTR_CAP_BASE;

static void _cfn_capture_ptr_append(void *mem, const char *file, size_t line, size_t size) {
    if (!cfn_capture_ptr) {
        cfn_capture_ptr =
            (CdebugPtrInfo *)malloc(cfn_capture_ptr_cap * sizeof(CdebugPtrInfo));
    } else if (cfn_capture_ptr_size + 1 >= cfn_capture_ptr_cap) {
        cfn_capture_ptr_cap *= 2;
        cfn_capture_ptr = (CdebugPtrInfo *)realloc(
            cfn_capture_ptr, cfn_capture_ptr_cap * sizeof(CdebugPtrInfo));
    }
    CdebugPtrInfo *capture_ptr = cfn_capture_ptr + cfn_capture_ptr_size;
    capture_ptr->mem           = mem;
    capture_ptr->free          = false;
    capture_ptr->file          = file;
    capture_ptr->line          = line;
    capture_ptr->size          = size;
    cfn_capture_ptr_size++;
}

static void _cfn_capture_ptr_append_if_not_exist(void *mem, const char *file, size_t line,
                                                    size_t size) {
    for (size_t i = 0; i < cfn_capture_ptr_size; i++) {
        if (cfn_capture_ptr[i].mem == mem) {
            cfn_capture_ptr[i].free = false;
            cfn_capture_ptr[i].line = line;
            cfn_capture_ptr[i].file = file;
            cfn_capture_ptr[i].size = size;
            return;
        }
    }
    _cfn_capture_ptr_append(mem, file, line, size);
}

void *cfn_malloc(size_t size, const char *file, int line) {
    void *mem = malloc(size);

    if (cfn_config.print_malloc) {
        fprintf(cfn_config.out,
                "CFN (malloc): allocating %zu bytes (in %s:%zu) to memory: %x\n", size, file,
                line, mem);
    }

    if (cfn_config.capture_ptr) {
        _cfn_capture_ptr_append_if_not_exist(mem, file, line, size);
    }

    return mem;
}

void *cfn_calloc(size_t size, size_t size_type, const char *file, int line) {
    void *mem = calloc(size, size_type);

    if (cfn_config.print_calloc) {
        fprintf(cfn_config.out,
                "CFN (calloc): allocating %zu bytes (in %s:%zu) to memory: %x\n",
                size * size_type, file, line, mem);
    }

    if (cfn_config.capture_ptr) {
        _cfn_capture_ptr_append_if_not_exist(mem, file, line, size * size_type);
    }

    return mem;
}

void *cfn_realloc(void *old_mem, size_t size, const char *file, int line) {
    void *mem = realloc(old_mem, size);

    if (cfn_config.print_realloc) {
        fprintf(cfn_config.out,
                "CFN (realloc): reallocating %x in %s:%zu to memory %x of size %zu\n", old_mem,
                file, line, mem, size);
    }

    if (cfn_config.capture_ptr) {
        if (old_mem == mem) {
            for (size_t i = 0; i < cfn_capture_ptr_size; i++) {
                CdebugPtrInfo *ptr_info = cfn_capture_ptr + i;
                if (ptr_info->mem == old_mem) { ptr_info->size = size; }
            }
        } else {
            for (size_t i = 0; i < cfn_capture_ptr_size; i++) {
                CdebugPtrInfo *ptr_info = cfn_capture_ptr + i;
                if (ptr_info->mem == old_mem) {
                    if (ptr_info->free) {
                        fprintf(cfn_config.out,
                                "CFN (realloc): ERROR: double free at %s:%zu"
                                "for memory %x allocated in %s:%zu with size %zu\n",
                                file, line, mem, ptr_info->file, ptr_info->line, ptr_info->size);
                    } else {
                        ptr_info->free = true;
                    }
                }
            }
            _cfn_capture_ptr_append_if_not_exist(mem, file, line, size);
        }
    }

    return mem;
}

void cfn_free(void *mem, const char *file, int line) {
    if (cfn_config.print_calloc) {
        fprintf(cfn_config.out, "CFN (free): deallocating at %x in %s:%zu\n", mem, file,
                line);
    }

    if (cfn_config.capture_ptr) {
        for (size_t i = 0; i < cfn_capture_ptr_size; i++) {
            CdebugPtrInfo *ptr_info = cfn_capture_ptr + i;
            if (ptr_info->mem == mem) {
                if (ptr_info->free) {
                    fprintf(cfn_config.out,
                            "CFN (free): ERROR: double free at %s:%zu"
                            " for memory %x allocated in %s:%zu with size %zu\n",
                            file, line, mem, ptr_info->file, ptr_info->line, ptr_info->size);
                } else {
                    ptr_info->free = true;
                }
            }
        }
    }
    free(mem);
}

void cfn_capture_ptr_free(void) { free(cfn_capture_ptr); }

void cfn_print_capture_ptr(void) {
    assert(cfn_config.capture_ptr && "capture ptr need to be enabled");

    fprintf(cfn_config.out,
            "CFN (print capture ptr):\n************************************************\n");
    fprintf(cfn_config.out, "|memory|size|free|file|line|\n");

    for (size_t i = 0; i < cfn_capture_ptr_size; i++) {
        CdebugPtrInfo *ptr_info = cfn_capture_ptr + i;
        fprintf(cfn_config.out, "|%x|%zu|%s|%s|%zu|\n", ptr_info->mem, ptr_info->size,
                (ptr_info->free) ? "true" : "false", ptr_info->file, ptr_info->line);
    }

    fprintf(cfn_config.out, "************************************************\n");
}

void cfn_check_memory_leak(void) {
    assert(cfn_config.capture_ptr && "capture ptr need to be enabled");

    size_t *memory_leaked_idx     = (size_t *)malloc(sizeof(size_t) * cfn_capture_ptr_size);
    size_t  memory_leake_idx_size = 0;

    for (size_t i = 0; i < cfn_capture_ptr_size; i++) {
        if (!cfn_capture_ptr[i].free) { memory_leaked_idx[memory_leake_idx_size++] = i; }
    }

    fprintf(cfn_config.out,
            "CFN (check memory leak):\n************************************************\n");
    for (size_t i = 0; i < memory_leake_idx_size; i++) {
        CdebugPtrInfo *ptr_info = cfn_capture_ptr + memory_leaked_idx[i];
        fprintf(cfn_config.out, "memory not freed at %s:%zu for memory %x (size=%zu)\n",
                ptr_info->file, ptr_info->line, ptr_info->mem, ptr_info->size);
    }

    fprintf(cfn_config.out, "TOTAL: %zu memory being leaked\n", memory_leake_idx_size);
    fprintf(cfn_config.out, "************************************************\n");

    free(memory_leaked_idx);
}

void cfn_init_default_config() {
    cfn_config.out                   = stdout;
    cfn_config.print_malloc          = true;
    cfn_config.print_calloc          = true;
    cfn_config.print_realloc         = true;
    cfn_config.print_free            = true;
    cfn_config.capture_ptr           = true;
    cfn_config.check_memory_leak_end = true;
}

void cfn_quit() {
    if (cfn_config.capture_ptr) {
        if (cfn_config.check_memory_leak_end) { cfn_check_memory_leak(); }
        cfn_capture_ptr_free();
    }
}

#endif // CFN_IMPL

#define malloc(size)            cfn_malloc(size, __FILE__, __LINE__)
#define calloc(size, size_type) cfn_calloc(size, size_type, __FILE__, __LINE__)
#define realloc(mem, size)      cfn_realloc(mem, size, __FILE__, __LINE__)
#define free(mem)               cfn_free(mem, __FILE__, __LINE__)

#endif // CFN_H
