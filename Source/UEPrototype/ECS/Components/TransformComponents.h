#pragma once

#include "Math/Quat.h"
#include "Math/Vector.h"


struct Translation
{
    FVector Value;
};

struct Rotation
{
    FQuat Value;
};

struct Scale
{
    FVector Value;
};


struct LocalToWorld
{
   FMatrix Value;

   FVector Right()    const { return FVector(Value.GetColumn(0).X, Value.GetColumn(0).Y, Value.GetColumn(0).Z); }
   FVector Up()       const { return FVector(Value.GetColumn(1).X, Value.GetColumn(1).Y, Value.GetColumn(1).Z); }
   FVector Forward()  const { return FVector(Value.GetColumn(2).X, Value.GetColumn(2).Y, Value.GetColumn(2).Z); }  
   FVector Position() const { return FVector(Value.GetColumn(3).X, Value.GetColumn(3).Y, Value.GetColumn(3).Z); }

   FQuat Rotation() const { return FQuat(Value); }
};


struct WorldToLocal
{
    FMatrix Value;

    FVector Right()    const { return FVector(Value.GetColumn(0).X, Value.GetColumn(0).Y, Value.GetColumn(0).Z); }
    FVector Up()       const { return FVector(Value.GetColumn(1).X, Value.GetColumn(1).Y, Value.GetColumn(1).Z); }
    FVector Forward()  const { return FVector(Value.GetColumn(2).X, Value.GetColumn(2).Y, Value.GetColumn(2).Z); }
    FVector Position() const { return FVector(Value.GetColumn(3).X, Value.GetColumn(3).Y, Value.GetColumn(3).Z); }

    FQuat Rotation() const { return FQuat(Value); }
};

struct LocalToParent
{
    FMatrix Value;

    FVector Right()    const { return FVector(Value.GetColumn(0).X, Value.GetColumn(0).Y, Value.GetColumn(0).Z); }
    FVector Up()       const { return FVector(Value.GetColumn(1).X, Value.GetColumn(1).Y, Value.GetColumn(1).Z); }
    FVector Forward()  const { return FVector(Value.GetColumn(2).X, Value.GetColumn(2).Y, Value.GetColumn(2).Z); }
    FVector Position() const { return FVector(Value.GetColumn(3).X, Value.GetColumn(3).Y, Value.GetColumn(3).Z); }

    FQuat Rotation() const { return FQuat(Value); }
};

struct CopyTransformToActor
{

    
};

struct CopyTransformFromActor
{

};
