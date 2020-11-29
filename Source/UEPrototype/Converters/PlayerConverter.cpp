// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerConverter.h"


#include "Components/AnimComponent.h"
#include "Components/GeneralComponents.h"
#include "Components/MovementComponents.h"
#include "Components/PlayerComponents.h"
#include "Components/TransformComponents.h"
#include "Core/EcsWorld.h"



void UPlayerConverter::Convert(Entity e, FEntityManager dstManager)
{    
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
    dstManager.SetComponentData<FActorComponent>(e, { GetOwner() });
    dstManager.SetComponentData<AnimComponent>(e, { Cast<UCharacterAnimInstance>(SkeletalMesh->PostProcessAnimBlueprint) });
    
}
