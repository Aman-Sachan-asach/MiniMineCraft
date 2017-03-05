#include "blocklist.h"
#include <functional>

#include <la.h>
#include <iostream>


BlockList::BlockList()
    : Drawable(nullptr)
{}

BlockList::BlockList(GLWidget277* context)
    : Drawable(context),
      BlockMap(), ChunkMap(),
      WorldBoundary()
{
     WorldBoundary.push_back(-32.0f);
     WorldBoundary.push_back(64.0f);
     WorldBoundary.push_back(-32.0f);
     WorldBoundary.push_back(64.0f);
     WorldBoundary.push_back(-32.0f);
     WorldBoundary.push_back(64.0f);
     /*
     WorldBoundary.push_back(-50.0f);
     WorldBoundary.push_back(100.0f);
     WorldBoundary.push_back(-10.0f);
     WorldBoundary.push_back(60.0f);
     WorldBoundary.push_back(-50.0f);
     WorldBoundary.push_back(100.0f);
     */
}

void BlockList::create()
{}

void BlockList::createInitialWorld()
{
    int min_x = (int)floor(WorldBoundary[0]/16.0f);
    int max_x = (int)ceil(WorldBoundary[1]/16.0f);
    int min_y = (int)floor(WorldBoundary[2]/16.0f);
    int max_y = (int)ceil(WorldBoundary[3]/16.0f);
    int min_z = (int)floor(WorldBoundary[4]/16.0f);
    int max_z = (int)ceil(WorldBoundary[5]/16.0f);

    for(int i = min_x; i <= max_x; i++) //4 chunks in X
    {
        for(int j = min_y; j <= max_y+1; j++) //4 chunks in Y for every X
        {
            for(int k =min_z; k <= max_z ;k++) //4 chunks in Z for every XY
            {
                //create VBOs for chunk
                CreateChunkVBOs(i, j, k);
                //draw the chunk in MyGL
            }
        }
    }
}

//Populates the block map
glm::vec2 BlockList::CreateInitialBlockMap() // 64x64xheight blocks
{
    glm::vec2 y_bounds;
    for(int i = WorldBoundary[0]; i<= WorldBoundary[1] ;i++) //4 chunks in X
    {
        for(int k = WorldBoundary[4]+1; k <= WorldBoundary[5] ;k++) //4 chunks in Y for every X
        {
            float perlin_value = 30.0f * std::fabs(scene->perlinNoise2D(std::abs(i) / 4.0f, std::abs(k) / 4.0f));
            for(int m = 0; m < perlin_value; m++)
            {
                //Determine Block Type using Height
                AddBlock(std::tuple<int, int, int>(i, m, k), DIRT);
            }
            y_bounds = glm::vec2(0, perlin_value);
        }
    }
    return y_bounds;
}

glm::vec2 BlockList::CreateMoreBlocks(int min_x, int max_x, int min_z, int max_z)
{
    glm::vec2 y_chunk_bounds;
    for(int i = min_x; i<= max_x ;i++) //chunks in X
    {
        for(int k = min_z; k <= max_z ;k++) //4 chunks in Y for every X
        {
            float perlin_value = 30.0f * std::fabs(scene->perlinNoise2D(std::abs(i) / 4.0f, std::abs(k) / 4.0f));

            for(int m = 0; m < (int)perlin_value; m++)
            {
                AddBlock(std::tuple<int, int, int>(i, m, k), DIRT);
            }
            y_chunk_bounds = glm::vec2(0, perlin_value);

        }
    }
    return y_chunk_bounds;
}

