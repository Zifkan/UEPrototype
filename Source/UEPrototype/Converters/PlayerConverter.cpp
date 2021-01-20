// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerConverter.h"
#include "CharacterActor.h"
#include "Components/AnimComponent.h"
#include "Components/GeneralComponents.h"
#include "Components/MovementComponents.h"
#include "Components/StateMachine/CheckActionTag.h"
#include "Components/PlayerComponents.h"
#include "Components/TransformComponents.h"
#include "Components/CharacterActions/CharacterActionsComponents.h"
#include "Components/CharacterStates/CharacterStates.h"
#include "Core/EcsWorld.h"



void UPlayerConverter::Convert(Entity s, FEntityManager dstManager)
{ 
	auto e = dstManager.defaultWorld->singleton<PlayerEntityType>();
	
	dstManager.AddComponentData<PlayerTag>(e);
	
	auto inputEntity =  dstManager.defaultWorld->singleton<InputEntityType>();
	
    inputEntity.add_childof(e);

    const auto characterActor = Cast<ACharacterActor>(GetOwner());
    const auto characterAnimInstance = Cast<UCharacterAnimInstance>(characterActor->GetAnimInstance());
    
    dstManager.AddComponentData<CharacterTag>(e);
    dstManager.AddComponentData<MovementVelocity>(e);
    dstManager.SetComponentData<MovementSpeed>(e, { 0.f,1 });
    dstManager.AddComponentData<MoveDirectionData>(e);
    dstManager.AddComponentData<ViewDirectionData>(e);
    dstManager.AddComponentData<Rotation>(e);
    dstManager.AddComponentData<LocalToWorld>(e);
    dstManager.AddComponentData<Translation>(e);
  //  dstManager.AddComponentData<CopyTransformFromActor>(e);
    dstManager.AddComponentData<CopyTransformToActor>(e);    
    dstManager.SetComponentData<FActorComponent>(e, { characterActor });
    dstManager.SetComponentData<AnimComponent>(e, { characterAnimInstance }); 	
	
    UE_LOG(LogTemp, Warning, TEXT("Player Entity, %i"),e.id());
    UE_LOG(LogTemp, Warning, TEXT("Input Entity, %i"),inputEntity.id());

    UE_LOG(LogTemp, Warning, TEXT("Has PlayerTag, %s"),UTF8_TO_TCHAR (inputEntity.get_parent<CharacterTag>().has<CharacterTag>()?"true" : "false"));

    auto Idle = dstManager.defaultWorld->entity().add<IdleState>();
    auto Move = dstManager.defaultWorld->entity().add<MoveState>();
    auto Attack = dstManager.defaultWorld->entity().add<AttackState>();
	auto MoveBlock  = dstManager.defaultWorld->entity().add<MoveBlockState>();
    auto Movement = dstManager.SetType("Movement","IdleState, MoveState,MoveBlockState, AttackState");

    
	
	e.add_switch(Movement);
	e.add_case<IdleState>();
	
	inputEntity.set<CheckActionData>({});
	inputEntity.add<ActionAvailableTag>();  
}
