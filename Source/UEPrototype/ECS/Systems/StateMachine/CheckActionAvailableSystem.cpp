#include "CheckActionAvailableSystem.h"

void CheckActionAvailableSystem::OnCreate()
{
    SystemRun->signature("!ActionAvailableTag").each([](flecs::entity e, CheckActionData& checkData)
    {
        UE_LOG(LogTemp, Warning, TEXT("CheckActionTag Working, %f"), FPlatformTime::Seconds());
        if(checkData.StartTime+checkData.Duration <= FPlatformTime::Seconds()-checkData.Space)
        {
            e.add<ActionAvailableTag>();
        }
    });
}
