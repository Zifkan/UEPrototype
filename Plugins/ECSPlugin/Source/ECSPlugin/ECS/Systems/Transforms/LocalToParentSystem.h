#pragma once

#include "ECSPlugin/ECS/Systems/SystemBase.h"

class LocalToParentSystem final : public SystemBase<>
{
public:
    void OnCreate() override;
};
