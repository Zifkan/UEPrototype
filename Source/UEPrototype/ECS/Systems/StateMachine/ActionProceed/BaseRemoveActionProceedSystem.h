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
};

template <class ComponentType>
void BaseRemoveActionProceedSystem<ComponentType>::OnCreate()
{
   SystemRun->each([this](flecs::entity e, ComponentType& inputTag)
   {      
       e.remove<ComponentType>();
      // UE_LOG(LogTemp, Warning, TEXT("Remove AttackInputTag"));
   });
}

class  AttackRemoveActionProceedSystem   :public BaseRemoveActionProceedSystem<AttackInputTag>
{
};

class  BlockRemoveActionProceedSystem   :public BaseRemoveActionProceedSystem<BlockInputTag>
{
};

class  SprintRemoveActionProceedSystem   :public BaseRemoveActionProceedSystem<SprintInputTag>
{
};

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

