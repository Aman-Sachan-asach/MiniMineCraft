#pragma once
#include <QList>
#include <camera.h>

#include "blocklist.h"

//#include "mygl.h"

//class MyGL;

class Scene
{
public:
    Scene();
    QList<QList<QList<bool>>> objects;//A 3D list of the geometry in the world. Currently, it just lists whether or not a given cell contains a block.
    void CreateTestScene();

    glm::ivec3 dimensions;



    //---------------------------------------- PERLIN NOISE TERRAIN GENERATION ---------------------------------------
    //BlockList blockslist;                   //to get the map and block type
    //BlockList* blockslist;
    //MyGL* mygl;


    //void createPerlinNoiseScene(BlockList* blocklist);          //to iterate through the map
    static float perlinNoise2D(float x, float y);   //to calculate the z value
    void perlinWorm();
    std::vector<glm::vec3> makeEllipse(glm::vec3 center, float thickness, float width, float height);
    std::vector<glm::vec3> makeSphere(glm::vec3 center, float radius);
};
