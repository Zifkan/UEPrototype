// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"


#include "Components/StaticMeshComponent.h"
#include "AnimVertexMeshComponent.generated.h"
/**
 * 
 */

class FAnimationVertexSceneProxy;

UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class ANIMMESH_API UAnimVertexMeshComponent : public UStaticMeshComponent
{
	GENERATED_BODY()

public:    
	// Sets default values for this component's properties
	UAnimVertexMeshComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;

	void SetRenderIndex(int index);
	void SetBuffer(TArray<FMatrix> buffer);

	FAnimationVertexSceneProxy* Proxy;
};
