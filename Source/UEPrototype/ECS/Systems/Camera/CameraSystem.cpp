#include "CameraSystem.h"

#include "Components/GeneralComponents.h"
#include "Components/PlayerComponents.h"
#include "Components/CharacterActions/CharacterActionsComponents.h"
#include "UEPrototype/ECS/Components/InputComponent.h"

void CameraSystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e, CamComponent& camera, Rotation& camRotation,Translation& camTranslation)
    {
        auto inputEnt =  GetEntityManager()->Singleton<InputEntityType>();
      
        auto player =  GetEntityManager()->Singleton<PlayerEntityType>();
       
      
        const LocalToWorld* playerLocalToWorld = player.get<LocalToWorld>();
        const PlayerInputComponent* input = inputEnt.get<PlayerInputComponent>();

        
        if(playerLocalToWorld ==nullptr ||  input == nullptr) return;

        const auto frameTime = GetDeltaTime();

        FVector2D vec2 = FVector2D(FMath::Clamp<float>(input->AimAxis.Y, -1, 1), FMath::Clamp<float>(input->AimAxis.X, -1, 1));
       

        if (FMath::Abs(vec2.X) < 0.4f  &&  FMath::Abs(vec2.Y) > FMath::Abs(vec2.X))
            vec2 = FVector2D(0, vec2.Y);
        if (FMath::Abs(vec2.Y) < 0.4f  &&   FMath::Abs(vec2.X) > FMath::Abs(vec2.Y))
            vec2 = FVector2D(vec2.X, 0);

        auto  deltaRotation = vec2;

        const auto heightOffset = camera.HeightOffset;
        const auto yMinLimit = camera.yMinLimit;
        const auto yMaxLimit = camera.yMaxLimit;
        const auto m_pitchLimit = camera.m_pitchLimit;
        const auto xSpeed = camera.xSpeed;
        const auto ySpeed = camera.ySpeed;
        const auto currentRadius = camera.currentRadius;

        AActor* m_pTarget  = nullptr;

        
        float x = camera.x;
        float y = camera.y;

        if (m_pTarget!=nullptr)
        {
            const auto target = m_pTarget->GetActorTransform().GetTranslation();
            const auto location = playerLocalToWorld->Position() + FVector(0, 0, heightOffset);

            x = target.X - location.X;
            y = target.Y - location.Y;

        //    auto lookDir = FVector(x, y, target.Z - location.Z).Normalize();
            camRotation.Value = FQuat::FindBetween(target,location);

        
        }
        else
        {
            x += deltaRotation.X * xSpeed * currentRadius * frameTime;
            y -= deltaRotation.Y * ySpeed * frameTime;

            y = ClampAngle(y, yMinLimit, yMaxLimit);
        
            camRotation.Value = FQuat::MakeFromEuler(FVector(0, (y), (x)));
        }       


        camera.x = x;
        camera.y = y;

        const auto negDistance = FVector(-currentRadius, 0.0f,  0);
        const auto position = camRotation.Value * negDistance + (playerLocalToWorld->Position() + FVector(0, 0,heightOffset));
      
        camTranslation.Value = position;
    }).kind(0);
}

float CameraSystem::ClampAngle(float angle, float min, float max)
{
    if (angle < -360)
        angle += 360;
    if (angle > 360)
        angle -= 360;
    return FMath::Clamp<float>(angle, min, max);
}

void CameraCollisionSystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e, CamComponent& camera,FActorComponent& owner, Translation& camTranslation)
    {
        auto player = m_pWorld->DefaultWorld->singleton<PlayerEntityType>();     
              
        const LocalToWorld* playerLocalToWorld = player.get<LocalToWorld>();
     
        if (playerLocalToWorld == nullptr) return;
            
        const auto heightOffset = camera.HeightOffset;
              
        FVector ArmOrigin = playerLocalToWorld->Position() + FVector(0, 0, heightOffset);
        
        FVector DesiredLoc = camTranslation.Value;

        FCollisionQueryParams QueryParams(SCENE_QUERY_STAT(SpringArm), false, owner.ptr.Get());

        FHitResult Result;
        GetUWorld()->SweepSingleByChannel(Result, ArmOrigin, DesiredLoc, FQuat::Identity, camera.ProbeChannel, FCollisionShape::MakeSphere(camera.ProbeSize), QueryParams);

       
        camTranslation.Value = BlendLocations(DesiredLoc, Result.Location, Result.bBlockingHit);
    }).kind(0);
}


FVector CameraCollisionSystem::BlendLocations(const FVector& DesiredArmLocation, const FVector& TraceHitLocation, bool bHitSomething)
{
    return bHitSomething ? TraceHitLocation : DesiredArmLocation;
}


