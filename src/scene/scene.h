#pragma once
#include <QList>
#include <camera.h>

#include "blocklist.h"

class Scene
{
public:
    Scene();
    QList<QList<QList<bool>>> objects;//A 3D list of the geometry in the world. Currently, it just lists whether or not a given cell contains a block.
    void CreateTestScene();

    glm::ivec3 dimensions;



    //---------------------------------------- PERLIN NOISE TERRAIN GENERATION ---------------------------------------
    static float perlinNoise2D(float x, float y);   //to calculate the z value
    std::vector<glm::vec3> makeEllipse(glm::vec3 center, float thickness, float width, float height);
    std::vector<glm::vec3> makeSphere(glm::vec3 center, float radius);
};
