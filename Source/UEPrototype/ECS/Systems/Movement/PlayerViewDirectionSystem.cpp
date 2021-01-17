#include "PlayerViewDirectionSystem.h"


#include "Components/MovementComponents.h"
#include "Components/TransformComponents.h"

void PlayerViewDirectionSystem::OnCreate()
{
    SystemRun->signature("PlayerTag").each([this](Entity e, ViewDirectionData& viewDirection)
    {
        const auto s = flecs::entity(*m_pWorld->DefaultWorld, "Camera");
        auto* camLocalToWorld = s.get<WorldToLocal>();

        if (camLocalToWorld ==nullptr) return;

        viewDirection.Axises = camLocalToWorld->Forward();
    }).kind(0);
}
