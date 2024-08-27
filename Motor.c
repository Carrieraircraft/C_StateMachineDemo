#include "Motor.h"                  // 引入电机控制的头文件
#include "StateMachine.h"           // 引入状态机管理的头文件
#include <stdio.h>                  // 引入标准输入输出库

/*
主要功能：
状态机定义：定义了电机的不同状态（如空闲、启动、停止和改变速度）以及对应的状态处理函数。
事件处理：定义了电机速度设置和停止事件的处理逻辑。
状态迁移：通过状态映射表实现了状态之间的迁移规则。
数据访问：能够获取和更新电机的速度数据。
代码中的重要概念：
状态机（State Machine）：一个用于管理状态和状态之间过渡的编程模型。
事件驱动编程：基础于事件的系统，通过触发事件来改变系统状态。
状态定义：使用STATE_DEFINE等宏来实现特定状态的处理逻辑。
*/

// 状态枚举，状态的顺序必须与状态图中的方法顺序一致
enum States
{
    ST_IDLE,                       // 空闲状态
    ST_STOP,                       // 停止状态
    ST_START,                      // 启动状态
    ST_CHANGE_SPEED,               // 改变速度状态
    ST_MAX_STATES                  // 最大状态数
};

// 状态机状态函数的声明
STATE_DECLARE(Idle, NoEventData)                // 声明Idle状态处理函数，并不需要额外数据
STATE_DECLARE(Stop, NoEventData)                // 声明Stop状态处理函数，并不需要额外数据
STATE_DECLARE(Start, MotorData)                 // 声明Start状态处理函数，需要传入MotorData类型的数据
STATE_DECLARE(ChangeSpeed, MotorData)           // 声明ChangeSpeed状态处理函数，需要传入MotorData类型的数据

// 状态映射，定义状态函数的顺序
BEGIN_STATE_MAP(Motor)
    STATE_MAP_ENTRY(ST_Idle)                   // 进入空闲状态
    STATE_MAP_ENTRY(ST_Stop)                   // 进入停止状态
    STATE_MAP_ENTRY(ST_Start)                  // 进入启动状态
    STATE_MAP_ENTRY(ST_ChangeSpeed)            // 进入改变速度状态
END_STATE_MAP(Motor)

// 设置电机速度的外部事件
EVENT_DEFINE(MTR_SetSpeed, MotorData)
{
    // 在给定的SetSpeed事件中，根据当前状态机状态过渡到新状态
    BEGIN_TRANSITION_MAP                        // - 当前状态 -
        TRANSITION_MAP_ENTRY(ST_START)          // ST_Idle       
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)     // ST_Stop       
        TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED)   // ST_Start      
        TRANSITION_MAP_ENTRY(ST_CHANGE_SPEED)   // ST_ChangeSpeed
    END_TRANSITION_MAP(Motor, pEventData)      // 结束过渡映射
}

// 使电机停止的外部事件
EVENT_DEFINE(MTR_Halt, NoEventData)
{
    // 在给定的Halt事件中，根据当前状态机状态过渡到新状态
    BEGIN_TRANSITION_MAP                        // - 当前状态 -
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)     // ST_Idle
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)     // ST_Stop
        TRANSITION_MAP_ENTRY(ST_STOP)           // ST_Start
        TRANSITION_MAP_ENTRY(ST_STOP)           // ST_ChangeSpeed
    END_TRANSITION_MAP(Motor, pEventData)      // 结束过渡映射
}

// 状态机在电机不运行时停留在这里
STATE_DEFINE(Idle, NoEventData)
{
    printf("%s ST_Idle\n", self->name);         // 输出当前状态为ST_Idle
}

// 停止电机 
STATE_DEFINE(Stop, NoEventData)
{
    // 获取实例数据指针并更新当前速度
    Motor* pInstance = SM_GetInstance(Motor);
    pInstance->currentSpeed = 0;                 // 将当前速度设为0

    // 在此处执行停止电机的处理
    printf("%s ST_Stop: %d\n", self->name, pInstance->currentSpeed);

    // 通过内部事件过渡到ST_Idle
    SM_InternalEvent(ST_IDLE, NULL);
}

// 启动电机
STATE_DEFINE(Start, MotorData)
{
    ASSERT_TRUE(pEventData);                       // 检查事件数据是否有效

    // 获取实例数据指针并更新当前速度
    Motor* pInstance = SM_GetInstance(Motor);
    pInstance->currentSpeed = pEventData->speed;  // 将电机速度设置为事件数据中的速度

    // 在此处执行启动电机的处理
    printf("%s ST_Start: %d\n", self->name, pInstance->currentSpeed);
}

// 当电机在运行状态下改变速度
STATE_DEFINE(ChangeSpeed, MotorData)
{
    ASSERT_TRUE(pEventData);                       // 检查事件数据是否有效

    // 获取实例数据指针并更新当前速度
    Motor* pInstance = SM_GetInstance(Motor);
    pInstance->currentSpeed = pEventData->speed;  // 将电机速度更新为事件数据中的速度

    // 在此处执行修改电机速度的处理
    printf("%s ST_ChangeSpeed: %d\n", self->name, pInstance->currentSpeed);
}

// 获取当前速度的事件定义
GET_DEFINE(MTR_GetSpeed, INT)
{
    Motor* pInstance = SM_GetInstance(Motor);     // 获取电机实例
    return pInstance->currentSpeed;                // 返回当前速度
}