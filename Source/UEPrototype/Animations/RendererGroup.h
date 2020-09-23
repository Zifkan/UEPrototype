#pragma once
#include "Core/EntityManager.h"

class RendererGroup
{
public:

    TArray<Entity> RenderEntityList;    
    TQueue<int> FreeIndexes;
        
    TArray<FMatrix> Buffer;

    
    static RendererGroup* instance()
    {
        static RendererGroup* _instance = nullptr;

        if (_instance == nullptr)
        {
            _instance = new RendererGroup();
        }
        return _instance;
    }
};
