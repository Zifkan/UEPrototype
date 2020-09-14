// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActor.h"

// Sets default values
ACharacterActor::ACharacterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = false;

    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

	Mesh =  CreateDefaultSubobject<UStaticMeshComponent>(TEXT("Mesh"));
	Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ACharacterActor::BeginPlay()
{
	Super::BeginPlay();
	
}
