#pragma once
#include "ECSPlugin/ECS/Components/TransformComponents.h"
#include "ECSPlugin/ECS/Systems/SystemBase.h"

class TRSToLocalToWorldSystem  final : public SystemBase< LocalToWorld, Rotation, Translation>
{
public:
    void OnCreate() override;
};
