#include "CheckActionAvailableSystem.h"

void CheckActionAvailableSystem::OnCreate()
{
    SystemRun->signature("!ActionAvailableTag").each([](flecs::entity e, CheckActionData& checkData)
    {       
        if(FPlatformTime::Seconds()>= checkData.StartTime+checkData.Duration -checkData.Space)
        {
            UE_LOG(LogTemp, Warning, TEXT("CheckActionTag  Start time: %f; Duration: %f, Result: %f; Now: %f"),  checkData.StartTime, checkData.Duration,checkData.StartTime+checkData.Duration -checkData.Space,FPlatformTime::Seconds());
            e.add<ActionAvailableTag>();
        }
    });
}
