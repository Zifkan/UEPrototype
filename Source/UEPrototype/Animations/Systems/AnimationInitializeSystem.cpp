#include "AnimationInitializeSystem.h"

#include "RendererGroup.h"


void AnimationInitializeSystem::OnCreate()
{
    rendererGroup = RendererGroup::instance();
    
    SystemRun->each([this](flecs::entity e, RendererRoot& rootRendererData)
    {
        auto groupData = RegisterRendererInGroup(e);
        auto rootRenderId = groupData.RootRenderId;

        //rootRendererData.SetIndex(rootRenderId); TODO Set render index to material

//TODO Set render index to material remove mechanism
     /*   PostUpdateCommands.AddSharedComponent(customRendererEntity, new RemoveRenderer
        {
            CustomRendererIndex = rootRenderId,

        });*/

        rootRendererData.RenderEntityListId = rootRenderId;
    });
}

RendererGroupData AnimationInitializeSystem::RegisterRendererInGroup(Entity customRendererEntity) const
{
            
    if (rendererGroup->FreeIndexes.IsEmpty())
    {
        rendererGroup->RenderEntityList.Add(customRendererEntity);
        return RendererGroupData(rendererGroup->RenderEntityList.Num() - 1);
    }

   
    int idInList;
    rendererGroup->FreeIndexes.Dequeue(idInList);
    rendererGroup->RenderEntityList[idInList] = customRendererEntity;
    return RendererGroupData(idInList);
}
        

