#include "InputMoveProcessingSystem.h"
#include "UEPrototype/ECS/Components/InputComponent.h"

void InputMoveProcessingSystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e, PlayerTag& player, MoveDirectionData& moveDirection, ViewDirectionData& viewDirection)
    {
        auto inputEnt = Entity(*m_pWorld->DefaultWorld, "InputEntity");  
        const PlayerInputComponent* input = inputEnt.get<PlayerInputComponent>();

        if (input==nullptr) return;

        auto dir = viewDirection.Axises;      

        const auto moveDir = input->MovementAxis.Y * dir + input->MovementAxis.X * FVector(-dir.Y, dir.X, dir.Z);
        
        moveDirection.Axises = FVector::DotProduct(moveDir,moveDir) > 1 ?  moveDir.GetSafeNormal() : moveDir;
        moveDirection.Axises.Z = 0;
    });
}
