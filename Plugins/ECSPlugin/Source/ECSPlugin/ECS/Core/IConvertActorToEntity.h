#pragma once

#include "EntityManager.h"

class IConvertActorToEntity
{
public:   

    virtual  void Convert(flecs::entity entity, FEntityManager dstManager) = 0;
};
