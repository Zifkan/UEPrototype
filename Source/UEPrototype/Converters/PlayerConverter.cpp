// Fill out your copyright notice in the Description page of Project Settings.


#include "PlayerConverter.h"
#include "Components/MovementComponents.h"
#include "Components/PlayerComponents.h"
#include "Components/TransformComponents.h"
#include "Core/EcsWorld.h"

void UPlayerConverter::Convert(flecs::entity entity, FEntityManager dstManager)
{
    auto e = dstManager.CreateEntity("Player");

    dstManager.SetComponentData<PlayerTag>(e, {  });
    dstManager.SetComponentData<MovementVelocity>(e, {  });
    dstManager.SetComponentData<MovementSpeed>(e, { 20.f });
    dstManager.SetComponentData<MoveDirectionData>(e, {  });
    dstManager.SetComponentData<ViewDirectionData>(e, {  });
    dstManager.SetComponentData<Rotation>(e, {  });

}