void BlockList::CreateClouds(int max_x, int max_z)
{
    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand (seed);

    for(int i = 0; i < max_x ;i++) //chunks in X
    {
        for(int k = 0; k < max_z ;k++) //4 chunks in Y for every X
        {
            float a = rand() % 200 - 100;
            float c = rand() % 200 - 100;

            glm::vec3 center = glm::vec3(a,31,c);
            float t = rand() % 2 + 4.0f;;
            float w = rand() % 2 + 6.0f;
            float h = rand() % 2 + 9.0f;

            std::vector<glm::vec3> points_ellipse = scene->makeEllipse(center, t, w, h);

            for(glm::vec3 point : points_ellipse)
            {
                AddBlock(std::tuple<float, float, float>(point.x, point.y, point.z), CLOUD);
            }
        }
    }
}

int BlockList::BlockTypeDetermination(int height)
{
    if(height <= -16)
    {
        return STONE;
    }
    else
    {
        return DIRT;
    }
}

glm::vec2 BlockList::UVforFace(int block_type, char face)
{
    glm::vec2 uv;// = uv_GRASStop;

    switch(block_type)
    {
        case WATER:
            //for right now it's just one texture instead of an animated thing
            uv = uv_WATER1;
            return uv;
            break;
        case GRASS:
            switch(face)
            {
                case 't': //top
                    uv = uv_GRASStop;
                    return uv;
                    break;
                case 's': //side
                    uv = uv_GRASSside;
                    return uv;
                    break;
                case 'b': //bottom
                    uv = uv_DIRTandGRASSbottom;
                    return uv;
                    break;
            }
            break;
        case LAVA:
            //for right now it's just one texture instead of an animated thing
            uv = uv_LAVA1;
            return uv;
            break;
        case DIRT:
            uv = uv_DIRTandGRASSbottom;
            return uv;
            break;
        case WOOD:
            switch(face)
            {
                case 't': //top and bottom
                    uv = uv_WOODtopandbottom;
                    return uv;
                    break;
                case 's': //side
                    uv = uv_WOODside;
                    return uv;
                    break;
            }
            break;
        case LEAF:
            uv = uv_Leaf;
            return uv;
            break;
        case STONE:
            uv = uv_STONE;
            return uv;
            break;
        case BEDROCK:
            uv = uv_BEDROCK;
            return uv;
            break;
        case COAL:
            uv = uv_COAL;
            return uv;
            break;
        case IRONORE:
            uv = uv_IRONORE;
            return uv;
            break;
        case CRYSTAL:
            uv = uv_CRYSTAL;
            return uv;
            break;
        case BRICK:
            uv = uv_BRICK;
            return uv;
            break;
        case CLOUD:
            uv = uv_CLOUD;
            return uv;
            break;
        default:
            uv = uv_CRYSTAL; //so you'll know if something went wrong
            return uv;
            break;
    }

    return uv;
}

bool BlockList::CreateChunkVBOs(int chunkX, int chunkY, int chunkZ)
{
    VertexList.clear();
    IndexList.clear();
    UVList.clear();

    facecount = 0;
    bool flag =false;

    int xb = 16*(chunkX-1);
    int yb = 16*(chunkY-1) +1;
    int zb = 16*(chunkZ-1) +1;
    int xe = 16*(chunkX) -1;
    int ye = 16*(chunkY);
    int ze = 16*(chunkZ);

    flag = OriginalChunkCreationHelper(xb, xe, yb, ye, zb, ze);

    //pass things into vbos
    if(flag)
    {
        std::tuple<int, int, int> temp (chunkX, chunkY, chunkZ);
        AddChunk(temp);

        int index = getChunkID(temp);

        generateNewVBO(index);
        context->glBindBuffer(GL_ARRAY_BUFFER, VBOList[index]);
        context->glBufferData(GL_ARRAY_BUFFER,
                              VertexList.size()*sizeof(vertex_data), //size of the thing
                              VertexList.data(), //from where
                              GL_STATIC_DRAW);

        generateNewVBOIndex(index);
        IndexCount.push_back(IndexList.size());
        context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOIndexList[index]);
        context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                              IndexList.size()*sizeof(GLuint),
                              IndexList.data(),
                              GL_STATIC_DRAW);
    }
    return flag;
}

