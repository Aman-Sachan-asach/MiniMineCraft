#include "chunk.h"

Chunk::Chunk(): Drawable(nullptr)
{}
Chunk::Chunk(GLWidget277* context, glm::vec4 center):
    Drawable(context),
    chunk_center(center),
    BlocksInChunk()
{}

void Chunk::createchunk()
{
    //chunks are distributed along the XZ plane with the y column being 256 blocks long from -128 to 127
    float base_i = chunk_center[0] - 7.5f;
    float base_j = chunk_center[1] - 7.5f;
    float base_k = chunk_center[2] - 7.5f;

    ///Fill in every 16x256x16 chunk with blocks
    for(int i=0; i<16; i++)
    {
        for(int k=0; k<16; k++)
        {
            float perlin_value = 30.0f * std::fabs(scene->perlinNoise2D(std::abs(i) / 4.0f,
                                                                        std::abs(k) / 4.0f));

            for(int h=-128; h<0; h++)
            {
                BlocksInChunk.push_back(EMPTY);
            }
            for(int m = 0; m < perlin_value; m++)
            {
                //use perlin value to determine terrain height and make the rest empty blocks
                BlocksInChunk.push_back(DIRT);
            }
            for(int n=perlin_value; n<128; n++)
            {
                BlocksInChunk.push_back(EMPTY);
            }
        }
    }

    ///Now that all the blocks in the chunk have been assigned create VBO's
    CreateVBO();
}

void Chunk::CreateVBO()
{
    VertexList.clear();
    IndexList.clear();

    ChunkCreationHelper(chunk_center);

    ///pass things into vbos
    generateChunkVBO();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufChunk);
    context->glBufferData(GL_ARRAY_BUFFER,
                          VertexList.size()*sizeof(Vertex_Data), //size of the thing
                          VertexList.data(), //from where
                          GL_STATIC_DRAW);

    count = IndexList.size();
    generateChunkVBOIndex();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufChunkIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                          IndexList.size()*sizeof(GLuint),
                          IndexList.data(),
                          GL_STATIC_DRAW);
}

