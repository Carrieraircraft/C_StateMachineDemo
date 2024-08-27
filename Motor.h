#ifndef _MOTOR_H
#define _MOTOR_H

#include "DataTypes.h"        // 引入一个头文件，通常定义了一些数据类型
#include "StateMachine.h"     // 引入状态机相关的头文件

/*
代码详解
首先，通过#ifndef _MOTOR_H和#endif指令确保这个头文件只被引入一次，避免重复包含时的编译错误。
引入DataTypes.h可能是为了使用其中定义的数据类型，比如INT。引入StateMachine.h可能是为了使用其中定义的状态机相关的宏和函数。
定义了一个Motor结构体，这个结构体中包含一个名为currentSpeed的字段，用于存储电机的当前速度。
定义了一个名为MotorData的结构体，这是一个事件数据类型，里面包含一个名为speed的字段，用于在需要设置电机速度的事件中传递速度值。
使用EVENT_DECLARE宏定义了两个事件，一个是MTR_SetSpeed用于设置电机的速度，一个是MTR_Halt用于停止电机。这里假设EVENT_DECLARE宏用于声明与状态机相关的事件处理函数。
GET_DECLARE宏用于声明一个获取电机速度的函数。同样假设这是一个预先定义的宏，用于创建某种形式的访问器函数。
总结
这个头文件为电机模块提供了基础的数据结构和状态机事件的声明，使得代码其他部分能通过这些声明来实现对电机控制的具体逻辑。它将电机的操作抽象成状态和事件，方便集成到较大的状态机控制框架中。
*/

// 定义电机对象的结构体，存储电机的当前速度
typedef struct
{
    INT currentSpeed;        // “INT”可能在"DataTypes.h"中定义，代表一种整型数据
} Motor;

// 定义事件数据的结构体
typedef struct
{
    INT speed;               // 储存电机速度的整数
} MotorData;

// 声明状态机相关的事件, 包括设置速度和停止
EVENT_DECLARE(MTR_SetSpeed, MotorData)  // 声明一个设置电机速度的事件
EVENT_DECLARE(MTR_Halt, NoEventData)    // 声明一个停止电机的事件，没有额外数据

// 对外提供的函数声明
GET_DECLARE(MTR_GetSpeed, INT);         // 声明一个函数用于获取电机的速度

#endif // _MOTOR_H