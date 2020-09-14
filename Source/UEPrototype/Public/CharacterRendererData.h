// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "UEPrototype/Animations/BoneInfo.h"

#include "CharacterRendererData.generated.h"



UCLASS(ClassGroup = (ECS), meta = (BlueprintSpawnableComponent))
class UEPROTOTYPE_API UCharacterRendererData : public UActorComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UCharacterRendererData();

public:
    TArray<BoneInfo>  BoneInfoArray;
    TArray<UStaticMeshComponent> MeshRenderers;

protected:
	// Called when the game starts
	virtual void BeginPlay() override;
		
};