void Chunk::ChunkCreationHelper(glm::vec4 ChunkCenter)
{
    //chunk center is the position of the center of the chunk in block units
    float base_i = ChunkCenter[0] - 7.5f;    //glm::vec4(-7.5f,0,0,0);
    float base_j = ChunkCenter[1] - 7.5f;    //glm::vec4(0,-7.5f,0,0);
    float base_k = ChunkCenter[2] - 7.5f;    //glm::vec4(0,0,-7.5f,0);

    int index = -1;
    for(float i=0; i<16; i++)
    {
        for(float k=0; k<16; k++)
        {
            for(float j=-128; j<128; j++)
            {
                //int index = 128 + j + k*256 + i*256*16;
                index++;
                BlockType block_type = BlocksInChunk[index];

                if(block_type == EMPTY)
                {
                    break;
                }
                //Add the blocks position color index and nramls to the VBO
                //Add 24 positions for the block
                //Add 36 indices
                //Add 24 colors
                //Add 24 normals

                //for every face check if it should be drawn depending on if
                //there is a block touching that face

                glm::vec4 BlockCenter = glm::vec4(base_i + i, base_j + j, base_k + k, 0); //0 because will be added

                //to another vec4 position
                //Front face
                //UR
                glm::vec4 p1 = BlockCenter + glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
                //LR
                glm::vec4 p2 = BlockCenter + glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
                //LL
                glm::vec4 p3 = BlockCenter + glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
                //UL
                glm::vec4 p4 = BlockCenter + glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);

                //Right face
                //UR
                glm::vec4 p5 = BlockCenter + glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
                //LR
                glm::vec4 p6 = BlockCenter + glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
                //LL
                glm::vec4 p7 = BlockCenter + glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
                //UL
                glm::vec4 p8 = BlockCenter + glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);

                //Left face
                //UR
                glm::vec4 p9 = BlockCenter + glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
                //LR
                glm::vec4 p10 = BlockCenter + glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);
                //LL
                glm::vec4 p11 = BlockCenter + glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
                //UL
                glm::vec4 p12 = BlockCenter + glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);

                //Back face
                //UR
                glm::vec4 p13 = BlockCenter + glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);
                //LR
                glm::vec4 p14 = BlockCenter + glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
                //LL
                glm::vec4 p15 = BlockCenter + glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
                //UL
                glm::vec4 p16 = BlockCenter + glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);

                //Top face
                //UR
                glm::vec4 p17 = BlockCenter + glm::vec4(0.5f, 0.5f, -0.5f, 1.0f);
                //LR
                glm::vec4 p18 = BlockCenter + glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
                //LL
                glm::vec4 p19 = BlockCenter + glm::vec4(-0.5f, 0.5f, 0.5f, 1.0f);
                //UL
                glm::vec4 p20 = BlockCenter + glm::vec4(-0.5f, 0.5f, -0.5f, 1.0f);

                //Bottom face
                //UR
                glm::vec4 p21 = BlockCenter + glm::vec4(0.5f, -0.5f, 0.5f, 1.0f);
                //LR
                glm::vec4 p22 = BlockCenter + glm::vec4(0.5f, -0.5f, -0.5f, 1.0f);
                //LL
                glm::vec4 p23 = BlockCenter + glm::vec4(-0.5f, -0.5f, -0.5f, 1.0f);
                //UL
                glm::vec4 p24 = BlockCenter + glm::vec4(-0.5f, -0.5f, 0.5f, 1.0f);

                //find block and add the block to the VBO
                //int index = 128 + j + k*256 + i*256*16;
                //BlockType block_type = BlocksInChunk[index];

                float shininess = BlockShininess(block_type);
                int flag_texture_animate = AnimateTexture(block_type);

                //Front Face
                if(TestFaceForDrawing('f', i,j,k,index))
                {
                    PerFaceDetails('f',
                                   block_type, index,
                                   p1, p2, p3, p4,
                                   shininess,
                                   flag_texture_animate);
                }

                //Right Face
                if(TestFaceForDrawing('r', i,j,k,index))
                {
                    PerFaceDetails('r',
                                   block_type, index,
                                   p5, p6, p7, p8,
                                   shininess,
                                   flag_texture_animate);
                }

                //Left Face
                if(TestFaceForDrawing('l', i,j,k,index))
                {
                    PerFaceDetails('l',
                                   block_type, index,
                                   p9, p10, p11, p12,
                                   shininess,
                                   flag_texture_animate);
                }

                //Back Face
                if(TestFaceForDrawing('b', i,j,k,index))
                {
                    PerFaceDetails('b',
                                   block_type, index,
                                   p13, p14, p15, p16,
                                   shininess,
                                   flag_texture_animate);
                }

                //Top Face
                if(TestFaceForDrawing('t', i,j,k,index))
                {
                    PerFaceDetails('t',
                                   block_type, index,
                                   p17, p18, p19, p20,
                                   shininess,
                                   flag_texture_animate);
                }

                //Bottom Face
                if(TestFaceForDrawing('d', i,j,k,index))
                {
                    PerFaceDetails('d',
                                   block_type, index,
                                   p21, p22, p23, p24,
                                   shininess,
                                   flag_texture_animate);
                }
            }
        }
    }
}

float Chunk::BlockShininess(Chunk::BlockType block_type)
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

int Chunk::AnimateTexture(Chunk::BlockType block_type)
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

glm::vec2 Chunk::UVforFace(Chunk::BlockType block_type, char face)
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
                case 'r': //side
                    uv = uv_GRASSside;
                    return uv;
                    break;
                case 'l': //side
                    uv = uv_GRASSside;
                    return uv;
                    break;
                case 'f': //side
                    uv = uv_GRASSside;
                    return uv;
                    break;
                case 'b': //side
                    uv = uv_GRASSside;
                    return uv;
                    break;
                case 'd': //bottom
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
                case 'r': //side
                    uv = uv_WOODside;
                    return uv;
                    break;
                case 'l': //side
                    uv = uv_WOODside;
                    return uv;
                    break;
                case 'f': //side
                    uv = uv_WOODside;
                    return uv;
                    break;
                case 'b': //side
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

