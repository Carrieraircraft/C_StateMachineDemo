/*解析
SM_DECLARE(CentrifugeTestSM) - 这是一个宏调用，用于声明一个状态机的实例。CentrifugeTestSM很可能是状态机的名称，而SM_DECLARE是一个宏，用于在状态机的实现文件中创建或引用一个状态机的结构。

EVENT_DECLARE - 这是用来声明具体的状态机事件。例如，EVENT_DECLARE(CFG_Start, NoEventData)声明一个事件CFG_Start，它不携带任何数据（NoEventData）。事件用于在状态机中触发特定的行为。

BOOL CFG_IsPollActive(); - 这是一个查询函数，可能用于检查当前离心机的状态机是否在轮询某些条件或参数。

总结
此头文件是设置离心机测试相关状态机的一部分，通过引入状态管理和事件驱动的逻辑，可以帮助开发、管理复杂的动态系统。并且通过使用状态机，代码可以更为模块化，易于管理和扩展。这种方法在许多实时或嵌入式系统中非常常见，有助于确保系统响应迅速且可预测。*/
#ifndef _CENTRIFUGE_TEST_H
#define _CENTRIFUGE_TEST_H

// 引入相关的数据类型和状态机定义文件
#include "DataTypes.h"   // 可能定义了一些专门的数据类型
#include "StateMachine.h"   // 提供状态机相关的宏定义和函数接口

// 使用宏声明一个名为CentrifugeTestSM的状态机的私有实例
SM_DECLARE(CentrifugeTestSM)

// 声明三个状态机事件函数。这些事件没有附带数据。
EVENT_DECLARE(CFG_Start, NoEventData)
EVENT_DECLARE(CFG_Cancel, NoEventData)
EVENT_DECLARE(CFG_Poll, NoEventData)

// 定义一个函数，用于检查是否处于Poll状态
BOOL CFG_IsPollActive();

#endif // _CENTRIFUGE_TEST_H
