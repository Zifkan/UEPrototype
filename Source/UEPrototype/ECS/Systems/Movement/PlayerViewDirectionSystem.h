#pragma once
#include "Components/PlayerComponents.h"
#include "Systems/SystemBase.h"

struct ViewDirectionData;

class PlayerViewDirectionSystem  final : public SystemBase<ViewDirectionData>
{
public:
    PlayerViewDirectionSystem() : SystemBase("PlayerViewDirectionSystem"){}
   
    void OnCreate() override;
};
