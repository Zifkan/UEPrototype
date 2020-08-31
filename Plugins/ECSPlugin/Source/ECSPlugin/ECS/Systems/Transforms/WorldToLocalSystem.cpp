#include "WorldToLocalSystem.h"

void WorldToLocalSystem::OnCreate()
{
    SystemRun->each([](flecs::entity e, LocalToWorld& localToWorld, WorldToLocal& worldToLocal)
    {
        worldToLocal.Value = localToWorld.Value.Inverse();
    });
}
