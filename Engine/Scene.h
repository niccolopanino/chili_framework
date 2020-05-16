#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"

class Scene
{
public:
    virtual ~Scene() = default;
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) = 0;
    virtual void draw(Graphics &gfx) const = 0;
};
