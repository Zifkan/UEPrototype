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


class MoveActionProceedSystem   :public SystemBase<MoveInputTag,ActionAvailableTag>
{
public:
    MoveActionProceedSystem() : SystemBase("MoveActionProceedSystem"){}
    
    virtual void OnCreate() override;
};

inline void MoveActionProceedSystem::OnCreate()
{
    SystemRun->each([this](Entity e, MoveInputTag& moveActionTag, ActionAvailableTag& availableTag)
    {  
        const auto parentEntity = e.get_parent<PlayerTag>(); 
        parentEntity.add_case<MoveState>();  
    }).kind(0);
}



class  AttackActionProceedSystem   :public SystemBase<AttackInputTag,ActionAvailableTag>
{
public:
    AttackActionProceedSystem() : SystemBase("AttackActionProceedSystem"){}
    
    virtual void OnCreate() override;
};

inline void AttackActionProceedSystem::OnCreate()
{
    SystemRun->each([this](Entity e, AttackInputTag& AttackInputTag, ActionAvailableTag& availableTag)
    {
        const auto parentEntity= e.get_parent<PlayerTag>();  
        parentEntity.add_case<AttackState>(); 
    }).kind(0);
}

class  SprintActionProceedSystem   :public SystemBase<SprintInputTag>
{
public:
    SprintActionProceedSystem() : SystemBase("SprintActionProceedSystem"){}
  
    virtual void OnCreate()
    {        
        SystemRun->each([](Entity e, SprintInputTag& sprintInputTag)
        {
            const auto parentEntity= e.get_parent<MovementSpeed>();  
            parentEntity.set<MovementSpeed>({1000,2});
        }).kind(0);
    }
};


class  BlockActionProceedSystem   :public SystemBase<BlockInputTag,  ActionAvailableTag>
{
public:
    BlockActionProceedSystem() : SystemBase("BlockActionProceedSystem"){}
  
    virtual void OnCreate() override;
};

inline void BlockActionProceedSystem::OnCreate()
{
    SystemRun->each([](Entity e,const BlockInputTag blockInputTag,const ActionAvailableTag& availableTag)
    {
        const auto parentEntity= e.get_parent<MovementSpeed>();
        parentEntity.add_case<MoveBlockState>();    
    }).kind(0);
}
