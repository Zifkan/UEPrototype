#pragma once

struct CamEntityType
{
    
};

struct  CamComponent
{
    float HeightOffset;
    float FocusHeightOffset;

    float xDelta = 0.0f;
    float yDelta = 0.0f;
    float radius = 5.0f;

    float currentRadius = 0.0f;

    float xSpeed = 80.0f;
    float ySpeed = 80.0f;
    float yMinLimit = -90.0f;
    float yMaxLimit = 90.0f;
    float m_pitchLimit = -35.f;

    TEnumAsByte<ECollisionChannel> ProbeChannel;

    float ProbeSize;

    float x;
    float y;
};
