#pragma once


struct InputTag
{
   // Entity Owner;
};

struct MoveInputTag :  InputTag
{
   
};

struct RollInputTag:  InputTag
{
    
};

struct BlockInputTag:  InputTag
{
    
};


struct AttackInputTag:  InputTag
{
    
};


struct SprintInputTag:  InputTag
{
    
};

struct InheritsFrom
{
    flecs::entity base;
};
