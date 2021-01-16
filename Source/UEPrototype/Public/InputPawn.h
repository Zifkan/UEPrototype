// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"
#include "Core/EntityManager.h"
#include "GameFramework/Pawn.h"
#include "UEPrototype/ECS/Components/InputComponent.h"
#include "UEPrototype/ECS/Core/EcsWorld.h"
#include "InputPawn.generated.h"

UCLASS()
class UEPROTOTYPE_API AInputPawn : public APawn
{
	GENERATED_BODY()

public:
	// Sets default values for this pawn's properties
	AInputPawn();

protected:
	// Called when the game starts or when spawned
	virtual void BeginPlay() override;


    void SetInput() ;
    void MoveForward(float Value);
    void MoveRight(float Value);
    void TurnAtRate(float Rate);
	void Roll();
	void SprintPressed();
	void SprintReleased();	
	void Attack();
	void Block();
	void LookUpAtRate(float Rate);
    void AddControllerPitchInput(float Val);
    void AddControllerYawInput(float Val);

public:	


	// Called to bind functionality to input
	virtual void SetupPlayerInputComponent(class UInputComponent* PlayerInputComponent) override;


	virtual void Tick(float DeltaSeconds) override;
private:
    Entity inputEntity;
	PlayerInputComponent inputData;
    FEcsWorld* world;
    FEntityManager* entityManager;

	bool isSprint;
};