bool BlockList::ReCreateVBOChunk(int chunkX, int chunkY, int chunkZ)
{
    VertexList.clear();
    IndexList.clear();
    UVList.clear();

    facecount = 0;
    bool flag = false;

    int xb = 16*(chunkX-1);
    int yb = 16*(chunkY-1) + 1;
    int zb = 16*(chunkZ-1) + 1;
    int xe = 16*(chunkX) - 1;
    int ye = 16*(chunkY);
    int ze = 16*(chunkZ);

    flag = RecreatingChunkHelper(xb, xe, yb, ye, zb, ze);

    //pass things into vbos
    if(flag)
    {
        std::tuple<int, int, int> temp (chunkX, chunkY, chunkZ);
        int index;

        if(ChunkMap.find(temp) == ChunkMap.end())
        {
            //chunk doesn't exist
            AddChunk(temp);
            index = getChunkID(temp);
        }
        else
        {
            //AddChunk(temp);
            index = getChunkID(temp);
            RegenerateNewVBO(index);
            RegenerateNewVBOIndex(index);
        }

        context->glBindBuffer(GL_ARRAY_BUFFER, VBOList[index]);
        context->glBufferData(GL_ARRAY_BUFFER,
                                 VertexList.size()*sizeof(vertex_data), //size of the thing
                                 VertexList.data(), //from where
                                 GL_STATIC_DRAW);

        context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOIndexList[index]);
        context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                                 IndexList.size()*sizeof(GLuint),
                                 IndexList.data(),
                                 GL_STATIC_DRAW);
    }
    else
    {
        std::tuple<int, int, int> temp (chunkX, chunkY, chunkZ);
        int index = getChunkID(temp);

        DeleteVBOHandle(index);
        DeleteVBOIndex(index);
    }
    return flag;
}

int BlockList::BlockShininess(int block_type)
{
    float shiny;
    switch(block_type)
    {
        case WATER:
            //for right now it's just one texture instead of an animated thing
            shiny = shininess_WATER;
            return shiny;
            break;
        case GRASS:
            shiny = shininess_GRASS;
            return shiny;
            break;
        case LAVA:
            //for right now it's just one texture instead of an animated thing
            shiny = shininess_LAVA;
            return shiny;
            break;
        case DIRT:
            shiny = shininess_DIRT;
            return shiny;
            break;
        case WOOD:
            shiny = shininess_WOOD;
            return shiny;
            break;
        case LEAF:
            shiny = shininess_LEAF;
            return shiny;
            break;
        case STONE:
            shiny = shininess_STONE;
            return shiny;
            break;
        case BEDROCK:
            shiny = shininess_BEDROCK;
            return shiny;
            break;
        case COAL:
            shiny = shininess_COAL;
            return shiny;
            break;
        case IRONORE:
            shiny = shininess_IRONORE;
            return shiny;
            break;
        case CRYSTAL:
            shiny = shininess_CRYSTAL;
            return shiny;
            break;
        case BRICK:
            shiny = shininess_BRICK;
            return shiny;
            break;
        case CLOUD:
            shiny = shininess_CLOUD;
            return shiny;
            break;
        default:
        {
            return 1.0f; //default shininess
            break;
        }
    }
}

