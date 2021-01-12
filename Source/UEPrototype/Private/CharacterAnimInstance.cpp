// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterAnimInstance.h"

void UCharacterAnimInstance::PlayAnim()
{
  /* if (Montage_IsPlaying(animMontage)) return;
   auto t =  Montage_Play(animMontage);*/

 IsAttack = true;
   
  // UE_LOG(LogTemp, Warning, TEXT("AttackState Working PLay Time: %f"),t);
}
