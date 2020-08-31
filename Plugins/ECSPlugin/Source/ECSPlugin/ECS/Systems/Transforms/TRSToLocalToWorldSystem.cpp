#include "TRSToLocalToWorldSystem.h"

#include "Math/ScaleRotationTranslationMatrix.h"

void TRSToLocalToWorldSystem::OnCreate()
{
    SystemRun->each([](flecs::entity e,LocalToWorld& localToWorld, Rotation& rot, Translation& translation)
    {
        localToWorld.Value = FScaleRotationTranslationMatrix(FVector(1,1,1), FRotator(rot.Value), translation.Value);
    });
}
