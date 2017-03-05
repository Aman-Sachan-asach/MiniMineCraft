#ifndef PLAYER_H
#define PLAYER_H

#include <iostream>
#include "drawable.h"
#include <la.h>
//#include "../behaviors.h"

class Player : public Drawable
{
private:
protected:

public:
    Player(GLWidget277 *context) : Drawable(context) {}

    virtual void create();
    virtual GLenum drawMode();

    glm::vec3 pos = glm::vec3(20, 5, 21);
    glm::vec3 velocity = glm::vec3(0,0,0);
    glm::vec3 acceleration = glm::vec3(0,0,0);
    glm::vec3 target;
    float r = 6.f;
    float max_force = 0.1f;
    float max_speed = 4.f;

    // Behavior stuff
    //Behaviors behavior;
    void setTarget(float x, float y, float z);

    void printPosition();
};



#endif // PLAYER_H
