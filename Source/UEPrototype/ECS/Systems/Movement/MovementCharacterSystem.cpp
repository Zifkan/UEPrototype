#include "MovementCharacterSystem.h"



void MovementCharacterSystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e,MovementVelocity& velocity , ViewDirectionData viewDir ,Translation& translation, Rotation& rotation)
    {
        auto newRot = FQuat::Identity;     

        if (!velocity.Value.IsZero())
        {
            if ((lastRot - rotation.Value).IsIdentity())
            {
                newRot = viewDir.Axises.GetSafeNormal().ToOrientationRotator().Quaternion();  
            }
            else
            {
                newRot = FQuat( 0, 0,rotation.Value.Z, rotation.Value.W);   
            }
        }
        
        lastRot = rotation.Value;
        
        translation.Value+=velocity.Value;
        rotation.Value = newRot;
    });
}
