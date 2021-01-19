#pragma once

#include "Systems/SystemBase.h"

class LocalToParentSystem final : public SystemBase<>
{
public:
    LocalToParentSystem() : SystemBase("LocalToParentSystem"){}    

    void OnCreate() override;
};
