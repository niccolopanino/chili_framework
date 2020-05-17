#pragma once
#include "Keyboard.h"
#include "Mouse.h"
#include "Graphics.h"
#include <string>

class Scene
{
public:
    Scene(const std::string &name) : m_name(name) { }
    virtual ~Scene() = default;
    virtual void update(Keyboard &kbd, Mouse &mouse, float dt) = 0;
    virtual void draw(Graphics &gfx) const = 0;
    const std::string &get_name() const { return m_name; }
private:
    std::string m_name;
};
