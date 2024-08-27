// https://www.codeproject.com/Articles/1275479/State-Machine-Design-in-C
//
// The StateMachine module is a C language implementation of a finite state 
// machine (FSM).
//
// All event data must be created dynamically using SM_XAlloc. Use a fixed 
// block allocator or the heap as desired. 
//
// The standard version (non-EX) supports state and event functions. The 
// extended version (EX) supports the additional guard, entry and exit state
// machine features. 
//该代码片段为复杂的状态机实现提供了一个结构化的方式，通过宏和函数原型简化了状态机的创建和管理，有效地减少了代码重复，提高了代码可读性和可维护性。
// Macros are used to assist in creating the state machine machinery. 

#ifndef _STATE_MACHINE_H
#define _STATE_MACHINE_H

#include "DataTypes.h" // 引入自定义数据类型
#include "Fault.h"     // 引入故障管理相关的头文件

#ifdef __cplusplus
extern "C" {
#endif

#define USE_SM_ALLOCATOR   // 定义宏，用于选择固定块分配器而不是堆分配

#ifdef USE_SM_ALLOCATOR
    #include "sm_allocator.h"          // 引入状态机专用的内存分配器头文件
    #define SM_XAlloc(size) SMALLOC_Alloc(size)     // 定义状态机内存分配的宏
    #define SM_XFree(ptr)   SMALLOC_Free(ptr)       // 定义状态机内存释放的宏
#else
    #include <stdlib.h>
    #define SM_XAlloc(size) malloc(size)    // 使用标准库的malloc来分配内存
    #define SM_XFree(ptr)   free(ptr)       // 使用标准库的free来释放内存
#endif

enum { EVENT_IGNORED = 0xFE, CANNOT_HAPPEN = 0xFF };  // 定义事件处理的结果常量

typedef void NoEventData;    // 空事件数据类型定义

// 状态机常量数据结构
typedef struct
{
    const CHAR* name;                // 状态机名称
    const BYTE maxStates;            // 最大状态数
    const struct SM_StateStruct* stateMap;        // 指向常规状态映射的指针
    const struct SM_StateStructEx* stateMapEx;    // 指向扩展状态映射的指针
} SM_StateMachineConst;

// 状态机实例数据结构
typedef struct 
{
    const CHAR* name;       // 实例名称
    void* pInstance;        // 指向实例数据的指针
    BYTE newState;          // 新状态
    BYTE currentState;      // 当前状态
    BOOL eventGenerated;    // 表示是否生成了事件
    void* pEventData;       // 指向事件数据的指针
} SM_StateMachine;

// 定义各种状态函数、守卫函数、入口函数和出口函数的类型
typedef void (*SM_StateFunc)(SM_StateMachine* self, void* pEventData);
typedef BOOL (*SM_GuardFunc)(SM_StateMachine* self, void* pEventData);
typedef void (*SM_EntryFunc)(SM_StateMachine* self, void* pEventData);
typedef void (*SM_ExitFunc)(SM_StateMachine* self);

typedef struct SM_StateStruct
{
    SM_StateFunc pStateFunc;    // 状态函数指针
} SM_StateStruct;

typedef struct SM_StateStructEx
{
    SM_StateFunc pStateFunc;     // 状态函数指针
    SM_GuardFunc pGuardFunc;     // 守卫函数指针
    SM_EntryFunc pEntryFunc;     // 入口函数指针
    SM_ExitFunc pExitFunc;      // 出口函数指针
} SM_StateStructEx;

// Public functions这些公共函数宏是用于触发状态机事件和获取状态机信息：
/*
SM_Event: 触发指定的状态机事件处理函数。
SM_Get: 获取通过状态机处理的数据。
*/
#define SM_Event(_smName_, _eventFunc_, _eventData_) \
    _eventFunc_(&_smName_##Obj, _eventData_)
#define SM_Get(_smName_, _getFunc_) \
    _getFunc_(&_smName_##Obj)

// Protected functions这些内部函数宏用于更新状态机的内部状态和获取实例指针：
/*
SM_InternalEvent: 当前状态机实例产生一个内部事件，改变状态。
SM_GetInstance: 获取状态机实例数据，为类型转换做准备。*/
#define SM_InternalEvent(_newState_, _eventData_) \
    _SM_InternalEvent(self, _newState_, _eventData_)
#define SM_GetInstance(_instance_) \
    (_instance_*)(self->pInstance);

// Private functions这些是在状态机实现文件中使用的内部控制函数：
/*
_SM_ExternalEvent和_SM_InternalEvent: 这些函数用于处理从外部或内部触发的状态变化。
_SM_StateEngine和_SM_StateEngineEx: 这些函数负责处理状态机的状态转换逻辑，包括基本和扩展状态机。
*/
void _SM_ExternalEvent(SM_StateMachine* self, const SM_StateMachineConst* selfConst, BYTE newState, void* pEventData);
void _SM_InternalEvent(SM_StateMachine* self, BYTE newState, void* pEventData);
void _SM_StateEngine(SM_StateMachine* self, const SM_StateMachineConst* selfConst);
void _SM_StateEngineEx(SM_StateMachine* self, const SM_StateMachineConst* selfConst);

//这些宏用于在代码中声明和定义状态机及其组件：
/*
SM_DECLARE: 在其他文件中声明一个状态机。
SM_DEFINE: 定义一个初始状态为0的状态机实例。
*/
#define SM_DECLARE(_smName_) \
    extern SM_StateMachine _smName_##Obj; 

#define SM_DEFINE(_smName_, _instance_) \
    SM_StateMachine _smName_##Obj = { #_smName_, _instance_, \
        0, 0, 0, 0 }; 

/*
事件、状态、条件、入口和出口的声明与定义宏
这些宏用于定义状态机的各个组成部分，如事件处理函数、状态函数、条件函数（守护），以及入口和出口函数：
这些宏用于声明和定义处理特定事件、状态、守卫条件、入口和出口动作的函数。可以视需求进行扩展或修改。
*/
#define EVENT_DECLARE(_eventFunc_, _eventData_) \
    void _eventFunc_(SM_StateMachine* self, _eventData_* pEventData);

#define EVENT_DEFINE(_eventFunc_, _eventData_) \
    void _eventFunc_(SM_StateMachine* self, _eventData_* pEventData)

#define GET_DECLARE(_getFunc_, _getData_) \
    _getData_ _getFunc_(SM_StateMachine* self);

#define GET_DEFINE(_getFunc_, _getData_) \
    _getData_ _getFunc_(SM_StateMachine* self)

#define STATE_DECLARE(_stateFunc_, _eventData_) \
    static void ST_##_stateFunc_(SM_StateMachine* self, _eventData_* pEventData);

#define STATE_DEFINE(_stateFunc_, _eventData_) \
    static void ST_##_stateFunc_(SM_StateMachine* self, _eventData_* pEventData)

#define GUARD_DECLARE(_guardFunc_, _eventData_) \
    static BOOL GD_##_guardFunc_(SM_StateMachine* self, _eventData_* pEventData);

#define GUARD_DEFINE(_guardFunc_, _eventData_) \
    static BOOL GD_##_guardFunc_(SM_StateMachine* self, _eventData_* pEventData)

#define ENTRY_DECLARE(_entryFunc_, _eventData_) \
    static void EN_##_entryFunc_(SM_StateMachine* self, _eventData_* pEventData);

#define ENTRY_DEFINE(_entryFunc_, _eventData_) \
    static void EN_##_entryFunc_(SM_StateMachine* self, _eventData_* pEventData)

#define EXIT_DECLARE(_exitFunc_) \
    static void EX_##_exitFunc_(SM_StateMachine* self);

#define EXIT_DEFINE(_exitFunc_) \
    static void EX_##_exitFunc_(SM_StateMachine* self)

/*
状态映射宏
这类宏用于定义状态机的状态流程，包括基本和带扩展功能（如入口和出口动作）的状态流程图：
BEGIN_STATE_MAP 和 END_STATE_MAP: 定义一个基本的状态映射表。
BEGIN_STATE_MAP_EX 和 END_STATE_MAP_EX: 定义一个包含额外数据如守卫函数、入口和出口函数的状态映射表。
*/
#define BEGIN_STATE_MAP(_smName_) \
    static const SM_StateStruct _smName_##StateMap[] = { 

#define STATE_MAP_ENTRY(_stateFunc_) \
    { (SM_StateFunc)_stateFunc_ },

#define END_STATE_MAP(_smName_) \
    }; \
    static const SM_StateMachineConst _smName_##Const = { #_smName_, \
        (sizeof(_smName_##StateMap)/sizeof(_smName_##StateMap[0])), \
        _smName_##StateMap, NULL };

#define BEGIN_STATE_MAP_EX(_smName_) \
    static const SM_StateStructEx _smName_##StateMap[] = { 

#define STATE_MAP_ENTRY_EX(_stateFunc_) \
    { (SM_StateFunc)_stateFunc_, NULL, NULL, NULL },

#define STATE_MAP_ENTRY_ALL_EX(_stateFunc_, _guardFunc_, _entryFunc_, _exitFunc_) \
    { (SM_StateFunc)_stateFunc_, (SM_GuardFunc)_guardFunc_, (SM_EntryFunc)_entryFunc_, (SM_ExitFunc)_exitFunc_ },

#define END_STATE_MAP_EX(_smName_) \
    }; \
    static const SM_StateMachineConst _smName_##Const = { #_smName_, \
        (sizeof(_smName_##StateMap)/sizeof(_smName_##StateMap[0])), \
        NULL, _smName_##StateMap };

/*
转换映射宏
这类宏用于定义状态间在特定事件下的转换规则：
BEGIN_TRANSITION_MAP, TRANSITION_MAP_ENTRY, END_TRANSITION_MAP: 这些宏定义了一个静态数组来保存状态转换信息，并生成外部事件以触发状态转换。
*/
#define BEGIN_TRANSITION_MAP \
    static const BYTE TRANSITIONS[] = { \

#define TRANSITION_MAP_ENTRY(_entry_) \
    _entry_,

#define END_TRANSITION_MAP(_smName_, _eventData_) \
    }; \
    _SM_ExternalEvent(self, &_smName_##Const, TRANSITIONS[self->currentState], _eventData_); \
    C_ASSERT((sizeof(TRANSITIONS)/sizeof(BYTE)) == (sizeof(_smName_##StateMap)/sizeof(_smName_##StateMap[0])));

#ifdef __cplusplus
}
#endif
 
#endif // _STATE_MACHINE_H
