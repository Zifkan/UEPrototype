// Fill out your copyright notice in the Description page of Project Settings.


#include "CharacterActor.h"


#include "EngineUtils.h"
#include "flecs.h"
#include "Components/CustomBone.h"
#include "Components/RendererRoot.h"
#include "Components/TransformComponents.h"
#include "Core/EcsWorld.h"

// Sets default values
ACharacterActor::ACharacterActor()
{
 	// Set this actor to call Tick() every frame.  You can turn this off to improve performance if you don't need it.
	PrimaryActorTick.bCanEverTick = true;
	
    Root = CreateDefaultSubobject<USceneComponent>(TEXT("Root"));
    RootComponent = Root;

	//AnimationVertexMeshComponent = CreateDefaultSubobject<UAnimVertexMeshComponent>(TEXT("Animation Vertex Mesh Component"));
	//AnimationVertexMeshComponent->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);

	Mesh = CreateDefaultSubobject<USkeletalMeshComponent>(TEXT("Animation Vertex Mesh Component"));
	Mesh->AttachToComponent(Root, FAttachmentTransformRules::KeepRelativeTransform);
}

// Called when the game starts or when spawned
void ACharacterActor::BeginPlay()
{
	Super::BeginPlay();
	
	auto* world = FEcsWorld::instance();
	auto em = world->EntityManager;
	auto characterEntity = world->EntityManager->CreateEntity();

	characterEntity.add<RendererRoot>();
	characterEntity.set<CharacterActorComponent>({this});
	
	for (int i = 0; i < BoneInfoArray.Num(); i++)
	{       
		auto entityBone = world->EntityManager->CreateEntity();

       
		em->SetComponentData<Translation>(entityBone,{});
		em->SetComponentData<LocalToWorld>(entityBone, {});    
		em->SetComponentData<CustomBone>(entityBone, { BoneInfoArray[i].BindMatrix });
		entityBone.add_childof(characterEntity);   
	}
	
	
}

void ACharacterActor::Tick(float DeltaSeconds)
{
	if (AnimVertexMeshComponent== nullptr)
	{
		auto actor =DestinationActor.Get();

		if (actor==nullptr) return;
		
		auto component =  actor->FindComponentByClass<UAnimVertexMeshComponent>();
		if(component!=nullptr)
		{
			AnimVertexMeshComponent =component;
		}
	
	}
	
}

void ACharacterActor::SetIndex(int index)
{
	
}

void ACharacterActor::SetSetBuffer(TArray<FMatrix> buffer)
{
	if(AnimVertexMeshComponent==nullptr)return;
 	//AnimVertexMeshComponent->UpdateBoneArray(buffer);
}


UAnimInstance* ACharacterActor::GetAnimInstance() const
{
	return Mesh->GetAnimInstance();
}

