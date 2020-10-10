// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "AnimationVertexMeshComponent.h"
#include "BoneData.h"
#include "GameFramework/Actor.h"
#include "CharacterActor.generated.h"

UCLASS(Blueprintable)
class UEPROTOTYPE_API ACharacterActor : public AActor
{
	GENERATED_BODY()
	
public:	
	// Sets default values for this actor's properties
	ACharacterActor();

    UPROPERTY()
    USceneComponent* Root;
	
	UPROPERTY(EditAnywhere,Category=ECS)
	TArray<FBoneDataInfo>  BoneInfoArray;
	
	UPROPERTY(EditAnywhere)
	UAnimationVertexMeshComponent*  animationVertexMeshComponent;
	
	TArray<UStaticMeshComponent> MeshRenderers;
	
	void SetIndex(int index);

	void SetSetBuffer(TArray<FMatrix> buffer);

	
	

	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;	
	
};
