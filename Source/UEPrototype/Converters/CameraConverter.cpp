// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraConverter.h"

#include "Components/GeneralComponents.h"
#include "Core/EntityManager.h"
#include "Systems/Transforms/WorldToLocalSystem.h"
#include "UEPrototype/ECS/Components/CamComponent.h"

void UCameraConverter::Convert(flecs::entity entity, FEntityManager dstManager)
{
         dstManager.SetComponentData<CamComponent>(entity,
    {
            HeightOffset, FocusHeightOffset,0,0,Radius, CurrentRadius,SpeedX,SpeedY, yMinLimit,yMaxLimit,PitchLimit,ProbeChannel,ProbeSize
    });
       
    dstManager.SetComponentData<LocalToWorld>(entity, { FMatrix::Identity });
    dstManager.SetComponentData<CopyTransformFromActor>(entity, {});
    dstManager.SetComponentData<FActorComponent>(entity, { GetOwner() });
    dstManager.SetComponentData<Rotation>(entity, { FQuat::Identity });
    dstManager.SetComponentData<Translation>(entity, { FVector::ZeroVector });
    dstManager.SetComponentData<WorldToLocal>(entity, { FMatrix::Identity });
    dstManager.SetComponentData<CopyTransformToActor>(entity, {});
}
