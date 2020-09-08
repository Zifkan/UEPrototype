#include "CopyTransformFromActorSystem.h"
#include "Components/GeneralComponents.h"

void CopyTransformFromActorSystem::OnCreate()
{
    SystemRun->signature("CopyTransformFromActor").each([](flecs::entity e, LocalToWorld& localToWorld, FActorComponent& actorComponent)
    {
        localToWorld.Value = actorComponent.ptr->GetTransform().ToMatrixWithScale();
    });
}
