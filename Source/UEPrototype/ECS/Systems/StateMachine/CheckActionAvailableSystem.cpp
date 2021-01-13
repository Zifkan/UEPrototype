#include "CheckActionAvailableSystem.h"

void CheckActionAvailableSystem::OnCreate()
{
    SystemRun->signature("!ActionAvailableTag").each([](flecs::entity e, CheckActionData& checkData)
    {       
        if(checkData.StartTime+checkData.Duration <= FPlatformTime::Seconds()-checkData.Space)
        {
            UE_LOG(LogTemp, Warning, TEXT("CheckActionTag Add  ActionAvailableTag Ent Id, %i"), e.id());
            e.add<ActionAvailableTag>();
        }
    });
}
