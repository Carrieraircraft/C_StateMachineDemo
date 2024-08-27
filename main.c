#include "fb_allocator.h"          // 引入自定义内存分配器头文件
#include "StateMachine.h"          // 引入状态机管理头文件
#include "Motor.h"                 // 引入电机控制头文件
#include "CentrifugeTest.h"        // 引入离心测试头文件

/*
*主要功能：
电机控制：通过状态机管理电机的速度和停止操作。
事件处理：通过事件（如MTR_SetSpeed和MTR_Halt）驱动电机的行为。
内存管理：使用自定义的内存分配器来管理电机操作的动态数据。
代码中用到的概念：
状态机（State Machine）：用于管理系统的不同行为状态，以及在这些状态之间的转变。
动态内存分配：根据需要分配内存以存储电机状态信息。
事件驱动编程：通过发送事件来触发特定动作，使得系统对外部变化作出响应。
*/

// 定义电机对象
static Motor motorObj1;          // 创建第一个电机对象
static Motor motorObj2;          // 创建第二个电机对象

// 定义两个公共电机状态机实例
SM_DEFINE(Motor1SM, &motorObj1)  // 定义电机1的状态机
SM_DEFINE(Motor2SM, &motorObj2)  // 定义电机2的状态机

int main(void)
{
    ALLOC_Init();                // 初始化自定义内存分配器

    MotorData* data;            // 声明一个指向MotorData结构的指针

    // 创建事件数据
    data = SM_XAlloc(sizeof(MotorData)); // 从状态机分配内存
    data->speed = 100;           // 设置电机速度为100

    // 调用MTR_SetSpeed事件函数以启动动电机
    SM_Event(Motor1SM, MTR_SetSpeed, data); // 发送设置速度的事件

    // 调用MTR_SetSpeed事件函数以更改电机速度
    data = SM_XAlloc(sizeof(MotorData)); // 再次从状态机分配内存
    data->speed = 200;           // 更新电机速度为200
    SM_Event(Motor1SM, MTR_SetSpeed, data); // 发送设置新速度的事件

    // 从Motor1SM获取当前速度
    INT currentSpeed = SM_Get(Motor1SM, MTR_GetSpeed); // 获取当前速度

    // 再次停止电机将被忽略
    SM_Event(Motor1SM, MTR_Halt, NULL); // 发送停止电机的事件

    // Motor2SM 示例
    data = SM_XAlloc(sizeof(MotorData)); // 为电机2分配内存
    data->speed = 300;            // 设置电机2的速度为300
    SM_Event(Motor2SM, MTR_SetSpeed, data); // 发送设置电机2速度的事件
    SM_Event(Motor2SM, MTR_Halt, NULL); // 发送停止电机2的事件

    // 离心测试状态机的示例
    SM_Event(CentrifugeTestSM, CFG_Cancel, NULL); // 发送取消配置的事件
    SM_Event(CentrifugeTestSM, CFG_Start, NULL); // 发送启动配置的事件
    while (CFG_IsPollActive()) // 当配置轮询仍然处于活动状态时
        SM_Event(CentrifugeTestSM, CFG_Poll, NULL); // 发送轮询事件

    ALLOC_Term();              // 终止自定义内存分配器

    return 0;                 // 返回0，表示程序正常结束
}