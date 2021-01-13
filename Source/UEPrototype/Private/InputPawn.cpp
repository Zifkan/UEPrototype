// Fill out your copyright notice in the Description page of Project Settings.


#include "InputPawn.h"

#include "MainGameMode.h"
#include "Components/CharacterActions/CharacterActionsComponents.h"

// Sets default values
AInputPawn::AInputPawn()
{
 	// Set this pawn to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;

}

// Called when the game starts or when spawned
void AInputPawn::BeginPlay()
{
    world = FEcsWorld::instance();
    entityManager = world->EntityManager;
	Super::BeginPlay(); 
}

// Called to bind functionality to input
void AInputPawn::SetupPlayerInputComponent(UInputComponent* PlayerInputComponent)
{
	Super::SetupPlayerInputComponent(PlayerInputComponent);

    // Set up gameplay key bindings
    check(InputComponent);
    InputComponent->BindAxis("MoveForward", this, &AInputPawn::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AInputPawn::MoveRight);

    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
 
    InputComponent->BindAxis("LookUp", this, &AInputPawn::AddControllerPitchInput);
    InputComponent->BindAxis("Turn", this, &AInputPawn::AddControllerYawInput);


    InputComponent->BindAction("Roll", EInputEvent::IE_Pressed, this, &AInputPawn::Roll);
    InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AInputPawn::Sprint);

    InputComponent->BindAction("Attack", EInputEvent::IE_Pressed, this, &AInputPawn::Attack);
    InputComponent->BindAction("Sprint", EInputEvent::IE_Pressed, this, &AInputPawn::Block);
}

void AInputPawn::Tick(float DeltaSeconds)
{
    if (inputData.MovementAxis.SizeSquared()>0)
    {
        inputEntity = entityManager->Singleton<InputEntityType>();
        entityManager->AddComponentDataSafety<MoveInputTag>(inputEntity);

      //  UE_LOG(LogTemp, Warning, TEXT("Pressing movement MoveInputTag, %i"),inputEntity.id());
        SetInput() ;
    }
}

void AInputPawn::SetInput() 
{
    inputEntity = entityManager->Singleton<InputEntityType>();
    entityManager->SetComponentData<PlayerInputComponent>(inputEntity, { inputData });
}


void AInputPawn::MoveForward(float Value)
{
    inputData.MovementAxis = FVector2D(inputData.MovementAxis.X,Value);
  //  SetInput();
}

void AInputPawn::MoveRight(float Value)
{
    inputData.MovementAxis = FVector2D(Value,inputData.MovementAxis.Y);
   // SetInput();
}


void AInputPawn::Roll()
{
 /*   inputEntity = Entity(*world->DefaultWorld, "InputEntity");  
    entityManager->AddComponentDataSafety<RollInputTag>(inputEntity);*/
}

void AInputPawn::Sprint()
{
  /*  inputEntity = Entity(*world->DefaultWorld, "InputEntity");  
    entityManager->AddComponentDataSafety<SprintInputTag>(inputEntity);*/
}

void AInputPawn::Attack()
{
    inputEntity = entityManager->Singleton<InputEntityType>();
    
 //   UE_LOG(LogTemp, Warning, TEXT("AInputPawn Input Entity, %i"),inputEntity.id());
    
    entityManager->AddComponentDataSafety<AttackInputTag>(inputEntity);
  //  entityManager->AddComponentDataSafety<InputTag>(inputEntity);
}

void AInputPawn::Block()
{
   /* inputEntity = Entity(*world->DefaultWorld, "InputEntity");  
    entityManager->AddComponentDataSafety<BlockInputTag>(inputEntity);*/
}


// Y= Pitch
void AInputPawn::AddControllerPitchInput(float Val)
{
    //UE_LOG(LogTemp, Warning, TEXT("Text, x = %f"), Val);
  
    inputData.AimAxis = FVector2D( Val,inputData.AimAxis.Y);
    SetInput();
}

// Z= Yaw
void AInputPawn::AddControllerYawInput(float Val)
{
   // UE_LOG(LogTemp, Warning, TEXT("Text, x = %f"), Val);
    inputData.AimAxis = FVector2D( inputData.AimAxis.X, Val);
   SetInput();
}

