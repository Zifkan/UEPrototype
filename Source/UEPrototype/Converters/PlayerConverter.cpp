// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerConverter.h"
#include "CharacterActor.h"
#include "Components/AnimComponent.h"
#include "Components/GeneralComponents.h"
#include "Components/MovementComponents.h"
#include "Components/StateMachine/CheckActionTag.h"
#include "Components/PlayerComponents.h"
#include "Components/TransformComponents.h"
#include "Components/CharacterStates/CharacterStates.h"
#include "Core/EcsWorld.h"



void UPlayerConverter::Convert(Entity e, FEntityManager dstManager)
{
    auto inputEntity = dstManager.CreateEntity("InputEntity");

    inputEntity.add_childof(e);
    
    const auto characterActor = Cast<ACharacterActor>(GetOwner());
    const auto characterAnimInstance = Cast<UCharacterAnimInstance>(characterActor->GetAnimInstance());
    
    dstManager.AddComponentData<PlayerTag>(e);
    dstManager.AddComponentData<MovementVelocity>(e);
    dstManager.SetComponentData<MovementSpeed>(e, { 2000.f });
    dstManager.AddComponentData<MoveDirectionData>(e);
    dstManager.AddComponentData<ViewDirectionData>(e);
    dstManager.AddComponentData<Rotation>(e);
    dstManager.AddComponentData<LocalToWorld>(e);
    dstManager.AddComponentData<Translation>(e);
  //  dstManager.AddComponentData<CopyTransformFromActor>(e);
    dstManager.AddComponentData<CopyTransformToActor>(e);    
    dstManager.SetComponentData<FActorComponent>(e, { characterActor });
    dstManager.SetComponentData<AnimComponent>(e, { characterAnimInstance });
    dstManager.AddComponentData<CheckActionData>(e);
    

    auto Idle = dstManager.defaultWorld->entity().add<IdleState>();
    auto Move = dstManager.defaultWorld->entity().add<MoveState>();
    auto Movement = dstManager.SetType("Movement","IdleState, MoveState");

    auto CheckAction = dstManager.defaultWorld->entity().add<CheckActionTag>();    
    auto stateAvailable = dstManager.SetType("StateAvailable","CheckActionTag");
    
     e.add_switch(Movement).add_case<MoveState>();
     e.add_switch(stateAvailable).add_case<CheckActionTag>();
    
}
