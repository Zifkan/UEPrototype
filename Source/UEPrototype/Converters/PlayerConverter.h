// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConvertToEntity.h"
#include "UObject/ObjectMacros.h"

#include "PlayerConverter.generated.h"

/**
 * 
 */
UCLASS(ClassGroup = (ECS), meta = (BlueprintSpawnableComponent))
class UEPROTOTYPE_API UPlayerConverter : public UConvertToEntity
{
public:

	
    void Convert(flecs::entity entity, FEntityManager dstManager) override;
private:
	GENERATED_BODY()
	
};
