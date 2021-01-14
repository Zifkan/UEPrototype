#include "MovementCharacterSystem.h"



void MovementCharacterSystem::OnCreate()
{
    SystemRun->each([this](flecs::entity e,MovementVelocity& velocity , Translation& translation, Rotation& rotation)
    {
        auto newRot = rotation.Value;     

        if (!velocity.Value.IsZero())
        {
            if ((lastRot - rotation.Value).Size()>0.0f)
            {               
                newRot = FQuat(0,0, rotation.Value.Z,rotation.Value.W);   
            }
            else
            {
                newRot = FVector(velocity.Value.Y,-velocity.Value.X,0).GetSafeNormal().ToOrientationRotator().Quaternion();  
            }
        }
        
        lastRot = rotation.Value;

        
        translation.Value+=velocity.Value;
        rotation.Value = newRot;
    });
}
