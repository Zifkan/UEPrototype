#pragma once
#include "Components/CharacterActions/CharacterActionsComponents.h"
#include "Systems/SystemBase.h"
#include "Systems/SystemLauncher.h"

template<class ComponentType>
class  BaseRemoveActionProceedSystem   :public SystemBase<ComponentType>
{
public:
    virtual void OnCreate() override;
    virtual ~BaseRemoveActionProceedSystem(){}

    virtual void Execute(flecs::entity e);
};

template <class ComponentType>
void BaseRemoveActionProceedSystem<ComponentType>::OnCreate()
{
   SystemRun->each([this](flecs::entity e, ComponentType& inputTag)
   {      
       e.remove<ComponentType>();
      // UE_LOG(LogTemp, Warning, TEXT("Remove AttackInputTag"));
       Execute(e);
   }).kind(0); 
}

template <class ComponentType>
void BaseRemoveActionProceedSystem<ComponentType>::Execute(flecs::entity e)
{
}

class  AttackRemoveActionProceedSystem   :public BaseRemoveActionProceedSystem<AttackInputTag>
{
};

class  BlockRemoveActionProceedSystem   :public BaseRemoveActionProceedSystem<BlockInputTag>
{
};

class  SprintRemoveActionProceedSystem   :public BaseRemoveActionProceedSystem<SprintInputTag>
{
public:
    virtual void Execute(flecs::entity e) override;
};

inline void SprintRemoveActionProceedSystem::Execute(flecs::entity e)
{
    auto parent = e.get_parent<MovementSpeed>();
    parent.set<MovementSpeed>({1000,1});
}

class  MoveRemoveActionProceedSystem   :public BaseRemoveActionProceedSystem<MoveInputTag>
{
};

class  RollInputTagRemoveActionProceedSystem   :public BaseRemoveActionProceedSystem<RollInputTag>
{
};

class RemoveActionProceedSystemLauncher : public SystemLauncher
{
public:
    RemoveActionProceedSystemLauncher(FEcsWorld* world) : SystemLauncher(world)
    {
        RegisterSystem(new AttackRemoveActionProceedSystem());
        RegisterSystem(new BlockRemoveActionProceedSystem());
        RegisterSystem(new SprintRemoveActionProceedSystem());
        RegisterSystem(new MoveRemoveActionProceedSystem());
        RegisterSystem(new RollInputTagRemoveActionProceedSystem());
    }
};

