// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Engine/StaticMesh.h"
#include "AnimVertexStaticMesh.generated.h"

/**
 * 
 */
UCLASS(hidecategories=Object, customconstructor, MinimalAPI, BlueprintType, config=Engine)
class UAnimVertexStaticMesh : public UStaticMesh
{
	GENERATED_BODY()
public:
	UPROPERTY()	
	uint32 BoneIndices;	
	/** The stream to read the bone weights from */
	UPROPERTY(EditAnywhere)
	uint32 TestValue;	
	
	
	FRHIShaderResourceView* BoneWeightSRV;


	FSkinWeightVertexBuffer SkinWeightVertexBuffer;
};
