#pragma once
#include "Systems/SystemBase.h"
#include "Components/CamComponent.h"
#include "Components/GeneralComponents.h"
#include "Components/TransformComponents.h"

class CameraSystem final : public SystemBase<CamComponent, Rotation , Translation>
{
public:
    void OnCreate() override;

    float ClampAngle(float angle, float min, float max);

};


class CameraCollisionSystem final : public SystemBase<CamComponent, FActorComponent, Translation>
{
public:
    void OnCreate() override;
    FVector BlendLocations(const FVector& DesiredArmLocation, const FVector& TraceHitLocation, bool bHitSomething);
};
