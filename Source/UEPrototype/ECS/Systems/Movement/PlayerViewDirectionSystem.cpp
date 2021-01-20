#include "PlayerViewDirectionSystem.h"
#include "Components/CamComponent.h"
#include "Components/MovementComponents.h"
#include "Components/TransformComponents.h"

void PlayerViewDirectionSystem::OnCreate()
{
    SystemRun->each([this](Entity e, ViewDirectionData& viewDirection)
    {
        const auto s = m_pWorld->DefaultWorld->singleton<CamEntityType>();
        auto* camLocalToWorld = s.get<WorldToLocal>();

        if (camLocalToWorld ==nullptr) return;
        
        viewDirection.Axises = camLocalToWorld->Forward();       
       
    }).kind(0);    
}
