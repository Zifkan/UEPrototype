// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConvertToEntity.h"
#include "PlayerConverter.generated.h"

/**
 * 
 */
UCLASS()
class UEPROTOTYPE_API UPlayerConverter : public UConvertToEntity
{
public:
    void Convert(flecs::entity entity, FEntityManager dstManager) override;
private:
	GENERATED_BODY()
	
};
