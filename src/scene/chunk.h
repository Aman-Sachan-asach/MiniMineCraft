#pragma once
#include <la.h>
#include <iostream>
#include <math.h>
#include <tuple>
#include <vector>
#include <chrono>
#include <random>
#include <time.h>
#include <stdlib.h>
#include <functional>

#include <QLabel>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include "drawable.h"
#include "scene.h"

class Scene;

struct Vertex_Data
{
    glm::vec4 pos;
    glm::vec4 color;
    glm::vec4 normal;
    glm::vec2 uv;
    glm::vec4 tangent;
    glm::vec4 bitangent;
    float shininess;
    int flag_animate;
};

class Chunk: public Drawable
{
public:
    enum BlockType{ EMPTY = 0, WATER = 1, DIRT = 2, LAVA = 3, GRASS = 4, WOOD = 5,
                    LEAF = 6, STONE = 7, BEDROCK = 8, COAL = 9, IRONORE = 10, CRYSTAL = 11,
                    BRICK = 12, CLOUD = 13};

    std::vector<BlockType> BlocksInChunk;

    int facecount = 0;
    int ChunkID;
    //int y_length = 256; //size of the y dimension of the world and hence every chunk
    glm::vec4 chunk_center;
    glm::vec3 PlayerPosition = glm::vec3(0.0f, 0.0f ,0.0f);
    glm::vec3 OldPlayerPosition = glm::vec3(0.0f, 0.0f ,0.0f);

    std::vector<Vertex_Data> VertexList;
    std::vector<GLuint> IndexList;

    Scene* scene;

    //ALl the UV co-ordinates for the starting bottom left of a block in the texture atlas, listed row wise
    float UVoffset = 0.1 * (1.0f/16.0f);
    float UV_block_stride = (1.0f/16.0f) - 2*UVoffset;

    glm::vec2 uv_BRICK = glm::vec2((7.0f/16.0f) + UVoffset, (0.0f/16.0f) + UVoffset);
    glm::vec2 uv_STONE = glm::vec2((1.0f/16.0f) + UVoffset, (0.0f) + UVoffset);
    glm::vec2 uv_DIRTandGRASSbottom = glm::vec2((2.0f/16.0f) + UVoffset, (0.0f) + UVoffset);
    glm::vec2 uv_GRASSside = glm::vec2((3.0f/16.0f) + UVoffset, (0.0f) + UVoffset);

    glm::vec2 uv_BEDROCK = glm::vec2((1.0f/16.0f) + UVoffset, (1.0f/16.0f) + UVoffset);
    glm::vec2 uv_WOODside = glm::vec2((4.0f/16.0f) + UVoffset, (1.0f/16.0f) + UVoffset);
    glm::vec2 uv_WOODtopandbottom = glm::vec2((5.0f/16.0f) + UVoffset, (1.0f/16.0f) + UVoffset);

    glm::vec2 uv_IRONORE = glm::vec2((1.0f/16.0f) + UVoffset, (2.0f/16.0f) + UVoffset);
    glm::vec2 uv_COAL = glm::vec2((2.0f/16.0f) + UVoffset, (2.0f/16.0f) + UVoffset);
    glm::vec2 uv_GRASStop = glm::vec2((8.0f/16.0f) + UVoffset, (2.0f/16.0f) + UVoffset);

    glm::vec2 uv_CRYSTAL = glm::vec2((3.0f/16.0f) + UVoffset, (3.0f/16.0f) + UVoffset);
    glm::vec2 uv_Leaf = glm::vec2((5.0f/16.0f) + UVoffset, (3.0f/16.0f) + UVoffset);

    glm::vec2 uv_WATER1 = glm::vec2((13.0f/16.0f) + UVoffset, (12.0f/16.0f) + UVoffset);
    glm::vec2 uv_WATER2 = glm::vec2((14.0f/16.0f) + UVoffset, (12.0f/16.0f) + UVoffset);
    glm::vec2 uv_WATER3 = glm::vec2((15.0f/16.0f) + UVoffset, (12.0f/16.0f) + UVoffset);

    glm::vec2 uv_WATER4 = glm::vec2((14.0f/16.0f) + UVoffset, (13.0f/16.0f) + UVoffset);
    glm::vec2 uv_WATER5 = glm::vec2((15.0f/16.0f) + UVoffset, (13.0f/16.0f + UVoffset));

    glm::vec2 uv_LAVA1 = glm::vec2((13.0f/16.0f) + UVoffset, (14.0f/16.0f) + UVoffset);
    glm::vec2 uv_LAVA2 = glm::vec2((14.0f/16.0f + UVoffset), (14.0f/16.0f) + UVoffset);
    glm::vec2 uv_LAVA3 = glm::vec2((15.0f/16.0f) + UVoffset, (14.0f/16.0f) + UVoffset);

    glm::vec2 uv_LAVA4 = glm::vec2((14.0f/16.0f) + UVoffset, (15.0f/16.0f) + UVoffset);
    glm::vec2 uv_LAVA5 = glm::vec2((15.0f/16.0f) + UVoffset, (15.0f/16.0f) + UVoffset);

    glm::vec2 uv_CLOUD = glm::vec2((2.0f/16.0f) + UVoffset, (4.0f/16.0f) + UVoffset);

    float shininess_amplification = 20.0f;
    float shininess_BRICK =   1.20f * shininess_amplification;
    float shininess_STONE =   1.60f * shininess_amplification;
    float shininess_DIRT  =   1.20f * shininess_amplification;
    float shininess_GRASS =   1.40f * shininess_amplification;
    float shininess_BEDROCK = 1.80f * shininess_amplification;
    float shininess_WOOD =    1.10f * shininess_amplification;
    float shininess_IRONORE = 1.85f * shininess_amplification;
    float shininess_COAL =    0.90f * shininess_amplification;
    float shininess_CRYSTAL = 1.99f * shininess_amplification;
    float shininess_LEAF =    1.50f * shininess_amplification;
    float shininess_WATER =   1.70f * shininess_amplification;
    float shininess_LAVA =    1.60f * shininess_amplification;
    float shininess_CLOUD =   1.10f * shininess_amplification;

public:
    Chunk();
    Chunk(GLWidget277* context, glm::vec4 center);

    void createchunk();
    void CreateVBO();
    void ChunkCreationHelper(glm::vec4 ChunkCenter);

    float BlockShininess(Chunk::BlockType block_type);
    int AnimateTexture(Chunk::BlockType block_type);
    glm::vec2 UVforFace(Chunk::BlockType block_type, char face);
    bool TestFaceForDrawing(char c, int x, int y, int z, int index);
    void PerFaceDetails(char c,
                        BlockType Block_type, int index,
                        glm::vec4 pos1, glm::vec4 pos2, glm::vec4 pos3, glm::vec4 pos4,
                        float shiny,
                        int flag_animation);
    Chunk::BlockType replacefacetexture(int index, int OldBlockType);
    void CreateFaceInOrder(char c,
                           glm::vec4 pos1, glm::vec4 pos2, glm::vec4 pos3, glm::vec4 pos4,
                           glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4,
                           float shiny,
                           int flag_animation);
    void CreateIndexListForFace(int face_count);

    void Update();
    void AddBlock(float posx, float posy, float posz, Chunk::BlockType block_type);
    void DeleteBlock(float posx, float posy, float posz);

    virtual void create();
    virtual GLenum drawMode();
};
