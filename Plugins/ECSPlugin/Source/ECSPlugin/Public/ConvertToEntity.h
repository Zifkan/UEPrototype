// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "ECSPlugin/ECS/Core/IConvertActorToEntity.h"

#include "ConvertToEntity.generated.h"


UCLASS( ClassGroup=(ECS), meta=(BlueprintSpawnableComponent) )
class ECSPLUGIN_API UConvertToEntity : public UActorComponent, public IConvertActorToEntity
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UConvertToEntity(){ PrimaryComponentTick.bCanEverTick = false; };

    UPROPERTY(EditAnywhere, Category = ECS)
    FString  EntityName;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;

public:	

    void Convert(flecs::entity entity, FEntityManager dstManager) override{};
};
