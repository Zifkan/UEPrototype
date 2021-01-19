#pragma once
#include "Systems/SystemBase.h"

class RendererGroup;

class BufferSystem final :public SystemBase<>
{
public:
    BufferSystem() : SystemBase("BufferSystem"){}
   
    virtual void OnCreate() override;
    
private:    
    RendererGroup* pGroupData;
};
