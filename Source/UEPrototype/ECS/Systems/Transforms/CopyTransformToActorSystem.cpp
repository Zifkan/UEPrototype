#include "CopyTransformToActorSystem.h"


void CopyTransformToActorSystem::OnCreate()
{
    SystemRun->signature("CopyTransformToActor").each([](flecs::entity e, LocalToWorld& localToWorld, FActorComponent& actor)
    {
        actor.ptr->SetActorTransform(FTransform(localToWorld.Value));
     //   actor.ptr->SetActorRotation(localToWorld.Rotation());
    });
}
