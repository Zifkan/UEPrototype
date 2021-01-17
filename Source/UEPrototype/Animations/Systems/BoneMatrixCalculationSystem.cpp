#include "BoneMatrixCalculationSystem.h"
#include "RendererGroup.h"
#include "Components/CustomBone.h"
#include "Components/TransformComponents.h"

void BoneMatrixCalculationSystem::OnCreate()
{
    rendererGroup = RendererGroup::instance();
    
    SystemRun->each([this](flecs::entity e, RendererRoot& rendererRoot)
   {
        for (auto children : e.children())
        {
            for (auto i : children)
            {
                auto boneEntity =  children.entity(i);  

                auto localToWorld =  boneEntity.get<LocalToWorld>()->Value;
                auto bindMatrix = boneEntity.get<CustomBone>()->BindMatrix;

                const auto renderEntityListId = rendererRoot.RenderEntityListId;
                const auto result = localToWorld *bindMatrix;

                rendererGroup->Buffer[renderEntityListId * 256 + i] = result;
            }
        }
   }).kind(0);
}
