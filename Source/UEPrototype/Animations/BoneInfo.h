// Fill out your copyright notice in the Description page of Project Settings.

#pragma once

#include "CoreMinimal.h"

struct BoneInfo
{
private:
    FTransform m_boneTransform;
    FMatrix m_bindMatrix;

public:
    BoneInfo(FTransform boneTransform, FMatrix bindMatrix)
    {
        m_boneTransform = boneTransform;
        m_bindMatrix = bindMatrix;
    };

    FTransform GetBoneTransform() const { return m_boneTransform; }

    FMatrix GetBindMatrix() const { return m_bindMatrix; }
	
};
