/*
用途和解释：
SMALLOC_Alloc(size_t size)

这个函数用来分配指定size大小的内存块。size是要分配的字节总数。
SMALLOC_Free(void ptr)*

释放之前通过SMALLOC_Alloc、SMALLOC_Realloc或者SMALLOC_Calloc等函数分配的内存块。ptr是指向这块内存的指针。
*SMALLOC_Realloc(void ptr, size_t new_size)

用于改变之前分配的内存块的大小。ptr是指向旧内存块的指针，new_size是新内存块的大小。
SMALLOC_Calloc(size_t num, size_t size)

用以分配一个元素数量为num，每个元素大小为size的数组，并自动将所有元素初始化为0。
使用场景：
这样的内存分配器通常在需要封装或改进基本内存分配行为的环境中使用，可能包括跟踪内存使用、调试内存泄漏、增加特殊内存操作等功能。
*/
// 确保这个头文件只被包含一次
#ifndef _SM_ALLOCATOR_H
#define _SM_ALLOCATOR_H

#include <stddef.h>  // 包含标准定义，如 size_t

// 当使用 C++ 时，确保这些函数以 C 语言的形式被声明
#ifdef __cplusplus
extern "C" {
#endif

// 分配指定大小的内存块
void* SMALLOC_Alloc(size_t size);

// 释放之前分配的内存块
void SMALLOC_Free(void* ptr);

// 重新分配内存块到一个新的大小
void* SMALLOC_Realloc(void *ptr, size_t new_size);

// 分配一个数组，每个元素的大小为 size，并初始化为 0
void* SMALLOC_Calloc(size_t num, size_t size);

// 如果使用 C++，这会结束 extern "C" 块
#ifdef __cplusplus
}
#endif

#endif // 结束包含保护的 ifdef