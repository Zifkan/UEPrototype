#pragma once
#include "RendererGroup.h"
#include "Systems/SystemBase.h"

class BufferFillSystem :public SystemBase<>
{
public:
    BufferFillSystem() : SystemBase("BufferFillSystem"){}
   
    virtual void OnCreate() override;
    

private:
    void SetComputeBuffer() const;

    RendererGroup* pGroupData;  
    
};
