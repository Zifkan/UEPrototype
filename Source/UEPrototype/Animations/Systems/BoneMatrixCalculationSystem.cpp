#include "BoneMatrixCalculationSystem.h"
#include "RendererGroup.h"
#include "Components/CustomBone.h"
#include "Components/TransformComponents.h"

void BoneMatrixCalculationSystem::OnCreate()
{
    rendererGroup = RendererGroup::instance();
    
    SystemRun->each([this](flecs::entity e, RendererRoot& rendererRoot,CharacterActorComponent& characterRender)
   {
    	UpdateRefToLocalMatricesInner(characterRender,0);

    	
    //	auto transforms = characterRender.Character->Mesh->SkeletalMesh->RefBasesInvMatrix;

      /*  for (int i = 0; i < transforms.Num(); ++i)
        {        
        	auto localToWorld =  transforms[i];
            
            auto bindMatrix = characterRender.Character->Mesh->GetComponentSpaceTransforms()[i].ToMatrixWithScale();

            const auto renderEntityListId = rendererRoot.RenderEntityListId;
            const auto result = localToWorld *bindMatrix;

            rendererGroup->Buffer[renderEntityListId * 256 + i] = result;
			
        	
    		//UE_LOG(LogTemp, Warning, TEXT("Bones Matrix ECS %s"),*transforms[i].ToMatrixWithScale().ToString());
    	}*/

    	
      /*  for (auto children : e.children())
        {
        	uint32 counter=0;
        	for (auto i : children)
	        {		   
                auto boneEntity =  children.entity(i);  

                auto localToWorld = characterRender.Character->Mesh->GetComponentSpaceTransforms()[i].ToMatrixWithScale(); // boneEntity.get<LocalToWorld>()->Value;
            
                auto bindMatrix = boneEntity.get<CustomBone>()->BindMatrix;

                const auto renderEntityListId = rendererRoot.RenderEntityListId;
                const auto result = localToWorld*bindMatrix ;

                 rendererGroup->Buffer[renderEntityListId * 256 + counter] = result;	        	
        		counter++;
        	//	UE_LOG(LogTemp, Warning, TEXT("Bones Matrix ECS %s"),*rendererGroup->Buffer[renderEntityListId * 256 + counter].ToString());
             //   Mesh->GetBoneSpaceTransforms()
            }
        }*/

		//UE_LOG(LogTemp, Warning, TEXT("Bones Matrix ECS %s"),*rendererGroup->Buffer[1].ToString());
   }).kind(0);
}


