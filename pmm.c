#include "pmm.h"
#include "multiboot.h"

uint32_t _pmm_memory_size = 0;
uint32_t _pmm_used_blocks = 0;
uint32_t _pmm_max_blocks = 0;
uint32_t *_pmm_memory_map = 0;

void pmm_init_from_mbi(uint32_t* address)
{
    multiboot_info_t* mbi = (multiboot_info_t*)address;
    uint32_t totalMemorySize = 1024 + mbi->mem_lower + mbi->mem_upper;
    // init and place the map at 3MB point
    pmm_init(totalMemorySize, (uint32_t*)0x300000);

    if (CHECK_FLAG (mbi->flags, 6))
    {
        multiboot_memory_map_t *mmap;

        for (mmap = (multiboot_memory_map_t *) mbi->mmap_addr;
           (unsigned long) mmap < mbi->mmap_addr + mbi->mmap_length;
           mmap = (multiboot_memory_map_t *) ((unsigned long) mmap
                                    + mmap->size + sizeof (mmap->size)))
        {
            if(mmap->type == MULTIBOOT_MEMORY_AVAILABLE)
            {
                uint32_t* base = (uint32_t*)((uint32_t)(mmap->addr & 0xffffffff));
                size_t size = (size_t)(mmap->len & 0xffffffff);
                pmm_init_region(base, size);
            }
        }
    }
	// deinit the region the kernel is in as its in use
    // this should be checked if its correct address and size
    pmm_deinit_region((uint32_t*)0x100000, 0x300000);

}

void pmm_init(size_t memSize, uint32_t *bitmap)
{
    _pmm_memory_size = memSize;
    _pmm_memory_map = (uint32_t *)bitmap;
    _pmm_max_blocks = (pmm_get_memory_size() * 1024) / PMM_BLOCK_SIZE;
    _pmm_used_blocks = pmm_get_block_count();

    // By default, all of memory is in use
    { // memset copied here to avoid an include
        int count = pmm_get_block_count() / PMM_BLOCKS_PER_BYTE;
        char *temp = (char *)_pmm_memory_map;
        for (; count != 0; count--)
            *temp++ = 0xF;
    }
}

void pmm_init_region(uint32_t *base, size_t size)
{
    int align = (uint32_t)base / PMM_BLOCK_SIZE;
    int blocks = size / PMM_BLOCK_SIZE;

    for (; blocks > 0; blocks--)
    {
        mmap_unset(align++);
        _pmm_used_blocks--;
    }

    mmap_set(0); // first block is always set. This insures allocs cant be 0
}

void pmm_deinit_region(uint32_t *base, size_t size)
{
    int align = (uint32_t)base / PMM_BLOCK_SIZE;
    int blocks = size / PMM_BLOCK_SIZE;

    for (; blocks > 0; blocks--)
    {
        mmap_set(align++);
        _pmm_used_blocks++;
    }
}

inline void mmap_set(int bit)
{
    _pmm_memory_map[bit / 32] |= (1 << (bit % 32));
}

inline void mmap_unset(int bit)
{
    _pmm_memory_map[bit / 32] &= ~(1 << (bit % 32));
}

inline bool mmap_test(int bit)
{
    return _pmm_memory_map[bit / 32] & (1 << (bit % 32));
}

int mmap_first_free()
{
    //! find the first free bit
    for (uint32_t i = 0; i < pmm_get_block_count() / 32; i++)
    {
        if (_pmm_memory_map[i] != 0xffffffff)
        {
            for (int j = 0; j < 32; j++)
            { //! test each bit in the dword
                int bit = 1 << j;
                if (!(_pmm_memory_map[i] & bit))
                    return i * 4 * 8 + j;
            }
        }
    }

    return -1;
}

int mmap_first_free_s(size_t size)
{
    if (size == 0)
        return -1;

    if (size == 1)
        return mmap_first_free();

    for (uint32_t i = 0; i < pmm_get_block_count() / 32; i++)
    {
        if (_pmm_memory_map[i] != 0xffffffff)
        {
            for (int j = 0; j < 32; j++)
            { //! test each bit in the dword

                int bit = 1 << j;
                if (!(_pmm_memory_map[i] & bit))
                {

                    int startingBit = i * 32;
                    startingBit += bit; // get the free bit in the dword at index i

                    uint32_t free = 0; // loop through each bit to see if its enough space
                    for (uint32_t count = 0; count <= size; count++)
                    {

                        if (!mmap_test(startingBit + count))
                            free++; // this bit is clear (free frame)

                        if (free == size)
                            return i * 4 * 8 + j; // free count==size needed; return index
                    }
                }
            }
        }
    }
    return -1;
}

void* pmm_alloc_block()
{

    if (pmm_get_free_block_count() <= 0)
        return 0; // out of memory

    int frame = mmap_first_free();

    if (frame == -1)
        return 0; // out of memory

    mmap_set(frame);

    uint32_t *addr = (uint32_t*)((uint32_t)frame * PMM_BLOCK_SIZE);
    _pmm_used_blocks++;

    return (void *)addr;
}

void pmm_free_block(void *p)
{
    uint32_t *addr = (uint32_t *)p;
    int frame = (uint32_t)addr / PMM_BLOCK_SIZE;

    mmap_unset(frame);

    _pmm_used_blocks--;
}

void* pmm_alloc_blocks(size_t size)
{

    if (pmm_get_free_block_count() <= size)
        return 0; // not enough space

    int frame = mmap_first_free_s(size);

    if (frame == -1)
        return 0; // not enough space

    for (uint32_t i = 0; i < size; i++)
        mmap_set(frame + i);

    uint32_t *addr = (uint32_t*)((uint32_t)frame * PMM_BLOCK_SIZE);
    _pmm_used_blocks += size;

    return (void *)addr;
}

void pmm_free_blocks(void *p, size_t size)
{

    uint32_t *addr = (uint32_t *)p;
    int frame = (uint32_t)addr / PMM_BLOCK_SIZE;

    for (uint32_t i = 0; i < size; i++)
        mmap_unset(frame + i);

    _pmm_used_blocks -= size;
}

uint32_t pmm_get_memory_size()
{
    return _pmm_memory_size;
}

uint32_t pmm_get_block_count()
{
    return _pmm_max_blocks;
}

uint32_t pmm_get_use_block_count()
{
    return _pmm_used_blocks;
}

uint32_t pmm_get_free_block_count()
{
    return _pmm_max_blocks - _pmm_used_blocks;
}

uint32_t pmm_get_block_size()
{
    return PMM_BLOCK_SIZE;
}
