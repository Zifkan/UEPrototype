#pragma once
#include "RendererGroup.h"
#include "Systems/SystemBase.h"

class BufferFillSystem :public SystemBase<>
{
public:
    virtual void OnCreate() override;
    

private:
    void SetComputeBuffer() const;

    RendererGroup* pGroupData;  
    
};
