#include "CopyTransformToActorSystem.h"


void CopyTransformToActorSystem::OnCreate()
{
    SystemRun->each([](flecs::entity e, LocalToWorld& localToWorld, FActorComponent& actor)
    {        
        actor.ptr->SetActorTransform(FTransform(localToWorld.Value));
    });
}
