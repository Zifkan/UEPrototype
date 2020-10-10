#include "AnimationInitializeSystem.h"

#include "RendererGroup.h"


void AnimationInitializeSystem::OnCreate()
{
    rendererGroup = RendererGroup::instance();
    
    SystemRun->signature("!RemoveRenderer").each([this](flecs::entity e, RendererRoot& rootRendererData,CharacterActorComponent& actor)
    {
        auto groupData = RegisterRendererInGroup(e);
        auto rootRenderId = groupData.RootRenderId;

        actor.Character->SetIndex(rootRenderId); //TODO Set render index to material


        rootRendererData.RenderEntityListId = rootRenderId;

        e.set<RemoveRenderer>({rootRenderId});
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
        

