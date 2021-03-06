// Fill out your copyright notice in the Description page of Project Settings.
#pragma once

#include "CoreMinimal.h"

#include "AnimMesh/Shaders/AnimationInstanceVertexFactory.h"

#include "AnimVertexMeshComponent.generated.h"
/**
 * 
 */

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANIMMESH_API UAnimVertexMeshComponent : public UInstancedStaticMeshComponent
{
	GENERATED_BODY()

public:
	// Sets default values for this component's properties
	UAnimVertexMeshComponent()
	:UInstancedStaticMeshComponent()
	{
		PrimaryComponentTick.bCanEverTick = true;
		PrimaryComponentTick.bStartWithTickEnabled = true;
	};


UPROPERTY(EditAnywhere)
	bool IsEditorUpdate;
	
	virtual FAnimationInstanceVertexSceneProxy* CreateSceneProxy() override;

	void UpdateBoneArray(TArray<FMatrix> buffer);	
	
	
	void SetRenderIndex(int index);

	UPROPERTY(EditAnywhere)
	USkeletalMesh* skeletalMesh;
	
private:
	void SetBuffer(TArray<FMatrix> buffer);
	void SetBufferFinish();
};
