// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CharacterRendererData.h"
#include "CoreMinimal.h"
#include "GameFramework/Actor.h"
#include "CharacterActor.generated.h"

UCLASS()
class UEPROTOTYPE_API ACharacterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterActor();

    UPROPERTY()
    USceneComponent* Root;

    UPROPERTY(EditAnywhere)
    UStaticMeshComponent* Mesh;

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

    UCharacterRendererData* characterRendererData;

};
