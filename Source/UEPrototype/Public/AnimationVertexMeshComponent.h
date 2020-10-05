// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Components/ActorComponent.h"
#include "AnimationVertexMeshComponent.generated.h"


UCLASS( ClassGroup=(Custom), meta=(BlueprintSpawnableComponent) )
class UEPROTOTYPE_API UAnimationVertexMeshComponent : public UMeshComponent
{
	GENERATED_BODY()

public:	
	// Sets default values for this component's properties
	UAnimationVertexMeshComponent();

	virtual FPrimitiveSceneProxy* CreateSceneProxy() override;
};
