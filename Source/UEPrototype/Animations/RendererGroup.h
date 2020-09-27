#pragma once
#include "Core/EntityManager.h"

#include "RHIUtilities.h"

class RendererGroup 
{
public:

    RendererGroup()
    {
      /*  DeformTransformsSB = RHICreateStructuredBuffer(sizeof(FMatrix), NumSections * sizeof(FMatrix), BUF_ShaderResource, CreateInfo);
        DeformTransformsSB.*/
    }

    

    TArray<Entity> RenderEntityList;    
    TQueue<int> FreeIndexes;
        
    TArray<FMatrix> Buffer;


    //The structured buffer that will contain all the deform transoform and going to be used as a shader resource
 //   FStructuredBufferRHIRef DeformTransformsSB;

    //The shader resource view of the structured buffer, this is what we bind to the vertex factory shader
//    FShaderResourceViewRHIRef DeformTransformsSRV;
    
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
