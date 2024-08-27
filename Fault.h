#ifndef _FAULT_H
#define _FAULT_H
/*
这些定义在软件开发中非常有用，特别是在开发阶段进行错误检查和快速定位问题：

使用 C_ASSERT 可以在编译时检查表达式(expr)。如果表达式为假，则编译会失败，从而防止可能的运行时错误。
ASSERT 和 ASSERT_TRUE 用于运行时检查。如果检查失败（即代码运行到这些断言处并且条件不为真），FaultHandler 会被调用。在这个函数中可以执行各种错误处理操作，如记录错误信息、停止程序或其他自定义操作，便于开发者快速定位和理解问题所在。
FaultHandler 函数目前只有声明，具体实现应根据项目需求来编写，可能包括打印错误信息，中断程序执行等。
总的来说，这段代码是用来增强代码安全性和便于调试的一种常见做法。
*/
// 如果是在C++环境中使用，需要用extern "C"来避免C++的名字修饰，确保C语言链接
#ifdef __cplusplus
extern "C" {
#endif

// 编译时的数组大小检查。在Windows VC++中如果条件不满足会报 "error C2118: negative subscript" 错误，
// 在GCC中会报 "size of unnamed array is negative" 错误。
#ifndef C_ASSERT
//#define C_ASSERT(expr)  {char uname[(expr)?1:-1];uname[0]=0;}  // 原来的宏定义
#define C_ASSERT(expr)  ((void)sizeof(char[(expr) ? 1 : -1]))   // 新的宏定义，修复了GCC的警告
#endif

// 定义ASSERT宏，不带条件，直接调用FaultHandler函数处理错误
#define ASSERT() \
    FaultHandler(__FILE__, (unsigned short) __LINE__)

// 定义带条件的ASSERT_TRUE宏，只有当condition为假的时候才调用FaultHandler函数
#define ASSERT_TRUE(condition) \
    do {if (!(condition)) FaultHandler(__FILE__, (unsigned short) __LINE__);} while (0)

// 定义FaultHandler函数，用于处理所有软件断言
// @param[in] file - 发生软件断言的文件名
// @param[in] line - 发生软件断言的行号
void FaultHandler(const char* file, unsigned short line);

#ifdef __cplusplus
}
#endif

#endif 