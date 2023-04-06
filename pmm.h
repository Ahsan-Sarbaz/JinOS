#pragma once
#include <types.h>

// http://www.brokenthorn.com/Resources/OSDev17.html

#define PMM_BLOCKS_PER_BYTE 8
#define PMM_BLOCK_SIZE	4096
#define PMM_BLOCK_ALIGN	PMM_BLOCK_SIZE

void pmm_init(size_t memSize, uint32_t* bitmap);
void pmm_init_region(uint32_t* base, size_t size);
void pmm_deinit_region(uint32_t* base, size_t size);
void mmap_set(int bit);
void mmap_unset(int bit);
bool mmap_test(int bit);
int mmap_first_free();
int mmap_first_free_s(size_t size);
void* pmm_alloc_block();
void pmm_free_block(void* p);
void* pmm_alloc_blocks(size_t size);
void pmm_free_blocks(void* p, size_t size);
uint32_t pmm_get_memory_size();
uint32_t pmm_get_block_count();
uint32_t pmm_get_use_block_count();
uint32_t pmm_get_free_block_count();
uint32_t pmm_get_block_size();

void pmm_init_from_mbi(uint32_t* address);