bool Chunk::TestFaceForDrawing(char c, int x, int y, int z, int index)
{
    //this function will draw the hull of every chunk, instead of the hull of the entire world
//    bool ret = false;
//    switch(c)
//    {
//        case 'f':
//        {
//            if(z!=15)
//            {
//                //int temp_index = 128 + y + (z+1)*256 + x*256*16;
//                int temp_index = index + 256;
//                BlockType temp_block_type = BlocksInChunk[temp_index];
//                if(temp_block_type == EMPTY)
//                {
//                    //block does not exist so the face can be drawn
//                    ret = true;
//                }
//                break;
//            }
//            else
//            {
//                ret =true;
//            }
//        }
//        case 'r':
//        {
//            if(x!=15)
//            {
//                //int temp_index = 128 + y + z*256 + (x+1)*256*16;
//                int temp_index = index + 256*16;
//                BlockType temp_block_type = BlocksInChunk[temp_index];
//                if(temp_block_type == EMPTY)
//                {
//                    //block does not exist so the face can be drawn
//                    ret = true;
//                }
//                break;
//            }
//            else
//            {
//                ret =true;
//            }
//        }
//        case 'l':
//        {
//            if(x!=0)
//            {
//                //int temp_index = 128 + y + z*256 + (x-1)*256*16;
//                int temp_index = index - 256*16;
//                BlockType temp_block_type = BlocksInChunk[temp_index];
//                if(temp_block_type == EMPTY)
//                {
//                    //block does not exist so the face can be drawn
//                    ret = true;
//                }
//                break;
//            }
//            else
//            {
//                ret =true;
//            }
//        }
//        case 'b':
//        {
//            if(z!=0)
//            {
//                //int temp_index = 128 + y + (z-1)*256 + x*256*16;
//                int temp_index = index - 256;
//                BlockType temp_block_type = BlocksInChunk[temp_index];
//                if(temp_block_type == EMPTY)
//                {
//                    //block does not exist so the face can be drawn
//                    ret = true;
//                }
//                break;
//            }
//            else
//            {
//                ret =true;
//            }
//        }
//        case 't':
//        {
//            if(y!=127)
//            {
//                //int temp_index = 128 + (y+1) + z*256 + x*256*16;
//                int temp_index = index + 1;
//                BlockType temp_block_type = BlocksInChunk[temp_index];
//                if(temp_block_type == EMPTY)
//                {
//                    //block does not exist so the face can be drawn
//                    ret = true;
//                }
//                break;
//            }
//            else
//            {
//                ret =true;
//            }
//        }
//        case 'd':
//        {
//            if(y!=-128)
//            {
//                //int temp_index = 128 + (y-1) + z*256 + x*256*16;
//                int temp_index = index - 1;
//                BlockType temp_block_type = BlocksInChunk[temp_index];
//                if(temp_block_type == EMPTY)
//                {
//                    //block does not exist so the face can be drawn
//                    ret = true;
//                }
//                break;
//            }
//            else
//            {
//                ret =true;
//            }
//        }
//    }
//    return ret;
    return true;
}

void Chunk::PerFaceDetails(char c,
                           BlockType Block_type, int index,
                           glm::vec4 pos1, glm::vec4 pos2, glm::vec4 pos3, glm::vec4 pos4,
                           float shiny,
                           int flag_animation)
{
    //if we go counter-clockwise and create a one-one mapping of vertex with UV co-ordinates,
    //the UV co-ordinates will be as shown below

    //the face can be drawn so it can be changed to a grass face if it is dirt
    BlockType new_block_type = replacefacetexture(index, Block_type);

    glm::vec2 UV1 = UVforFace( new_block_type, 'c');
    glm::vec2 UV2 = glm::vec2((UV1[0] + UV_block_stride), UV1[1]);
    glm::vec2 UV3 = glm::vec2((UV1[0] + UV_block_stride), (UV1[1] + UV_block_stride));
    glm::vec2 UV4 = glm::vec2( UV1[0], (UV1[1] + UV_block_stride));

    CreateFaceInOrder('c', pos1, pos2, pos3, pos4, UV1, UV2, UV3, UV4, shiny, flag_animation);
    CreateIndexListForFace(facecount);
    facecount++;
}

Chunk::BlockType Chunk::replacefacetexture(int index, int OldBlockType)
{
    //this function is also only capable of working correctly inside its own chunk, nit with respect to the world
    switch(OldBlockType)
    {
    //so far the only case is for grass becoming dirt
        case DIRT:
        {
            //called when
            //if it's a face we can initially see then it remains
            //grassy other wise its made into dirt

            BlocksInChunk[index] = GRASS;
            return GRASS;
            //break;
        }
        default:
        {
            return (Chunk::BlockType)OldBlockType;
            break;
        }
    }
}

