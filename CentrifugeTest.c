#include "CentrifugeTest.h"
#include "StateMachine.h"
#include <stdio.h>

// 定义离心机测试对象的结构体
typedef struct
{
    INT speed;       // 离心机的转速
    BOOL pollActive; // 轮询活动状态标志
} CentrifugeTest;

// 定义离心机测试对象的实例
CentrifugeTest centrifugeTestObj;

// 声明离心机的状态机，和离心机测试对象关联
SM_DEFINE(CentrifugeTestSM, &centrifugeTestObj)

// 定义状态枚举，各状态顺序需与状态处理函数映射表中的顺序一致
enum States
{
    ST_IDLE,
    ST_COMPLETED,
    ST_FAILED,
    ST_START_TEST,
    ST_ACCELERATION,
    ST_WAIT_FOR_ACCELERATION,
    ST_DECELERATION,
    ST_WAIT_FOR_DECELERATION,
    ST_MAX_STATES
};

// 各状态对应的处理函数声明
STATE_DECLARE(Idle, NoEventData)
ENTRY_DECLARE(Idle, NoEventData)
STATE_DECLARE(Completed, NoEventData)
STATE_DECLARE(Failed, NoEventData)
STATE_DECLARE(StartTest, NoEventData)
GUARD_DECLARE(StartTest, NoEventData)
STATE_DECLARE(Acceleration, NoEventData)
STATE_DECLARE(WaitForAcceleration, NoEventData)
EXIT_DECLARE(WaitForAcceleration)
STATE_DECLARE(Deceleration, NoEventData)
STATE_DECLARE(WaitForDeceleration, NoEventData)
EXIT_DECLARE(WaitForDeceleration)

// 状态函数映射表，定义各状态对应的入口、退出和中间处理函数
BEGIN_STATE_MAP_EX(CentrifugeTest)
    STATE_MAP_ENTRY_ALL_EX(ST_Idle, 0, EN_Idle, 0)
    STATE_MAP_ENTRY_EX(ST_Completed)
    STATE_MAP_ENTRY_EX(ST_Failed)
    STATE_MAP_ENTRY_ALL_EX(ST_StartTest, GD_StartTest, 0, 0)
    STATE_MAP_ENTRY_EX(ST_Acceleration)
    STATE_MAP_ENTRY_ALL_EX(ST_WaitForAcceleration, 0, 0, EX_WaitForAcceleration)
    STATE_MAP_ENTRY_EX(ST_Deceleration)
    STATE_MAP_ENTRY_ALL_EX(ST_WaitForDeceleration, 0, 0, EX_WaitForDeceleration)
END_STATE_MAP_EX(CentrifugeTest)

// 定义CFG_Start事件的处理逻辑
EVENT_DEFINE(CFG_Start, NoEventData)
{
    BEGIN_TRANSITION_MAP                                // 开始定义状态转换映射
        TRANSITION_MAP_ENTRY(ST_START_TEST)             // 当处于ST_IDLE时跳转到ST_START_TEST
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)             // 其他状态，此事件不应该发生
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)             
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             // 如果已在ST_START_TEST或后续状态中，忽略此事件
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             
    END_TRANSITION_MAP(CentrifugeTest, pEventData)      // 结束状态转换映射
}

/*
EVENT_DEFINE(CFG_Cancel, NoEventData) – 定义一个名为 "CFG_Cancel" 的事件，当此事件发生时，将根据当前状态执行相应的状态转换：

如果在 ST_IDLE（空闲）状态，事件被忽视（EVENT_IGNORED）。
如果在 ST_COMPLETED（完成）或 ST_FAILED（失败）状态，事件是不可能发生的（CANNOT_HAPPEN）。
如果在启动测试及后续进程状态（例如加速或减速中），转移到 ST_FAILED（失败）状态。
*/
EVENT_DEFINE(CFG_Cancel, NoEventData)
{
    BEGIN_TRANSITION_MAP                                // - Current State -
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)             // ST_IDLE
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)             // ST_COMPLETED
        TRANSITION_MAP_ENTRY(CANNOT_HAPPEN)             // ST_FAILED
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_START_TEST
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_WAIT_FOR_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_DECELERATION
        TRANSITION_MAP_ENTRY(ST_FAILED)                 // ST_WAIT_FOR_DECELERATION
    END_TRANSITION_MAP(CentrifugeTest, pEventData)
}
/*
EVENT_DEFINE(CFG_Poll, NoEventData) – 定义一个名为 "CFG_Poll" 的周期性检查事件，根据当前状态进行处理：

如果在空闲、完成、失败或测试启动中，事件被忽略。
如果在加速或等待加速状态，进入等待加速状态。
如果在减速或等待减速状态，进入等待减速状态。
*/
EVENT_DEFINE(CFG_Poll, NoEventData)
{
    BEGIN_TRANSITION_MAP                                    // - Current State -
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)                 // ST_IDLE
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)                 // ST_COMPLETED
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)                 // ST_FAILED
        TRANSITION_MAP_ENTRY(EVENT_IGNORED)                 // ST_START_TEST
        TRANSITION_MAP_ENTRY(ST_WAIT_FOR_ACCELERATION)      // ST_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_WAIT_FOR_ACCELERATION)      // ST_WAIT_FOR_ACCELERATION
        TRANSITION_MAP_ENTRY(ST_WAIT_FOR_DECELERATION)      // ST_DECELERATION
        TRANSITION_MAP_ENTRY(ST_WAIT_FOR_DECELERATION)      // ST_WAIT_FOR_DECELERATION
    END_TRANSITION_MAP(CentrifugeTest, pEventData)
}
//StartPoll 和 StopPoll – 这两个函数分别用来开始和停止轮询，通过修改 centrifugeTestObj.pollActive 来标识轮询的活动状态。
static void StartPoll(void)
{
    centrifugeTestObj.pollActive = TRUE;
}

