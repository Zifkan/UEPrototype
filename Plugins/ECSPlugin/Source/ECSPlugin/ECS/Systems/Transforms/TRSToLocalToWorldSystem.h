#pragma once
#include "ECSPlugin/ECS/Components/TransformComponents.h"
#include "ECSPlugin/ECS/Systems/SystemBase.h"

class TRSToLocalToWorldSystem  final : public SystemBase
{
public:
    void OnCreate() override;
};
