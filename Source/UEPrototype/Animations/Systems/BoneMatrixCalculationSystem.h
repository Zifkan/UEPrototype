#pragma once
#include "RendererGroup.h"
#include "Components/RendererRoot.h"
#include "Systems/SystemBase.h"

class  BoneMatrixCalculationSystem final :public SystemBase<RendererRoot,CharacterActorComponent>
{
public:
    BoneMatrixCalculationSystem() : SystemBase("BoneMatrixCalculationSystem"){}
   
    virtual void OnCreate() override;
    void UpdateRefToLocalMatricesInner(CharacterActorComponent& characterRender,const TArray<FBoneIndexType>* ExtraRequiredBoneIndices);
private:    
    RendererGroup* rendererGroup;
};
