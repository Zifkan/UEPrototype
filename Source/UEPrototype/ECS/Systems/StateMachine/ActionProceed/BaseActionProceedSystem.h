#pragma once
#include "flecs.h"
#include "Components/CharacterActions/CharacterActionsComponents.h"
#include "Components/CharacterStates/CharacterStates.h"
#include "Components/StateMachine/CheckActionTag.h"
#include "Systems/SystemBase.h"
/*
template<class ... ComponentType>
class  BaseActionProceedSystem   :public SystemBase<ComponentType,ActionAvailableTag>
{
  
};*/


class MoveActionProceedSystem   :public SystemBase<MoveActionTag/*,ActionAvailableTag*/>
{
public:
    virtual void OnCreate() override;
};

inline void MoveActionProceedSystem::OnCreate()
{
 
    SystemRun->each([this](Entity e, MoveActionTag& moveActionTag/*, ActionAvailableTag availableTag*/)
    {  
       const auto parentEntity = e.get_parent<PlayerTag>();

     
        UE_LOG(LogTemp, Warning, TEXT("AttackActionProceedSystem parentEntity, %i"), parentEntity.id());
     
        parentEntity.add_case<MoveState>();
     
    });
}



class  AttackActionProceedSystem   :public SystemBase<AttackInputTag/*,ActionAvailableTag*/>
{
public:
    virtual void OnCreate() override;
};

inline void AttackActionProceedSystem::OnCreate()
{
    SystemRun->each([this](Entity e, AttackInputTag& AttackInputTag/*, ActionAvailableTag availableTag*/)
    {
       const auto parentEntity= e.get_parent<PlayerTag>();

        
        UE_LOG(LogTemp, Warning, TEXT("AttackActionProceedSystem parentEntity, %s"),UTF8_TO_TCHAR ( parentEntity.has<PlayerTag>()?"true" : "false"));
        UE_LOG(LogTemp, Warning, TEXT("Player Entity, %i"),parentEntity.id());
        UE_LOG(LogTemp, Warning, TEXT("Input Entity, %i"),e.id());
        parentEntity.add_case<AttackState>();       
        
    });
}