bool BlockList::OriginalChunkCreationHelper(int min_x, int max_x, int min_y, int max_y, int min_z, int max_z)
{
    bool flag_local = false;
    for(float i=min_x; i<=max_x ;i++)
    {
        for(float j=min_y; j<=max_y ;j++)
        {
            for(float k=min_z; k<=max_z ;k++)
            {
                //check if the block exitst if so add the block to the VBOs
                std::tuple<int, int, int> key (i, j, k);
                std::map<std::tuple<int, int, int>, int>::iterator it = BlockMap.find(key);

                if(it != BlockMap.end()) // the block exists
                {
                    flag_local = true;
                    //int Block_type = getBlockType(key);
                    int Block_type = it->second;
                    //Add the blocks position color index and nramls to the VBO
                    //Add 24 positions for the block
                    //Add 36 indices
                    //Add 24 colors
                    //Add 24 normals

                    //for every face check if it should be drawn depending on if
                    //there is a block touching that face

                    glm::vec2 UV1;
                    glm::vec2 UV2;
                    glm::vec2 UV3;
                    glm::vec2 UV4;

                    float shininess = BlockShininess(Block_type);
                    int flag_texture_animate = AnimateTexture(Block_type);

                    //Front Face
                    if(TestFaceForDrawing('f', i, j, k))
                    {
                        glm::vec4 p1 = glm::vec4(i, j, k, 1.0f);
                        glm::vec4 p2 = glm::vec4(i, j-1, k, 1.0f);
                        glm::vec4 p3 = glm::vec4(i-1, j-1, k, 1.0f);
                        glm::vec4 p4 = glm::vec4(i-1, j, k, 1.0f);
                        //if we go counter-clockwise and create a one-one mapping of vertex with UV co-ordinates,
                        //the UV co-ordinates witll be as shown below

                        //the face can be drawn so it can be changed to a grass face if it is dirt
                        int new_block_type = replacefacetexture(i, j, k, Block_type);

                        UV1 = UVforFace(new_block_type, 's');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('f', p1, p2, p3, p4, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Right Face
                    if(TestFaceForDrawing('r', i, j, k))
                    {
                        glm::vec4 p5 = glm::vec4(i, j, k-1, 1.0f);
                        glm::vec4 p6 = glm::vec4(i, j-1, k-1, 1.0f);
                        glm::vec4 p7 = glm::vec4(i, j-1, k, 1.0f);
                        glm::vec4 p8 = glm::vec4(i, j, k, 1.0f);

                        //the face can be drawn so it can be changed to a grass face if it is dirt
                        int new_block_type = replacefacetexture(i, j, k, Block_type);

                        UV1 = UVforFace( new_block_type, 's');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('r', p5, p6, p7, p8, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Left Face
                    if(TestFaceForDrawing('l', i, j, k))
                    {
                        glm::vec4 p9 = glm::vec4(i-1, j, k, 1.0f);
                        glm::vec4 p10 = glm::vec4(i-1, j-1, k, 1.0f);
                        glm::vec4 p11 = glm::vec4(i-1, j-1, k-1, 1.0f);
                        glm::vec4 p12 = glm::vec4(i-1, j, k-1, 1.0f);

                        //the face can be drawn so it can be changed to a grass face if it is dirt
                        int new_block_type = replacefacetexture(i, j, k, Block_type);

                        UV1 = UVforFace(new_block_type, 's');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('l', p9, p10, p11, p12, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Back Face
                    if(TestFaceForDrawing('b', i, j, k))
                    {
                        glm::vec4 p13 = glm::vec4(i-1, j, k-1, 1.0f);
                        glm::vec4 p14 = glm::vec4(i-1, j-1, k-1, 1.0f);
                        glm::vec4 p15 = glm::vec4(i, j-1, k-1, 1.0f);
                        glm::vec4 p16 = glm::vec4(i, j, k-1, 1.0f);

                        //the face can be drawn so it can be changed to a grass face if it is dirt
                        int new_block_type = replacefacetexture(i, j, k, Block_type);

                        UV1 = UVforFace( new_block_type, 's');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('b', p13, p14, p15, p16, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Top Face
                    if(TestFaceForDrawing('t', i, j, k))
                    {
                        glm::vec4 p17 = glm::vec4(i, j, k-1, 1.0f);
                        glm::vec4 p18 = glm::vec4(i, j, k, 1.0f);
                        glm::vec4 p19 = glm::vec4(i-1, j, k, 1.0f);
                        glm::vec4 p20 = glm::vec4(i-1, j, k-1, 1.0f);

                        //the face can be drawn so it can be changed to a grass face if it is dirt
                        int new_block_type = replacefacetexture(i, j, k, Block_type);

                        UV1 = UVforFace(new_block_type, 't');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('t', p17, p18, p19, p20, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Bottom Face                    
                    if(TestFaceForDrawing('d', i, j, k))
                    {
                        glm::vec4 p21 = glm::vec4(i, j-1, k, 1.0f);
                        glm::vec4 p22 = glm::vec4(i, j-1, k-1, 1.0f);
                        glm::vec4 p23 = glm::vec4(i-1, j-1, k-1, 1.0f);
                        glm::vec4 p24 = glm::vec4(i-1, j-1, k, 1.0f);

                        //the face can be drawn so it can be changed to a grass face if it is dirt
                        int new_block_type = replacefacetexture(i, j, k, Block_type);

                        UV1 = UVforFace( new_block_type, 'b');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('d', p21, p22, p23, p24, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }
                }
            }
        }
    }
    return flag_local;
}

bool BlockList::RecreatingChunkHelper(int min_x, int max_x, int min_y, int max_y, int min_z, int max_z)
{
    bool flag_local = false;
    for(float i=min_x; i<max_x ;i++)
    {
        for(float j=min_y+1; j<=max_y ;j++)
        {
            for(float k=min_z+1; k<=max_z ;k++)
            {
                //check if the block exitst if so add the block to the VBOs
                std::tuple<int, int, int> key (i, j, k);
                std::map<std::tuple<int, int, int>, int>::iterator it = BlockMap.find(key);

                if(it != BlockMap.end()) // the block exists
                {
                    flag_local = true;
                    //int Block_type = getBlockType(key);
                    int Block_type = it->second;
                    //Add the blocks position color index and nramls to the VBO
                    //Add 24 positions for the block
                    //Add 36 indices
                    //Add 24 colors
                    //Add 24 normals

                    //for every face check if it should be drawn depending on if
                    //there is a block touching that face

                    glm::vec2 UV1;
                    glm::vec2 UV2;
                    glm::vec2 UV3;
                    glm::vec2 UV4;

                    float shininess = BlockShininess(Block_type);
                    int flag_texture_animate = AnimateTexture(Block_type);

                    //Front Face
                    if(TestFaceForDrawing('f', i, j, k))
                    {
                        glm::vec4 p1 = glm::vec4(i, j, k, 1.0f);
                        glm::vec4 p2 = glm::vec4(i, j-1, k, 1.0f);
                        glm::vec4 p3 = glm::vec4(i-1, j-1, k, 1.0f);
                        glm::vec4 p4 = glm::vec4(i-1, j, k, 1.0f);
                        //if we go counter-clockwise and create a one-one mapping of vertex with UV co-ordinates,
                        //the UV co-ordinates witll be as shown below
                        UV1 = UVforFace( Block_type, 's');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('f', p1, p2, p3, p4, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Right Face
                    if(TestFaceForDrawing('r', i, j, k))
                    {
                        glm::vec4 p5 = glm::vec4(i, j, k-1, 1.0f);
                        glm::vec4 p6 = glm::vec4(i, j-1, k-1, 1.0f);
                        glm::vec4 p7 = glm::vec4(i, j-1, k, 1.0f);
                        glm::vec4 p8 = glm::vec4(i, j, k, 1.0f);

                        UV1 = UVforFace( Block_type, 's');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('r', p5, p6, p7, p8, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Left Face
                    if(TestFaceForDrawing('l', i, j, k))
                    {
                        glm::vec4 p9 = glm::vec4(i-1, j, k, 1.0f);
                        glm::vec4 p10 = glm::vec4(i-1, j-1, k, 1.0f);
                        glm::vec4 p11 = glm::vec4(i-1, j-1, k-1, 1.0f);
                        glm::vec4 p12 = glm::vec4(i-1, j, k-1, 1.0f);

                        UV1 = UVforFace( Block_type, 's');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('l', p9, p10, p11, p12, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Back Face
                    if(TestFaceForDrawing('b', i, j, k))
                    {
                        glm::vec4 p13 = glm::vec4(i-1, j, k-1, 1.0f);
                        glm::vec4 p14 = glm::vec4(i-1, j-1, k-1, 1.0f);
                        glm::vec4 p15 = glm::vec4(i, j-1, k-1, 1.0f);
                        glm::vec4 p16 = glm::vec4(i, j, k-1, 1.0f);

                        UV1 = UVforFace( Block_type, 's');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('b', p13, p14, p15, p16, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Top Face
                    if(TestFaceForDrawing('t', i, j, k))
                    {
                        glm::vec4 p17 = glm::vec4(i, j, k-1, 1.0f);
                        glm::vec4 p18 = glm::vec4(i, j, k, 1.0f);
                        glm::vec4 p19 = glm::vec4(i-1, j, k, 1.0f);
                        glm::vec4 p20 = glm::vec4(i-1, j, k-1, 1.0f);

                        UV1 = UVforFace( Block_type, 't');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('t', p17, p18, p19, p20, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }

                    //Bottom Face
                    if(TestFaceForDrawing('d', i, j, k))
                    {
                        glm::vec4 p21 = glm::vec4(i, j-1, k, 1.0f);
                        glm::vec4 p22 = glm::vec4(i, j-1, k-1, 1.0f);
                        glm::vec4 p23 = glm::vec4(i-1, j-1, k-1, 1.0f);
                        glm::vec4 p24 = glm::vec4(i-1, j-1, k, 1.0f);

                        UV1 = UVforFace( Block_type, 'b');
                        UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
                        UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
                        UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

                        CreateFaceInOrder('d', p21, p22, p23, p24, UV1, UV2, UV3, UV4, shininess, flag_texture_animate);
                        CreateIndexListForFace(facecount);
                        facecount++;
                    }
                }
            }
        }
    }
    return flag_local;
}

int BlockList::replacefacetexture(int x, int y , int z, int OldBlockType)
{
    switch(OldBlockType)
    {
    //so far the only case is for grass becoming dirt
        case DIRT:
        {
            //called when
            //if it's a face we can initially see then it remains
            //grassy other wise its made into dirt
            std::tuple<int, int, int> key (x, y, z);
            DeleteBlock(key);
            AddBlock(key, GRASS);
            return GRASS;
            //break;
        }
        default:
        {
            return OldBlockType;
            break;
        }
    }    
}

int BlockList::AnimateTexture(int block_type)
{
    switch(block_type)
    {
        case WATER:
            return 1;
            break;
        case GRASS:
            return 0;
            break;
        case LAVA:
            return 1;
            break;
        case DIRT:
            return 0;
            break;
        case WOOD:
            return 0;
            break;
        case LEAF:
            return 0;
            break;
        case STONE:
            return 0;
            break;
        case BEDROCK:
            return 0;
            break;
        case COAL:
            return 0;
            break;
        case IRONORE:
            return 0;
            break;
        case CRYSTAL:
            return 0;
            break;
        case BRICK:
            return 0;
            break;
        case CLOUD:
            return 2;
            break;
        default:
        {
            return 0;
            break;
        }
    }
}

bool BlockList::TestFaceForDrawing(char c, int x, int y, int z)
{
    bool ret = false;
    switch(c)
    {
        case 'f':
        {
            //for front face only x,y values need to be checked to see if another block exists there
            std::tuple<int, int, int> key (x, y, (z+1));
            std::map<std::tuple<int, int, int>, int>::iterator it = BlockMap.find(key);
            if(it == BlockMap.end())
            {
                //block does not exist so the face can be drawn
                ret = true;
            }
            break;
        }
        case 'r':
        {
            //for right face only y,z values need to be checked to see if another block exists there
            std::tuple<int, int, int> key ((x+1), y, z);
            std::map<std::tuple<int, int, int>, int>::iterator it = BlockMap.find(key);
            if(it == BlockMap.end())
            {
                //block does not exist so the face can be drawn
                ret = true;
            }
            break;
        }
        case 'l':
        {
            //for left face only y,z values need to be checked to see if another block exists there
            std::tuple<int, int, int> key ((x-1), y, z);
            std::map<std::tuple<int, int, int>, int>::iterator it = BlockMap.find(key);
            if(it == BlockMap.end())
            {
                //block does not exist so the face can be drawn
                ret = true;
            }
            break;
        }
        case 'b':
        {
            //for back face only x,y values need to be checked to see if another block exists there
            std::tuple<int, int, int> key (x, y, (z-1));
            std::map<std::tuple<int, int, int>, int>::iterator it = BlockMap.find(key);
            if(it == BlockMap.end())
            {
                //block does not exist so the face can be drawn
                ret = true;
            }
            break;
        }
        case 't':
        {
            //for top face only x,z values need to be checked to see if another block exists there
            std::tuple<int, int, int> key (x, (y+1), z);
            std::map<std::tuple<int, int, int>, int>::iterator it = BlockMap.find(key);
            if(it == BlockMap.end())
            {
                //block does not exist so the face can be drawn
                ret = true;
            }
            break;
        }
        case 'd':
        {
            //for down/bottom face only x,z values need to be checked to see if another block exists there
            std::tuple<int, int, int> key (x, (y-1), z);
            if(BlockMap.find(key) == BlockMap.end())
            {
                //block does not exist so the face can be drawn
                ret = true;
            }
            break;
        }
    }
    return ret;
}

void BlockList::CreateFaceInOrder(char c, glm::vec4 pos1, glm::vec4 pos2, glm::vec4 pos3, glm::vec4 pos4,
                                  glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4, float shiny,
                                  int flag_animation)
{  
    glm::vec4 dirtBrown = glm::vec4(0.545098f, 0.270588f, 0.0745098f, 1.0f);
    glm::vec4 grassyGreen = glm::vec4(0.180392f, 0.545098f, 0.341176f, 1.0f);
    glm::vec4 fadeToBlack = glm::vec4(0.662745f, 0.662745f, 0.662745f, 1.0f);

    glm::vec4 Color = grassyGreen;

    glm::vec4 Normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

    switch(c)
    {
        case 'f':
            Color = dirtBrown;
            Normal = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
            break;
        case 'r':
            Color = dirtBrown;
            Normal = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
            break;
        case 'l':
            Color = dirtBrown;
            Normal = glm::vec4(-1.0f, 0.0f, 0.0f, 0.0f);
            break;
        case 'b':
            Color = dirtBrown;
            Normal = glm::vec4(0.0f, 0.0f, -1.0f, 0.0f);
            break;
        case 't':
            Color = grassyGreen;
            Normal = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
            break;
        case 'd':
            Color = fadeToBlack;
            Normal = glm::vec4(0.0f, -1.0f, 0.0f, 0.0f);
            break;
    }

    glm::vec4 edge1 = pos2 - pos1;
    glm::vec4 edge2 = pos3 - pos1;
    glm::vec2 deltaUV1 = uv3 - uv2;
    glm::vec2 deltaUV2 = uv4 - uv2;

    glm::vec4 tangenttemp;
    glm::vec4 bitangenttemp;

    GLfloat f = 1.0f / (deltaUV1.x * deltaUV2.y - deltaUV2.x * deltaUV1.y);

    tangenttemp[0] = f * (deltaUV2.y * edge1.x - deltaUV1.y * edge2.x);
    tangenttemp[1] = f * (deltaUV2.y * edge1.y - deltaUV1.y * edge2.y);
    tangenttemp[2] = f * (deltaUV2.y * edge1.z - deltaUV1.y * edge2.z);
    tangenttemp[3] = 0.0;
    tangenttemp = glm::normalize(tangenttemp);

    bitangenttemp[0] = f * (-deltaUV2.x * edge1.x + deltaUV1.x * edge2.x);
    bitangenttemp[1] = f * (-deltaUV2.x * edge1.y + deltaUV1.x * edge2.y);
    bitangenttemp[2] = f * (-deltaUV2.x * edge1.z + deltaUV1.x * edge2.z);
    bitangenttemp[3] = 0.0;
    bitangenttemp = glm::normalize(bitangenttemp);

    vertex_data temp1, temp2, temp3, temp4;
    //The order of the UV's is weird in that it sets temp1.uv to uv2 and so on
    //and so forth but without this the textures would be sideways
    temp1.pos = pos1;
    temp1.color = Color;
    temp1.normal = Normal;
    temp1.uv = uv2;
    temp1.tangent = tangenttemp;
    temp1.bitangent = bitangenttemp;
    temp1.shininess = shiny;
    temp1.flag_animate = flag_animation;
    VertexList.push_back(temp1);

    temp2.pos = pos2;
    temp2.color = Color;
    temp2.normal = Normal;
    temp2.uv = uv3;
    temp2.tangent = tangenttemp;
    temp2.bitangent = bitangenttemp;
    temp2.shininess = shiny;
    temp2.flag_animate = flag_animation;
    VertexList.push_back(temp2);

    temp3.pos = pos3;
    temp3.color = Color;
    temp3.normal = Normal;
    temp3.uv = uv4;
    temp3.tangent = tangenttemp;
    temp3.bitangent = bitangenttemp;
    temp3.shininess = shiny;
    temp3.flag_animate = flag_animation;
    VertexList.push_back(temp3);

    temp4.pos = pos4;
    temp4.color = Color;
    temp4.normal = Normal;
    temp4.uv = uv1;
    temp4.tangent = tangenttemp;
    temp4.bitangent = bitangenttemp;
    temp4.shininess = shiny;
    temp4.flag_animate = flag_animation;
    VertexList.push_back(temp4);
}

void BlockList::CreateIndexListForFace(int face_count)
{
    IndexList.push_back(face_count*4);
    IndexList.push_back(face_count*4+1);
    IndexList.push_back(face_count*4+2);
    IndexList.push_back(face_count*4);
    IndexList.push_back(face_count*4+2);
    IndexList.push_back(face_count*4+3);
}

GLenum BlockList::drawMode()
{
    return GL_TRIANGLES;
}

//Getter
std::map<std::tuple<int, int, int>, int> BlockList::getBlockMap()
{
    return BlockMap;
}

int BlockList::getBlockType(std::tuple<int, int, int> block_pos)
{
    return BlockMap[block_pos];
}

void BlockList::setBlockType(int type, std::tuple<int, int, int> block_pos)
{
    BlockMap[block_pos] = type;
}

int BlockList::getChunkID(std::tuple<int, int, int> chunk_pos)
{
    return ChunkMap[chunk_pos];
}

//Setter
void BlockList::AddChunk(std::tuple<int, int, int> chunk_pos)
{
    int chunk_id = ChunkMap.size();
    ChunkMap.insert(std::pair <
                         std::tuple<int, int, int>,
                         int >
                         (chunk_pos, chunk_id));
}

void BlockList::AddBlock(std::tuple<int, int, int> block_pos, int block_enum_type)
{
    BlockMap.insert(std::pair <
                    std::tuple<int, int, int>,
                    int >
                    (block_pos, block_enum_type));
}

void BlockList::DeleteBlock(std::tuple<int, int, int> block_pos)
{
    BlockMap.erase(block_pos);
//    std::map<std::tuple<int, int, int>, int>::iterator it = BlockMap.find(block_pos);
//    if (it== BlockMap.end()) {
//        std::cout << "it's gone" << std::endl;
//    } else {
//        std::cout << " :( " << std::endl;
//    }
}
