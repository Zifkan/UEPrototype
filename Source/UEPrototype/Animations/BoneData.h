#pragma once

#include "CoreMinimal.h"
#include "BoneData.generated.h" 

USTRUCT(BlueprintType)
struct FBoneDataInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly)
    FTransform BoneTransform;

    UPROPERTY(BlueprintReadOnly)
    FMatrix BindMatrix;
};
