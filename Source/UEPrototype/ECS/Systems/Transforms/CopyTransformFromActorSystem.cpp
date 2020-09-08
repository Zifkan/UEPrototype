#include "CopyTransformFromActorSystem.h"
#include "Components/GeneralComponents.h"

void CopyTransformFromActorSystem::OnCreate()
{
    SystemRun->signature("CopyTransformFromActor").each([](flecs::entity e, LocalToWorld& localToWorld, FActorComponent& actorComponent)
    {

       // UE_LOG(LogTemp, Warning, TEXT("Text, x = %f, y = %f, z = %f"), actorComponent.ptr->GetTransform().GetLocation().X, actorComponent.ptr->GetTransform().GetLocation().Y, actorComponent.ptr->GetTransform().GetLocation().Z);

        localToWorld.Value = actorComponent.ptr->GetActorTransform().ToMatrixWithScale();
    });
}
