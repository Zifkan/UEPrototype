#pragma once
#include "Core/EntityManager.h"

#include "RHIUtilities.h"

class RendererGroup 
{
public:
  

    TArray<Entity> RenderEntityList;    
    TQueue<int> FreeIndexes;
        
    TArray<FMatrix> Buffer;


    //The structured buffer that will contain all the calculated Matrices and going to be used as a shader resource
    FStructuredBufferRHIRef BindMatricesSB;

    //The shader resource view of the structured buffer, this is what we bind to the vertex factory shader
    FShaderResourceViewRHIRef BindMatricesSRV;
   

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
