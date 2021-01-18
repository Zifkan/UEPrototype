// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Animation/AnimInstance.h"
#include "UObject/ObjectMacros.h"

#include "CharacterAnimInstance.generated.h"

/**
 * 
 */
UCLASS()
class UEPROTOTYPE_API UCharacterAnimInstance : public UAnimInstance
{
	GENERATED_BODY()
public:
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Anim)
	FVector2D MoveDirection;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Anim)
	bool IsRoll;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Anim)
	bool IsAttack;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Anim)
	bool IsHitted;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Anim)
	bool IsBlocking;
	
	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Anim)
	TArray<UAnimSequence*> animSequence;

	UPROPERTY(EditAnywhere,BlueprintReadWrite,Category=Anim)
	UAnimMontage* animMontage;
	
	void PlayAttack();
};
