#pragma once

#include "CoreMinimal.h"
#include "BoneData.generated.h" 

USTRUCT(BlueprintType)
struct FBoneDataInfo
{
    GENERATED_BODY()

    UPROPERTY(BlueprintReadOnly,EditAnywhere)
    FTransform BoneTransform;

    UPROPERTY(BlueprintReadOnly,EditAnywhere)
    FMatrix BindMatrix;
};
