// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerConverter.h"

#include "Components/GeneralComponents.h"
#include "Components/MovementComponents.h"
#include "Components/PlayerComponents.h"
#include "Components/TransformComponents.h"
#include "Core/EcsWorld.h"



void UPlayerConverter::Convert(flecs::entity e, FEntityManager dstManager)
{
    dstManager.SetComponentData<PlayerTag>(e, {  });
    dstManager.SetComponentData<MovementVelocity>(e, {  });
    dstManager.SetComponentData<MovementSpeed>(e, { 20.f });
    dstManager.SetComponentData<MoveDirectionData>(e, {  });
    dstManager.SetComponentData<ViewDirectionData>(e, {  });
    dstManager.SetComponentData<Rotation>(e, {  });
    dstManager.SetComponentData<LocalToWorld>(e, {  });
 //   dstManager.SetComponentData<Translation>(e, { FVector::ZeroVector });
    dstManager.SetComponentData<CopyTransformFromActor>(e, {  });
    dstManager.SetComponentData<FActorComponent>(e, { GetOwner() });

}
