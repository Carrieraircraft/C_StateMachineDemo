#include "Fault.h"
#include "StateMachine.h"

// Generates an external event. Called once per external event 
// to start the state machine executing
// 根据外部事件触发状态机这个函数用于生成外部事件，并启动状态机执行。
void _SM_ExternalEvent(SM_StateMachine* self, const SM_StateMachineConst* selfConst, BYTE newState, void* pEventData) {
    // 如果新状态是忽略事件
    if (newState == EVENT_IGNORED) {
        // 如果有事件数据，则删除它
        if (pEventData)
            SM_XFree(pEventData);  // 释放事件数据内存
    }
    else {
        // 如果需要线程安全，这里可以加锁

        // 产生一个内部事件
        _SM_InternalEvent(self, newState, pEventData);

        // 根据状态映射表的类型，执行状态机
        if (selfConst->stateMap)
            _SM_StateEngine(self, selfConst);  // 执行基本状态引擎
        else
            _SM_StateEngineEx(self, selfConst);  // 执行扩展状态引擎

        // 如果加锁了，这里可以解锁
    }
}

// Generates an internal event. Called from within a state 
// function to transition to a new state这个函数用于在状态函数内部生成事件，并触发状态转换。
// 在状态函数内部产生一个内部事件，用于状态转换
void _SM_InternalEvent(SM_StateMachine* self, BYTE newState, void* pEventData) {
    ASSERT_TRUE(self);  // 断言状态机实例存在

    self->pEventData = pEventData;  // 设置事件数据
    self->eventGenerated = TRUE;    // 标记事件已生成
    self->newState = newState;      // 设置新状态
}

// The state engine executes the state machine states这个函数执行状态机状态的转换和执行。
// 状态引擎，执行状态机状态
void _SM_StateEngine(SM_StateMachine* self, const SM_StateMachineConst* selfConst) {
    void* pDataTemp = NULL;

    ASSERT_TRUE(self);
    ASSERT_TRUE(selfConst);

    // 当继续生成事件时，保持执行状态
    while (self->eventGenerated) {
        // 检查新状态是否有效
        ASSERT_TRUE(self->newState < selfConst->maxStates);

        // 从状态映射表获取状态函数指针
        SM_StateFunc state = selfConst->stateMap[self->newState].pStateFunc;

        // 暂存事件数据
        pDataTemp = self->pEventData;

        // 重置事件数据指针
        self->pEventData = NULL;

        // 重置事件生成标志
        self->eventGenerated = FALSE;

        // 切换到新的当前状态
        self->currentState = self->newState;

        // 执行状态函数，传递事件数据
        ASSERT_TRUE(state != NULL);
        state(self, pDataTemp);

        // 如果使用了事件数据，则删除它
        if (pDataTemp) {
            SM_XFree(pDataTemp);
            pDataTemp = NULL;
        }
    }
}

// The state engine executes the extended state machine states
//这段代码定义了 _SM_StateEngineEx 函数，它是执行扩展状态机状态的引擎。
//这个扩展版本的状态机包括了状态的进入（entry）动作和退出（exit）动作，以及状态转换守卫（guard conditions）
// 执行扩展状态机的状态引擎
void _SM_StateEngineEx(SM_StateMachine* self, const SM_StateMachineConst* selfConst) {
    BOOL guardResult = TRUE;  // 守卫条件结果，默认为真
    void* pDataTemp = NULL;   // 临时存储事件数据指针

    ASSERT_TRUE(self);  // 断言状态机实例存在
    ASSERT_TRUE(selfConst);  // 断言状态机常量结构体存在

    // 当事件被生成时，继续执行状态
    while (self->eventGenerated) {
        // 错误检查：新状态必须是有效的
        ASSERT_TRUE(self->newState < selfConst->maxStates);

        // 从扩展状态映射表中获取相关的函数指针
        SM_StateFunc state = selfConst->stateMapEx[self->newState].pStateFunc;
        SM_GuardFunc guard = selfConst->stateMapEx[self->newState].pGuardFunc;
        SM_EntryFunc entry = selfConst->stateMapEx[self->newState].pEntryFunc;
        SM_ExitFunc exit = selfConst->stateMapEx[self->currentState].pExitFunc;

        // 暂存事件数据
        pDataTemp = self->pEventData;

        // 重置事件数据指针
        self->pEventData = NULL;

        // 重置事件生成标志
        self->eventGenerated = FALSE;

        // 执行守卫函数
        if (guard != NULL)
            guardResult = guard(self, pDataTemp);  // 执行守卫条件判断函数

        // 如果守卫条件成功
        if (guardResult == TRUE) {
            // 转换到新状态？
            if (self->newState != self->currentState) {
                // 如果是新状态，先执行当前状态的退出动作
                if (exit != NULL)
                    exit(self);  // 执行退出函数

                // 执行新状态的进入动作
                if (entry != NULL)
                    entry(self, pDataTemp);  // 执行进入函数

                // 确保退出/进入动作没有意外地调用 SM_InternalEvent
                ASSERT_TRUE(self->eventGenerated == FALSE);
            }

            // 切换到新的当前状态
            self->currentState = self->newState;

            // 执行状态函数，传递事件数据
            ASSERT_TRUE(state != NULL);
            state(self, pDataTemp);
        }

        // 如果使用了事件数据，则删除它
        if (pDataTemp) {
            SM_XFree(pDataTemp);  // 释放事件数据内存
            pDataTemp = NULL;
        }
    }
}