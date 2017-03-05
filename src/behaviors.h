#ifndef BEHAVIORS_H
#define BEHAVIORS_H

#include "la.h"
#include <iostream>
#include "scene/player.h"

class Behaviors
{
private:
protected:

public:
    float acceleration = 9.8f;
    float dT = 0.2f;
    float maxVelocity = acceleration * dT;

    float slowing_radius = 2.f;

    Behaviors behavior();

    glm::vec3 Seek(glm::vec3 target, glm::vec3 pos, glm::vec3 agent_velocity) const;
    glm::vec3 Arrive(glm::vec3 target, glm::vec3 pos, glm::vec3 agent_velocity);
    void Arrive(Player &agent);
    void Wander();
};

#endif // BEHAVIORS_H
