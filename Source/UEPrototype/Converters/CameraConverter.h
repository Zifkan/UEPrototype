// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "ConvertToEntity.h"
#include "CameraConverter.generated.h"


UCLASS(ClassGroup = (ECS), meta = (BlueprintSpawnableComponent))
class UEPROTOTYPE_API UCameraConverter : public UConvertToEntity
{
public:
    void Convert(flecs::entity entity, FEntityManager dstManager) override;
private:
	GENERATED_BODY()

public:
    FVector2D DeltaRotation = FVector2D(0);

    UPROPERTY(EditAnywhere, Category = "ECS")
    float HeightOffset = 3.0f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float FocusHeightOffset = 3.0f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float Radius = 5.0f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float CurrentRadius = 0.0f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float SpeedX = 80.0f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float SpeedY = 80.0f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float yMinLimit = -70.0f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float yMaxLimit = 70.0f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float PitchLimit = -35.f;

    UPROPERTY(EditAnywhere, Category = "ECS")
    float ProbeSize;

    /** Collision channel of the query probe (defaults to ECC_Camera) */
    UPROPERTY(EditAnywhere, Category = "ECS")
    TEnumAsByte<ECollisionChannel> ProbeChannel;



};
