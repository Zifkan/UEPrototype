// Fill out your copyright notice in the Description page of Project Settings.


#include "MainGameMode.h"
#include "Components/TransformComponents.h"
#include "Engine/InputDelegateBinding.h"
#include "UEPrototype/ECS/Components/CamComponent.h"
#include "UEPrototype/ECS/Components/MovementComponents.h"
#include "UEPrototype/ECS/Components/InputComponent.h"


void AMainGameMode::BeginPlay()
{
  /*  if (InputComponent == nullptr)
    {
        static const FName InputComponentName(TEXT("PawnInputComponent0"));
        InputComponent = NewObject<UInputComponent>(this, InputComponentName);
        if (InputComponent)
        {
            SetupPlayerInputComponent();
            InputComponent->RegisterComponent();
            InputComponent->bBlockInput = false;
            UInputDelegateBinding::BindInputDelegates(GetClass(), InputComponent);

        }
    }*/
 
    world = FEcsWorld::instance();
    entityManager = world->EntityManager;

    inputEntity = entityManager->CreateEntity("InputEntity");

    entityManager->RegisterComponent<CamComponent>("CamComponent");
    entityManager->RegisterComponent<PlayerInputComponent>("InputComponent");
    entityManager->RegisterComponent<MovementVelocity>("MovementVelocity");
    entityManager->RegisterComponent<MoveDirectionData>("MoveDirectionData");
    entityManager->RegisterComponent<MovementSpeed>("MovementSpeed"); 
    entityManager->RegisterComponent<WorldToLocal>("WorldToLocal");
    entityManager->RegisterComponent<Translation>("Translation");
    entityManager->RegisterComponent<CopyTransformToActor>("CopyTransformToActor");
    entityManager->RegisterComponent<Scale>("Scale");
    entityManager->RegisterComponent<Rotation>("Rotation");
    entityManager->RegisterComponent<FActorComponent>("ActorComponent");
   

    RegisterTransformSystems();
    RegisterSystem();
}

void AMainGameMode::RegisterSystem()
{
    systemsLauncher = MakeUnique<SystemLauncher>(world);

  /*  systemsLauncher->RegisterSystem(new  CameraCollisionSystem());
    systemsLauncher->RegisterSystem(new  CameraSystem());
    systemsLauncher->RegisterSystem(new  PlayerViewDirectionSystem());
    systemsLauncher->RegisterSystem(new  InputMoveProcessingSystem());
    systemsLauncher->RegisterSystem(new  MovementVelocitySystem());
    systemsLauncher->RegisterSystem(new  CharacterRotationSystem());

    systemsLauncher->RegisterSystem(new  MovementCharacterSystem());
    systemsLauncher->RegisterSystem(new  CharacterIdleStateMachineSystem());
    systemsLauncher->RegisterSystem(new  CharacterStateMachineSystem());*/

}

void AMainGameMode::RegisterTransformSystems()
{
    transformLauncher = MakeUnique<TransformSystemLaunch>(world);
}

void AMainGameMode::Tick(float DeltaSeconds)
{
    transformLauncher->Update(DeltaSeconds);
    systemsLauncher->Update(DeltaSeconds);
}


void AMainGameMode::SetupPlayerInputComponent()
{
    // Set up gameplay key bindings
    check(InputComponent);
    InputComponent->BindAxis("MoveForward", this, &AMainGameMode::MoveForward);
    InputComponent->BindAxis("MoveRight", this, &AMainGameMode::MoveRight);

    // We have 2 versions of the rotation bindings to handle different kinds of devices differently
    // "turn" handles devices that provide an absolute delta, such as a mouse.
    // "turnrate" is for devices that we choose to treat as a rate of change, such as an analog joystick
    InputComponent->BindAxis("Turn", this, &AMainGameMode::AddControllerYawInput);
    InputComponent->BindAxis("TurnRate", this, &AMainGameMode::TurnAtRate);
    InputComponent->BindAxis("LookUp", this, &AMainGameMode::AddControllerPitchInput);
    InputComponent->BindAxis("LookUpRate", this, &AMainGameMode::LookUpAtRate);
}

void AMainGameMode::SetInput() const
{
    entityManager->SetComponentData<PlayerInputComponent>(inputEntity, { inputData });
}


void AMainGameMode::MoveForward(float Value)
{
    inputData.MovementAxis = FVector2D(Value, inputData.MovementAxis.Y);
    SetInput();
}

void AMainGameMode::MoveRight(float Value)
{
    inputData.MovementAxis = FVector2D(inputData.MovementAxis.X, Value);
    SetInput();
}

void AMainGameMode::LookUpAtRate(float Rate)
{
    inputData.AimAxis = FVector2D(inputData.AimAxis.X, Rate);
    SetInput();
}

void AMainGameMode::TurnAtRate(float Rate)
{
    inputData.AimAxis = FVector2D(Rate, inputData.AimAxis.Y);
    SetInput();
}

// Y= Pitch
void AMainGameMode::AddControllerPitchInput(float Val) 
{
    inputData.AimAxis = FVector2D(inputData.AimAxis.X, Val);
    SetInput();
}

// Z= Yaw
void AMainGameMode::AddControllerYawInput(float Val)
{
    inputData.AimAxis = FVector2D(Val, inputData.AimAxis.Y);
    SetInput();
}