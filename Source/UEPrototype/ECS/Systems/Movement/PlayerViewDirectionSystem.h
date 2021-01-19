#pragma once
#include "Systems/SystemBase.h"

struct ViewDirectionData;

class PlayerViewDirectionSystem  final : public SystemBase<ViewDirectionData>
{
public:
    PlayerViewDirectionSystem() : SystemBase("PlayerViewDirectionSystem"){}
   
    void OnCreate() override;
};
