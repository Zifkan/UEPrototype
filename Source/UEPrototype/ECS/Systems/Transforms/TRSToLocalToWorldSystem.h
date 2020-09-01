#pragma once
#include "Components/TransformComponents.h"
#include "Systems/SystemBase.h"

class TRSToLocalToWorldSystem  final : public SystemBase< LocalToWorld, Rotation, Translation>
{
public:
    void OnCreate() override;
};