void BoneMatrixCalculationSystem::UpdateRefToLocalMatricesInner(CharacterActorComponent& characterRender,const TArray<FBoneIndexType>* ExtraRequiredBoneIndices)
{
	int32 LODIndex =0;
	 FSkeletalMeshRenderData* InSkeletalMeshRenderData = characterRender.Character->Mesh->SkeletalMesh->GetResourceForRendering();
	const USkeletalMesh* const ThisMesh = characterRender.Character->Mesh->SkeletalMesh;
	const USkinnedMeshComponent* const MasterComp = characterRender.Character->Mesh->MasterPoseComponent.Get();
	const FSkeletalMeshLODRenderData& LOD = InSkeletalMeshRenderData->LODRenderData[LODIndex];
	auto& renderSections = LOD.RenderSections;
	
	
	const FReferenceSkeleton& RefSkeleton = ThisMesh->RefSkeleton;
	const TArray<int32>& MasterBoneMap = characterRender.Character->Mesh->GetMasterBoneMap();
	 bool bIsMasterCompValid = MasterComp && MasterBoneMap.Num() == ThisMesh->RefSkeleton.GetNum();
	const TArray<FTransform>& ComponentTransform = (bIsMasterCompValid)? MasterComp->GetComponentSpaceTransforms() : characterRender.Character->Mesh->GetComponentSpaceTransforms();
	const TArray<uint8>& BoneVisibilityStates = (bIsMasterCompValid) ? MasterComp->GetBoneVisibilityStates() : characterRender.Character->Mesh->GetBoneVisibilityStates();
	// Get inv ref pose matrices
	const TArray<FMatrix>* RefBasesInvMatrix = &ThisMesh->RefBasesInvMatrix;

	TArray<FMatrix> ReferenceToLocal;
	

	check(RefBasesInvMatrix->Num() != 0);

	if (ReferenceToLocal.Num() != RefBasesInvMatrix->Num())
	{
		ReferenceToLocal.Reset();
		ReferenceToLocal.AddUninitialized(RefBasesInvMatrix->Num());
	}

	const TArray<FBoneIndexType>* RequiredBoneSets[3] = { &LOD.ActiveBoneIndices, ExtraRequiredBoneIndices, NULL };

	const bool bBoneVisibilityStatesValid = BoneVisibilityStates.Num() == ComponentTransform.Num();
	//bIsMasterCompValid = MasterBoneMap != nullptr;
	bIsMasterCompValid = false;
	for (int32 RequiredBoneSetIndex = 0; RequiredBoneSets[RequiredBoneSetIndex] != NULL; RequiredBoneSetIndex++)
	{
		const TArray<FBoneIndexType>& RequiredBoneIndices = *RequiredBoneSets[RequiredBoneSetIndex];

		// Get the index of the bone in this skeleton, and loop up in table to find index in parent component mesh.
		for (int32 BoneIndex = 0; BoneIndex < RequiredBoneIndices.Num(); BoneIndex++)
		{
			const int32 ThisBoneIndex = RequiredBoneIndices[BoneIndex];

			if ( RefBasesInvMatrix->IsValidIndex(ThisBoneIndex) )
			{
				// On the off chance the parent matrix isn't valid, revert to identity.
				ReferenceToLocal[ThisBoneIndex] = FMatrix::Identity;

				//if we have master pose component, we use MasterBoneMap to figure out the mapping
				if( bIsMasterCompValid )
				{
					// If valid, use matrix from parent component.
					const int32 MasterBoneIndex = (MasterBoneMap)[ThisBoneIndex];
					if (ComponentTransform.IsValidIndex(MasterBoneIndex))
					{
						const int32 ParentIndex = RefSkeleton.GetParentIndex(ThisBoneIndex);
						bool bNeedToHideBone = BoneVisibilityStates[MasterBoneIndex] != BVS_Visible;
						if (bNeedToHideBone && ParentIndex != INDEX_NONE)
						{
							ReferenceToLocal[ThisBoneIndex] = ReferenceToLocal[ParentIndex].ApplyScale(0.f);
						}
						else
						{
							checkSlow(ComponentTransform[MasterBoneIndex].IsRotationNormalized());
							ReferenceToLocal[ThisBoneIndex] = ComponentTransform[MasterBoneIndex].ToMatrixWithScale();
						}
					}
					else
					{
						const int32 ParentIndex = RefSkeleton.GetParentIndex(ThisBoneIndex);
						const FMatrix RefLocalPose = RefSkeleton.GetRefBonePose()[ThisBoneIndex].ToMatrixWithScale();
						if (ParentIndex != INDEX_NONE)
						{
							ReferenceToLocal[ThisBoneIndex] = RefLocalPose * ReferenceToLocal[ParentIndex];
						}
						else
						{
							ReferenceToLocal[ThisBoneIndex] = RefLocalPose;
						}
					}
				}
				else
				{
					if (ComponentTransform.IsValidIndex(ThisBoneIndex))
					{
						if (bBoneVisibilityStatesValid)
						{
							// If we can't find this bone in the parent, we just use the reference pose.
							const int32 ParentIndex = RefSkeleton.GetParentIndex(ThisBoneIndex);
							bool bNeedToHideBone = BoneVisibilityStates[ThisBoneIndex] != BVS_Visible;
							if (bNeedToHideBone && ParentIndex != INDEX_NONE)
							{
								ReferenceToLocal[ThisBoneIndex] = ReferenceToLocal[ParentIndex].ApplyScale(0.f);
							}
							else
							{
								checkSlow(ComponentTransform[ThisBoneIndex].IsRotationNormalized());
								ReferenceToLocal[ThisBoneIndex] = ComponentTransform[ThisBoneIndex].ToMatrixWithScale();
							}
						}
						else
						{
							checkSlow(ComponentTransform[ThisBoneIndex].IsRotationNormalized());
							ReferenceToLocal[ThisBoneIndex] = ComponentTransform[ThisBoneIndex].ToMatrixWithScale();
						}
					}
				}
			}
			// removed else statement to set ReferenceToLocal[ThisBoneIndex] = FTransform::Identity;
			// since it failed in ( ThisMesh->RefBasesInvMatrix.IsValidIndex(ThisBoneIndex) ), ReferenceToLocal is not valid either
			// because of the initialization code line above to match both array count
			// if(ReferenceToLocal.Num() != ThisMesh->RefBasesInvMatrix.Num())					
		}
	}

	for (int32 ThisBoneIndex = 0; ThisBoneIndex < ReferenceToLocal.Num(); ThisBoneIndex++)
	{
		ReferenceToLocal[ThisBoneIndex] = (*RefBasesInvMatrix)[ThisBoneIndex] * ReferenceToLocal[ThisBoneIndex];
	}


	for (int i = 0; i < renderSections.Num(); ++i)
	{
		auto& renderSection = renderSections[i];
		auto& boneMap = renderSection.BoneMap;

		for (int boneIndex = 0; boneIndex < boneMap.Num(); ++boneIndex)
		{	
			const FBoneIndexType RefToLocalIdx = boneMap[boneIndex];
			
			FMatrix3x4 BoneMat;
			const FMatrix& RefToLocal =ReferenceToLocal[RefToLocalIdx];
			RefToLocal.To3x4MatrixTranspose((float*)BoneMat.M);

		/*	rendererGroup->Buffer[0 * 256 + boneIndex] = FMatrix(FPlane(BoneMat.M[0][0],BoneMat.M[0][1],BoneMat.M[0][2],BoneMat.M[0][3]),
                                                                 FPlane(BoneMat.M[1][0],BoneMat.M[1][1],BoneMat.M[1][2],BoneMat.M[1][3]),
                                                                 FPlane(BoneMat.M[2][0],BoneMat.M[2][1],BoneMat.M[2][2],BoneMat.M[2][3]),
                                                                 FPlane(0,0,0,1));*/


		//	rendererGroup->Buffer[0 * 256 + boneIndex] = ReferenceToLocal[RefToLocalIdx];
			

			rendererGroup->Buffer[0 * 256 + boneIndex] = FMatrix(FPlane(1,0,0,0),
                                                                 FPlane(0,1,0,0),
                                                                 FPlane(0,0,1,0),
                                                                 FPlane(0,0,0,1));
		}
	}

	
	/*
	for (int i = 0; i < boneMap.Num(); ++i)
	{
		//ReferenceToLocal[ThisBoneIndex] = FMatrix::Identity;
		
		//	ReferenceToLocal[ThisBoneIndex].To3x4MatrixTranspose((float*)result.M);
		////result.SetMatrix(FMatrix::Identity);
		auto testMatrix = FMatrix::Identity;
		testMatrix.ApplyScale(0.5);
		testMatrix.To3x4MatrixTranspose((float*)result.M);

		const FBoneIndexType RefToLocalIdx = boneMap[i];
		
	
	/*	const FMatrix& RefToLocal = ReferenceToLocal[RefToLocalIdx];
		RefToLocal.To3x4MatrixTranspose((float*)result.M);*/
	/*	rendererGroup->Buffer[0 * 256 + i] =  FMatrix(FPlane(result.M[0][0],result.M[0][1],result.M[0][2],result.M[0][3]),
                                                      FPlane(result.M[1][0],result.M[1][1],result.M[1][2],result.M[1][3]),
                                                      FPlane(result.M[2][0],result.M[2][1],result.M[2][2],result.M[2][3]),
                                                      FPlane(0,0,0,0));*/


		/*rendererGroup->Buffer[0 * 256 + i] =  FMatrix(FPlane(result.M[0][0],result.M[1][0],result.M[2][0],0),
                                                                  FPlane(result.M[0][1],result.M[1][1],result.M[2][1],0),
                                                                  FPlane(result.M[0][2],result.M[1][2],result.M[2][2],0),
                                                                 FPlane(result.M[0][2],result.M[1][2],result.M[2][2],0));*/
		

		//rendererGroup->Buffer[0 * 256 + i]= RefToLocal;
		/*  float3x4(
		float4(Instancing_BoneMatrixBufferSRV[Index][0].x, Instancing_BoneMatrixBufferSRV[Index][1].x, Instancing_BoneMatrixBufferSRV[Index][2].x,0),
		float4(Instancing_BoneMatrixBufferSRV[Index][0].y, Instancing_BoneMatrixBufferSRV[Index][1].y, Instancing_BoneMatrixBufferSRV[Index][2].y,0),
		float4(Instancing_BoneMatrixBufferSRV[Index][0].z, Instancing_BoneMatrixBufferSRV[Index][1].z, Instancing_BoneMatrixBufferSRV[Index][2].z,0));
		float4(Instancing_BoneMatrixBufferSRV[Index][0].z, Instancing_BoneMatrixBufferSRV[Index][1].z, Instancing_BoneMatrixBufferSRV[Index][2].z,0));

		
	}
	*/

	UE_LOG(LogTemp, Warning, TEXT("Bones Matrix: %s"),*ReferenceToLocal[10].ToString());

	//UE_LOG(LogTemp, Warning, TEXT("Bones Matrix RefBasesInvMatrix: %s"),*(*RefBasesInvMatrix)[1].ToString());

}
