// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"
#include "BoneData.h"
#include "flecs.h"
#include "GameFramework/Actor.h"
#include "AnimMesh/Public/AnimVertexMeshComponent.h"
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
	
/*	UPROPERTY(EditAnywhere,Category=ECS)
	UAnimVertexMeshComponent* AnimationVertexMeshComponent;*/

	UPROPERTY(Category=Animation, VisibleAnywhere, BlueprintReadOnly, meta=(AllowPrivateAccess = "true"))
	USkeletalMeshComponent* Mesh;
	
	/* The AnimBlueprint class to use. Use 'SetAnimInstanceClass' to change at runtime. */
	UPROPERTY(EditAnywhere, BlueprintReadOnly, Category = Animation)
	class TSubclassOf<UAnimInstance> AnimClass;

	/** The active animation graph program instance. */
	UPROPERTY(transient, NonTransactional)
	UAnimInstance* AnimScriptInstance;
	
	flecs::system<>* testSys;

	void SetIndex(int index);

	void SetSetBuffer(TArray<FMatrix> buffer);

	void StateTest();

	UAnimInstance* GetAnimInstance() const; 
	
protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;

public:
	virtual void Tick(float DeltaSeconds) override;
};
