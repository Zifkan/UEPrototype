﻿#include "InputMoveProcessingSystem.h"

#include "Components/CharacterActions/CharacterActionsComponents.h"
#include "UEPrototype/ECS/Components/InputComponent.h"

void InputMoveProcessingSystem::OnCreate()
{
    SystemRun->signature("PlayerTag").each([this](flecs::entity e,MoveDirectionData& moveDirection, ViewDirectionData& viewDirection)
    {
      
        auto inputEnt =  GetEntityManager()->Singleton<InputEntityType>();
        const PlayerInputComponent* input = inputEnt.get<PlayerInputComponent>();

        if (input==nullptr) return;

        auto dir = viewDirection.Axises; 
        dir.Z=0;
        dir = dir.GetSafeNormal();
        
        const auto moveDir = input->MovementAxis.Y * dir + input->MovementAxis.X * FVector(-dir.Y, dir.X, dir.Z);
        
        moveDirection.Axises = FVector::DotProduct(moveDir,moveDir) > 1 ?  moveDir.GetSafeNormal() : moveDir;
        moveDirection.Axises.Z = 0;

//        UE_LOG(LogTemp, Warning, TEXT(" moveDirection.Axises: x = %f; y = %f"),  moveDirection.Axises.X, moveDirection.Axises.Y);
          
    }).kind(0);
}
