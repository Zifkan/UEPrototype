#pragma once

class FAnimationVertexSceneProxy final : public FPrimitiveSceneProxy
{
public:
    FAnimationVertexSceneProxy(UAnimationVertexMeshComponent* Component)
         : FPrimitiveSceneProxy(Component)
    {
    }
};
