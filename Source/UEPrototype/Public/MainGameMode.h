// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/EcsWorld.h"
#include "GameFramework/GameModeBase.h"
#include "Systems/SystemLauncher.h"
#include "Systems/Transforms/TransformSystemLaunch.h"
#include "UEPrototype/ECS/Components/InputComponent.h"



#include "MainGameMode.generated.h"

/**
 * 
 */
UCLASS()
class UEPROTOTYPE_API AMainGameMode : public AGameModeBase
{
public:
    AMainGameMode()
    {
        PrimaryActorTick.bStartWithTickEnabled = true;
        PrimaryActorTick.bCanEverTick = true;

    }

    void Tick(float DeltaSeconds) override;
    void SetupPlayerInputComponent();
    void SetInput() const;

protected:
    void BeginPlay() override;

private:
	GENERATED_BODY()

    void RegisterSystem();
    void RegisterTransformSystems();

    void MoveForward(float Value);  
    void MoveRight(float Value); 
    void TurnAtRate(float Rate);
    void LookUpAtRate(float Rate);
    void AddControllerPitchInput(float Val);
    void AddControllerYawInput(float Val);
private:

    FEcsWorld* world;
    TUniquePtr<SystemLauncher> systemsLauncher;
    TUniquePtr < TransformSystemLaunch> transformLauncher;
    FEntityManager* entityManager;
    Entity inputEntity;
    PlayerInputComponent inputData;
};
