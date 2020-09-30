#pragma once
#include "RendererGroup.h"
#include "Systems/SystemBase.h"

class BufferFillSystem : SystemBase<>
{
public:
    virtual void OnCreate() override;
    void UpdateBoneMatrixBufferSB_RenderThread() const;
    RendererGroup* rendererGroup;
};
