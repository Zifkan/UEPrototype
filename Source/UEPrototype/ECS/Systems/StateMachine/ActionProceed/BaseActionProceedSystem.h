#pragma once
#include "Components/CharacterActions/CharacterActionsComponents.h"
#include "Components/StateMachine/CheckActionTag.h"
#include "Systems/SystemBase.h"
/*
template<class ... ComponentType>
class  BaseActionProceedSystem   :public SystemBase<ComponentType,ActionAvailableTag>
{
  
};*/


class  AttackActionProceedSystem   :public SystemBase<AttackInputTag,ActionAvailableTag>
{
public:
    virtual void OnCreate() override;
};

inline void AttackActionProceedSystem::OnCreate()
{
    SystemRun->each([](flecs::entity e, AttackInputTag& AttackInputTag, ActionAvailableTag availableTag)
    {
        auto parentEntity = e.get_parent(e);

        UE_LOG(LogTemp, Warning, TEXT("AttackActionProceedSystem parentEntity, %f"), FPlatformTime::Seconds());
    });
}
