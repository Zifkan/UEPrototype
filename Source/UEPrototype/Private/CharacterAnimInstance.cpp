// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"

void UCharacterAnimInstance::PlayAttack()
{
	/* if (Montage_IsPlaying(animMontage)) return;
	auto t =  Montage_Play(animMontage);*/

	IsAttack = true;
	IsBlocking = false;
	// UE_LOG(LogTemp, Warning, TEXT("AttackState Working PLay Time: %f"),t);
}
