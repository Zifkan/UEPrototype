#include "CopyTransformFromActorSystem.h"

#include "ECSPlugin/ECS/Components/GeneralComponents.h"

void CopyTransformFromActorSystem::OnCreate()
{
    SystemRun->each([](flecs::entity e, CopyTransformFromActor& v, LocalToWorld& localToWorld, FActorComponent& actorComponent)
    {
        localToWorld.Value = actorComponent.ptr->GetActorTransform().ToMatrixWithScale();
    });
}
