#ifndef RAY_H
#define RAY_H

#include "la.h"
#include <iostream>
#include <limits>
#include <cmath>
#include "camera.h"
#include "scene/blocklist.h"

/*
 * Ray-Cube Intersection
 */

class Ray
{
private:
protected:

public:
    // Constructors/Destructors
    Ray();
    Ray(Camera c);
    Ray(glm::vec3 eye, glm::vec3 ref);
    ~Ray();

    // Member variables
    float len;              // length of ray
    float alpha;            // half of fovy
    glm::vec3 v;            // U axis on casted ray
    glm::vec3 h;            // R axis on casted ray
    float sx;               // ndc x
    float sy;               // ndc y
    glm::vec3 p;            // point converted from px, py
    glm::vec3 origin;       // eye
    glm::vec3 direction;    // normalize(p - origin)

    // Functions
    bool RayMarch(BlockList &blocklist, std::tuple<int, int, int> &block);
    bool RayMarchAdd(BlockList &blocklist, std::tuple<int, int, int> &block);
    bool Collides(BlockList &blocklist);
};




#endif // RAY_H
