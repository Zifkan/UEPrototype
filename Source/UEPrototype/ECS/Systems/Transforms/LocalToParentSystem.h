#pragma once

#include "Systems/SystemBase.h"

class LocalToParentSystem final : public SystemBase<>
{
public:
    void OnCreate() override;
};