static void StopPoll(void)
{
    centrifugeTestObj.pollActive = FALSE;
}
//CFG_IsPollActive – 返回当前的轮询活动状态，在其他部分代码中可能用于条件判断。
BOOL CFG_IsPollActive(void) 
{ 
    return centrifugeTestObj.pollActive;
}
/*
状态定义（STATE_DEFINE）和 ENTRY_DEFINE:

Idle – 空闲状态的具体行为及其入口行为（设置转速为0，停止轮询）。
Completed 和 Failed – 在完成或失败的状态下，会打印相应的状态名称，并通过内部事件自动回到空闲状态。
StartTest – 启动测试时的状态定义，会立即触发加速度状态。
*/
STATE_DEFINE(Idle, NoEventData)
{
    printf("%s ST_Idle\n", self->name);
}

ENTRY_DEFINE(Idle, NoEventData)
{
    printf("%s EN_Idle\n", self->name);
    centrifugeTestObj.speed = 0;
    StopPoll();
}

STATE_DEFINE(Completed, NoEventData)
{
    printf("%s ST_Completed\n", self->name);
    SM_InternalEvent(ST_IDLE, NULL);
}

STATE_DEFINE(Failed, NoEventData)
{
    printf("%s ST_Failed\n", self->name);
    SM_InternalEvent(ST_IDLE, NULL);
}

// Start the centrifuge test state.
STATE_DEFINE(StartTest, NoEventData)
{
    printf("%s ST_StartTest\n", self->name);
    SM_InternalEvent(ST_ACCELERATION, NULL);
}

// Guard condition to determine whether StartTest state is executed.
GUARD_DEFINE(StartTest, NoEventData)
{
    printf("%s GD_StartTest\n", self->name);
    if (centrifugeTestObj.speed == 0)
        return TRUE;    // Centrifuge stopped. OK to start test.
    else
        return FALSE;   // Centrifuge spinning. Can't start test.
}

// Start accelerating the centrifuge.
STATE_DEFINE(Acceleration, NoEventData)
{
    printf("%s ST_Acceleration\n", self->name);

    // Start polling while waiting for centrifuge to ramp up to speed
    StartPoll();
}

// Wait in this state until target centrifuge speed is reached.
STATE_DEFINE(WaitForAcceleration, NoEventData)
{
    printf("%s ST_WaitForAcceleration : Speed is %d\n", self->name, centrifugeTestObj.speed);
    if (++centrifugeTestObj.speed >= 5)
        SM_InternalEvent(ST_DECELERATION, NULL);
}

// Exit action when WaitForAcceleration state exits.
EXIT_DEFINE(WaitForAcceleration)
{
    printf("%s EX_WaitForAcceleration\n", self->name);

    // Acceleration over, stop polling
    StopPoll();
}

// Start decelerating the centrifuge.
STATE_DEFINE(Deceleration, NoEventData)
{
    printf("%s ST_Deceleration\n", self->name);

    // Start polling while waiting for centrifuge to ramp down to 0
    StartPoll();
}

// Wait in this state until centrifuge speed is 0.
STATE_DEFINE(WaitForDeceleration, NoEventData)
{
    printf("%s ST_WaitForDeceleration : Speed is %d\n", self->name, centrifugeTestObj.speed);
    if (centrifugeTestObj.speed-- == 0)
        SM_InternalEvent(ST_COMPLETED, NULL);
}

// Exit action when WaitForDeceleration state exits.
EXIT_DEFINE(WaitForDeceleration)
{
    printf("%s EX_WaitForDeceleration\n", self->name);

    // Deceleration over, stop polling
    StopPoll();
}


