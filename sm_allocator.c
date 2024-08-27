// 包含特定的头文件，这些是内存分配器的相关定义和操作函数
// SMALLOC allocates either a 32 or 128 byte block depending 
// on the requested size. 

#include "sm_allocator.h"
#include "x_allocator.h"

// Maximum number of blocks for each size
// 定义两种内存块的最大数量
#define MAX_32_BLOCKS   10
#define MAX_128_BLOCKS	5

// Define size of each block including meta data overhead
// 定义每种内存块的大小，包括元数据的开销
#define BLOCK_32_SIZE     32 + XALLOC_BLOCK_META_DATA_SIZE
#define BLOCK_128_SIZE    128 + XALLOC_BLOCK_META_DATA_SIZE

// Define individual fb_allocators
// 定义两种不同大小的内存分配器对象
ALLOC_DEFINE(smDataAllocator32, BLOCK_32_SIZE, MAX_32_BLOCKS)
ALLOC_DEFINE(smDataAllocator128, BLOCK_128_SIZE, MAX_128_BLOCKS)

/// 内存分配器数组，先由小到大排列
static ALLOC_Allocator* allocators[] = {
    &smDataAllocator32Obj,
    &smDataAllocator128Obj
};

// 计算内存分配器的数量
#define MAX_ALLOCATORS   (sizeof(allocators) / sizeof(allocators[0]))

// 构建一个XAllocData结构体，存储分配器数组和其数量，用于管理所有的内存分配器
static XAllocData self = { allocators, MAX_ALLOCATORS };

//----------------------------------------------------------------------------
// SMALLOC_Alloc
//----------------------------------------------------------------------------
// 分配指定大小的内存块
void* SMALLOC_Alloc(size_t size)
{
    // 调用 XALLOC_Alloc来分配指定大小的内存块
    return XALLOC_Alloc(&self, size);
}

//----------------------------------------------------------------------------
// SMALLOC_Free
//----------------------------------------------------------------------------
// 释放先前分配的内存块
void SMALLOC_Free(void* ptr)
{
    // 调用 XALLOC_Free来释放内存
    XALLOC_Free(ptr);
}

//----------------------------------------------------------------------------
// SMALLOC_Realloc
//----------------------------------------------------------------------------
// 重新分配指定大小的内存块
void* SMALLOC_Realloc(void *ptr, size_t new_size)
{
    // 调用 XALLOC_Realloc来重新分配内存块的大小
    return XALLOC_Realloc(&self, ptr, new_size);
}

//----------------------------------------------------------------------------
// SMALLOC_Calloc
//----------------------------------------------------------------------------
// 分配并清零一个内存块数组
void* SMALLOC_Calloc(size_t num, size_t size)
{
    // 调用 XALLOC_Calloc来分配特定数量和大小的内存块，并初始化为零
    return XALLOC_Calloc(&self, num, size);
}