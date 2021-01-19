#pragma once
#include "Components/AnimComponent.h"
#include "Components/GeneralComponents.h"
#include "Components/MovementComponents.h"
#include "Components/CharacterStates/CharacterStates.h"
#include "Components/StateMachine/CheckActionTag.h"
#include "Systems/SystemBase.h"

template<class ... ComponentType>
class BaseStateSystem  :public SystemBase<ComponentType...>
{
public:    
    BaseStateSystem(const char *name = nullptr, const char *signature = nullptr): SystemBase(name,signature)
    {
        
    }
    
    template<class T>
    void SetActionAvailable(flecs::entity e,const  CheckActionData actionData)
    {
        for (auto children : e.children())
        {
            for (auto i : children)
            {
                if(children.entity(i).has<T>())
                {                   
                    children.entity(i).remove<ActionAvailableTag>();
                    children.entity(i).set<CheckActionData>(actionData);
                }
            }
        }
    }
};


class MovementStateSystem final :public BaseStateSystem<AnimComponent,MoveDirectionData,MovementSpeed,FActorComponent>
{
public:
    MovementStateSystem() : BaseStateSystem("MovementStateSystem"){}
    
    virtual void OnCreate() override;
};

class IdleStateSystem final :public BaseStateSystem<AnimComponent,MovementSpeed>
{
public:    
    IdleStateSystem() : BaseStateSystem("IdleStateSystem"){}

    virtual void OnCreate() override;
};

class AttackStateSystem final :public BaseStateSystem<AnimComponent,MovementSpeed>
{
public:    
    AttackStateSystem() : BaseStateSystem("AttackStateSystem"){}   

    virtual void OnCreate() override;
};

class BlockingStateSystem final :public BaseStateSystem<AnimComponent,MoveDirectionData,MovementSpeed,FActorComponent>
{
public:    
    BlockingStateSystem() : BaseStateSystem("BlockingStateSystem"){}
    
    virtual void OnCreate() override;
};