#include "fb_allocator.h"
#include "StateMachine.h"
#include "Motor.h"

int main()
{
    ALLOC_Init();

    MotorData* data;
    
    data = SM_XALLOC(sizeof(MotorData));
    data->speed = 100;
    MTR_SetSpeed(data);

    data = SM_XALLOC(sizeof(MotorData));
    data->speed = 200;
    MTR_SetSpeed(data);

    MTR_Halt();
    MTR_Halt();

    ALLOC_Term();

    return 0;
}

