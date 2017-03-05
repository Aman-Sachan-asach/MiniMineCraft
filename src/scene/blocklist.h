#pragma once
#include <la.h>
#include <iostream>
#include <math.h>

#include <QLabel>
#include <QOpenGLContext>
#include <QOpenGLBuffer>
#include <QOpenGLShaderProgram>

#include "drawable.h"
#include <tuple>
#include <vector>
#include <chrono>
#include <random>
#include <time.h>
#include <stdlib.h>

#include "scene.h"

class Scene;

struct vertex_data
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

class BlockList : public Drawable
{
public:
    enum BlockType{ WATER = 1, DIRT = 2, LAVA = 3, GRASS = 4, WOOD = 5, LEAF = 6,
                    STONE = 7, BEDROCK = 8, COAL = 9, IRONORE = 10, CRYSTAL = 11,
                    BRICK = 12, CLOUD = 13};

public:
    std::map<std::tuple<int, int, int>, int> BlockMap; //Maps a position (stored as a std::tuple)
                                                       // to a enum type
    std::map<std::tuple<int, int, int>, int> ChunkMap; //Maps position (position units are in
                                                       //terms of chunk size instead of blocksize)
                                                       //to chunk ID
    int facecount = 0;
    int ChunkID;
    glm::vec3 PlayerPosition = glm::vec3(0.0f, 0.0f ,0.0f);
    glm::vec3 OldPlayerPosition = glm::vec3(0.0f, 0.0f ,0.0f);
    std::vector<int> WorldBoundary; //x_max, x_min, y_max, y_min, z_max, z_min

    std::vector<bool> ChunkModified;

    std::vector<vertex_data> VertexList;
    std::vector<GLuint> IndexList;

    std::vector<glm::vec2> UVList;

    Scene* scene;

    QLabel myLabel;

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
    //Constructor
    BlockList();
    BlockList(GLWidget277* context);

    virtual void create();

    void createInitialWorld();
    glm::vec2 CreateInitialBlockMap();

    void CreateClouds(int max_x, int max_z);

    glm::vec2 CreateMoreBlocks(int min_x, int max_x, int min_z, int max_z);

    void CheckExistingVBOChunks();
    bool CreateChunkVBOs(int chunkX, int chunkY, int chunkZ);
    bool ReCreateVBOChunk(int chunkX, int chunkY, int chunkZ);
    bool RecreatingChunkHelper(int min_x, int max_x, int min_y, int max_y, int min_z, int max_z);
    bool OriginalChunkCreationHelper(int min_x, int max_x, int min_y, int max_y, int min_z, int max_z);

    glm::vec4 NormalforFace(int block_type, char face);

    glm::vec2 UVforFace(int block_type, char face);
    void UV_to_VBO(int index);
    int replacefacetexture(int x, int y , int z, int OldBlockType);

    int BlockTypeDetermination(int height);
    int BlockShininess(int block_type);
    int AnimateTexture(int block_type);

    void CreateIndexListForFace(int face_count);
    void CreateFaceInOrder(char c, glm::vec4 pos1, glm::vec4 pos2, glm::vec4 pos3, glm::vec4 pos4,
                           glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4, float shiny,
                           int flag_animation);
    bool TestFaceForDrawing(char c, int x, int y, int z);

    //Getter
    std::map<std::tuple<int, int, int>, int> getBlockMap();
    int getBlockType(std::tuple<int, int, int> block_pos);
    int getChunkID(std::tuple<int, int, int> chunk_pos);

    void setBlockType(int type, std::tuple<int, int, int> block_pos);

    //Setter
    void AddBlock(std::tuple<int, int, int> block_pos, int block_enum_type);
    void AddChunk(std::tuple<int, int, int> chunk_pos); //adds a chunk to chunkmap and then gives the chunk an id
    void DeleteBlock(std::tuple<int, int, int> block_pos);

    virtual GLenum drawMode();
};
