#include "CameraSystem.h"
#include "UEPrototype/ECS/Components/PlayerComponents.h"
#include "UEPrototype/ECS/Components/InputComponent.h"

void CameraSystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e, CamComponent& camera, Rotation& camRotation,Translation& camTranslation)
    {
        auto player = Entity(*m_pWorld->DefaultWorld, "Player");
        auto inputEnt = Entity(*m_pWorld->DefaultWorld, "InputEntity");
        const Translation* playerTranslation = player.get<Translation>();
        const PlayerInputComponent* input = inputEnt.get<PlayerInputComponent>();

        
        if(playerTranslation ==nullptr ||  input == nullptr) return;

      
     
        const auto frameTime = GetDeltaTime();

        FVector2D vec2 = FVector2D(FMath::Clamp<float>(input->AimAxis.X, -1, 1), FMath::Clamp<float>(input->AimAxis.Y, -1, 1)) ;

       
       

        if (FMath::Abs(vec2.X) < 0.4f  &&  FMath::Abs(vec2.Y) > FMath::Abs(vec2.X))
            vec2 = FVector2D(0, vec2.Y);
        if (FMath::Abs(vec2.Y) < 0.4f  &&   FMath::Abs(vec2.Y) > FMath::Abs(vec2.Y))
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
            const auto location = playerTranslation->Value + FVector(0, 0, heightOffset);

            x = target.X - location.X;
            y = target.Y - location.Y;

        //    auto lookDir = FVector(x, y, target.Z - location.Z).Normalize();
            camRotation.Value = FQuat::FindBetween(target,location);

        
        }
        else
        {
            x -= deltaRotation.X * xSpeed * currentRadius * frameTime;
            y -= deltaRotation.Y * ySpeed * frameTime;

            y = ClampAngle(y, yMinLimit, yMaxLimit);
        
            camRotation.Value = FQuat::MakeFromEuler(FVector((y), 0, (x)));
        }       


        camera.x = x;
        camera.y = y;

        const auto negDistance = FVector(0.0f, -currentRadius, 0);
        const auto position = camRotation.Value * negDistance + (playerTranslation->Value + FVector(0, 0, heightOffset));

         UE_LOG(LogTemp, Warning, TEXT("Text, x = %f, y = %f, z = %f"), position.X, position.Y, position.Z);

       camTranslation.Value = position;
    });
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
    SystemRun->each([this](flecs::entity e, CamComponent& camera)
    {
      /*  auto s = flecs::entity(*m_pWorld->DefaultWorld, flecs::Singleton);
        const PlayerTag* playerComponent = s.get<PlayerTag>();
     
        if (playerComponent == nullptr || camera.CameraEntity == nullptr) return;

        const auto frameTime = GetDeltaTime();
        const auto heightOffset = camera.HeightOffset;

        static IPhysicalEntity* pSkipEnts[10];
        pSkipEnts[0] = playerComponent->pCryEntity->GetPhysics();
        auto org = playerComponent->pCryEntity->GetWorldPos() + Vec3(0, 0, heightOffset);
        ray_hit rayhit;
        // Perform the ray cast.
        int hits = gEnv->pPhysicalWorld->RayWorldIntersection(org, camera.CameraEntity->GetWorldPos() - org,
            ent_static | ent_sleeping_rigid | ent_rigid | ent_independent | ent_terrain, rwi_stop_at_pierceable | rwi_colltype_any,
            &rayhit, 1, pSkipEnts, 2);

        if (hits)
        {
            camera.currentRadius -= frameTime;
        }
        else
        {
            if (camera.currentRadius < camera.radius)
                camera.currentRadius += frameTime;
        }*/
    });
}

