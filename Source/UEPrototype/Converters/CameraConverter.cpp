// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraConverter.h"
#include "Core/EntityManager.h"
#include "Systems/Transforms/WorldToLocalSystem.h"
#include "UEPrototype/ECS/Components/CamComponent.h"

void UCameraConverter::Convert(flecs::entity entity, FEntityManager dstManager)
{
         dstManager.SetComponentData<CamComponent>(entity,
    {
            HeightOffset, FocusHeightOffset,0,0,Radius, CurrentRadius,SpeedX,SpeedY, yMinLimit,yMaxLimit,PitchLimit
    });
       
    dstManager.SetComponentData<LocalToWorld>(entity, {  });
    dstManager.SetComponentData<CopyTransformFromActor>(entity, {});
    dstManager.SetComponentData<Rotation>(entity, {  });
    dstManager.SetComponentData<Translation>(entity, {  });
    dstManager.SetComponentData<WorldToLocal>(entity, {  });
}
