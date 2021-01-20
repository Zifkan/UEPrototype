// Fill out your copyright notice in the Description page of Project Settings.


#include "CameraConverter.h"

#include "Components/GeneralComponents.h"
#include "Core/EntityManager.h"
#include "Systems/Transforms/WorldToLocalSystem.h"
#include "UEPrototype/ECS/Components/CamComponent.h"

void UCameraConverter::Convert(flecs::entity e, FEntityManager dstManager)
{
	auto entity = dstManager.defaultWorld->singleton<CamEntityType>();
	
    dstManager.SetComponentData<CamComponent>(entity,
    {
            HeightOffset, FocusHeightOffset,0,0,Radius, CurrentRadius,SpeedX,SpeedY, yMinLimit,yMaxLimit,PitchLimit,ProbeChannel,ProbeSize
    });
       
    entity.set<LocalToWorld>({ FMatrix::Identity });
    entity.add<CopyTransformFromActor>( );
    entity.set<FActorComponent>( { GetOwner() });
    entity.set<Rotation>( { FQuat::Identity });
    entity.set<Translation>( { FVector::ZeroVector });
    entity.set<WorldToLocal>( { FMatrix::Identity });
    entity.add<CopyTransformToActor>( );
}