void Chunk::CreateFaceInOrder(char c,
                                  glm::vec4 pos1, glm::vec4 pos2, glm::vec4 pos3, glm::vec4 pos4,
                                  glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4,
                                  float shiny,
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

    Vertex_Data temp1, temp2, temp3, temp4;
    //The order of the UV's is weird in that it sets temp1.uv to uv2 and so on
    //and so forth but without this the textures would be sideways
    temp1.pos = pos1;
    temp1.color = Color;
    temp1.normal = Normal;
    temp1.uv = uv1;
    temp1.tangent = tangenttemp;
    temp1.bitangent = bitangenttemp;
    temp1.shininess = shiny;
    temp1.flag_animate = flag_animation;
    VertexList.push_back(temp1);

    temp2.pos = pos2;
    temp2.color = Color;
    temp2.normal = Normal;
    temp2.uv = uv2;
    temp2.tangent = tangenttemp;
    temp2.bitangent = bitangenttemp;
    temp2.shininess = shiny;
    temp2.flag_animate = flag_animation;
    VertexList.push_back(temp2);

    temp3.pos = pos3;
    temp3.color = Color;
    temp3.normal = Normal;
    temp3.uv = uv3;
    temp3.tangent = tangenttemp;
    temp3.bitangent = bitangenttemp;
    temp3.shininess = shiny;
    temp3.flag_animate = flag_animation;
    VertexList.push_back(temp3);

    temp4.pos = pos4;
    temp4.color = Color;
    temp4.normal = Normal;
    temp4.uv = uv4;
    temp4.tangent = tangenttemp;
    temp4.bitangent = bitangenttemp;
    temp4.shininess = shiny;
    temp4.flag_animate = flag_animation;
    VertexList.push_back(temp4);
}

void Chunk::CreateIndexListForFace(int face_count)
{
    IndexList.push_back(face_count*4);
    IndexList.push_back(face_count*4+1);
    IndexList.push_back(face_count*4+2);
    IndexList.push_back(face_count*4);
    IndexList.push_back(face_count*4+2);
    IndexList.push_back(face_count*4+3);
}

void Chunk::Update()
{
    //Assumes that the appropriate blocks have been added or removed from the Chunk,
    //and will simply reset the VBO to accommodate the new data

    //only difference between this and the  createVBO function is that this one
    //does not have to generate VBO handles
    VertexList.clear();
    IndexList.clear();

    ChunkCreationHelper(chunk_center);

    ///pass things into vbos
    //Don't call generateVBO(); because the handle has already been generated previously

    context->glBindBuffer(GL_ARRAY_BUFFER, bufChunk);
    context->glBufferData(GL_ARRAY_BUFFER,
                          VertexList.size()*sizeof(vertex_data), //size of the thing
                          VertexList.data(), //from where
                          GL_STATIC_DRAW);

    //Don't call generateVBOIndex(); because the handle has already been generated previously
    count = IndexList.size();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufChunkIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                          IndexList.size()*sizeof(GLuint),
                          IndexList.data(),
                          GL_STATIC_DRAW);
}

void Chunk::AddBlock(float posx, float posy, float posz, Chunk::BlockType block_type)
{
    //takes in world space co-ordinatesand checks if that position is in the chunk
    if((fabs(chunk_center[0] - posx) <= 8) &&
       (fabs(chunk_center[1] - posy) <= 8) &&
       (fabs(chunk_center[2] - posz) <= 8))
    {
        //The block to be added lies inside the chunk
        float base_i = chunk_center[0] - 7.5f;
        float base_j = chunk_center[1] - 7.5f;
        float base_k = chunk_center[2] - 7.5f;

        int i = posx - base_i;
        int j = posy - base_j;
        int k = posz - base_k;

        int index = 128 + j + k*256 + i*256*16;
        if(BlocksInChunk[index] == EMPTY)
        {
            BlocksInChunk[index] = block_type;
        }
    }
}

void Chunk::DeleteBlock(float posx, float posy, float posz)
{
    //takes in world space co-ordinatesand checks if that position is in the chunk
    if((fabs(chunk_center[0] - posx) <= 8) &&
       (fabs(chunk_center[1] - posy) <= 8) &&
       (fabs(chunk_center[2] - posz) <= 8))
    {
        //The block lies inside the chunk
        float base_i = chunk_center[0] - 7.5f;
        float base_j = chunk_center[1] - 7.5f;
        float base_k = chunk_center[2] - 7.5f;

        int i = posx - base_i;
        int j = posy - base_j;
        int k = posz - base_k;

        int index = 128 + j + k*256 + i*256*16;
        BlocksInChunk[index] = EMPTY;
    }
}

void Chunk::create()
{}

GLenum Chunk::drawMode()
{
    return GL_TRIANGLES;
}
