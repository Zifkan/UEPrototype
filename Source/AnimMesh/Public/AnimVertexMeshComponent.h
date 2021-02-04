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
	UAnimVertexMeshComponent();

	virtual FAnimationInstanceVertexSceneProxy* CreateSceneProxy() override;

	

	
	void SetRenderIndex(int index);
	void SetBuffer(TArray<FMatrix> buffer);
	void SetBufferFinish();
	
	virtual void TickComponent(float DeltaTime, ELevelTick TickType,FActorComponentTickFunction* ThisTickFunction) override;
	float scale;
};
