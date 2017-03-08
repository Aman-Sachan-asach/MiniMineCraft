#include "player.h"

static const int PLAYER_IDX_COUNT = 36;
static const int PLAYER_VERT_COUNT = 24;

glm::vec4 GetPlayerNormal(const glm::vec4& P)
{
    int idx = 0;
    float val = -1;
    for(int i = 0; i < 6; i++){
        if(glm::abs(P[i]) > val){
            idx = i;
            val = glm::abs(P[i]);
        }
    }
    glm::vec4 N(0,0,0,0);
    N[idx] = glm::sign(P[idx]);
    return N;
}

//These are functions that are only defined in this cpp file. They're used for organizational purposes
//when filling the arrays used to hold the vertex and index data.
void createPlayerVertexPositions(glm::vec4 (&player_vert_pos)[PLAYER_VERT_COUNT])
{
    int idx = 0;
    ///////             Long Cube                ///////
    //Front face
    //UR
    player_vert_pos[idx++] = glm::vec4(0.5f, 1.f, 0.5f, 1.0f);
    //LR
    player_vert_pos[idx++] = glm::vec4(0.5f, -1.f, 0.5f, 1.0f);
    //LL
    player_vert_pos[idx++] = glm::vec4(-0.5f, -1.f, 0.5f, 1.0f);
    //UL
    player_vert_pos[idx++] = glm::vec4(-0.5f, 1.f, 0.5f, 1.0f);

    //Right face
    //UR
    player_vert_pos[idx++] = glm::vec4(0.5f, 1.f, -0.5f, 1.0f);
    //LR
    player_vert_pos[idx++] = glm::vec4(0.5f, -1.f, -0.5f, 1.0f);
    //LL
    player_vert_pos[idx++] = glm::vec4(0.5f, -1.f, 0.5f, 1.0f);
    //UL
    player_vert_pos[idx++] = glm::vec4(0.5f, 1.f, 0.5f, 1.0f);

    //Left face
    //UR
    player_vert_pos[idx++] = glm::vec4(-0.5f, 1.f, 0.5f, 1.0f);
    //LR
    player_vert_pos[idx++] = glm::vec4(-0.5f, -1.f, 0.5f, 1.0f);
    //LL
    player_vert_pos[idx++] = glm::vec4(-0.5f, -1.f, -0.5f, 1.0f);
    //UL
    player_vert_pos[idx++] = glm::vec4(-0.5f, 1.f, -0.5f, 1.0f);

    //Back face
    //UR
    player_vert_pos[idx++] = glm::vec4(-0.5f, 1.f, -0.5f, 1.0f);
    //LR
    player_vert_pos[idx++] = glm::vec4(-0.5f, -1.f, -0.5f, 1.0f);
    //LL
    player_vert_pos[idx++] = glm::vec4(0.5f, -1.f, -0.5f, 1.0f);
    //UL
    player_vert_pos[idx++] = glm::vec4(0.5f, 1.f, -0.5f, 1.0f);

    //Top face
    //UR
    player_vert_pos[idx++] = glm::vec4(0.5f, 1.f, -0.5f, 1.0f);
    //LR
    player_vert_pos[idx++] = glm::vec4(0.5f, 1.f, 0.5f, 1.0f);
    //LL
    player_vert_pos[idx++] = glm::vec4(-0.5f, 1.f, 0.5f, 1.0f);
    //UL
    player_vert_pos[idx++] = glm::vec4(-0.5f, 1.f, -0.5f, 1.0f);

    //Bottom face
    //UR
    player_vert_pos[idx++] = glm::vec4(0.5f, -1.f, 0.5f, 1.0f);
    //LR
    player_vert_pos[idx++] = glm::vec4(0.5f, -1.f, -0.5f, 1.0f);
    //LL
    player_vert_pos[idx++] = glm::vec4(-0.5f, -1.f, -0.5f, 1.0f);
    //UL
    player_vert_pos[idx++] = glm::vec4(-0.5f, -1.f, 0.5f, 1.0f);
}

void createPlayerVertexNormals(glm::vec4 (&player_vert_nor)[PLAYER_VERT_COUNT])
{
    int idx = 0;
    ///////             Long Cube                ///////
    //Front
    for(int i = 0; i < 4; i++){
        player_vert_nor[idx++] = glm::vec4(0,0,1,0);
    }
    //Right
    for(int i = 0; i < 4; i++){
        player_vert_nor[idx++] = glm::vec4(1,0,0,0);
    }
    //Left
    for(int i = 0; i < 4; i++){
        player_vert_nor[idx++] = glm::vec4(-1,0,0,0);
    }
    //Back
    for(int i = 0; i < 4; i++){
        player_vert_nor[idx++] = glm::vec4(0,0,-1,0);
    }
    //Top
    for(int i = 0; i < 4; i++){
        player_vert_nor[idx++] = glm::vec4(0,1,0,0);
    }
    //Bottom
    for(int i = 0; i < 4; i++){
        player_vert_nor[idx++] = glm::vec4(0,-1,0,0);
    }
}

void createPlayerIndices(GLuint (&player_idx)[PLAYER_IDX_COUNT])
{
    int idx = 0;
    for(int i = 0; i < 6; i++){
        player_idx[idx++] = i*4;
        player_idx[idx++] = i*4+1;
        player_idx[idx++] = i*4+2;
        player_idx[idx++] = i*4;
        player_idx[idx++] = i*4+2;
        player_idx[idx++] = i*4+3;
    }
}

GLenum Player::drawMode()
{
    return GL_TRIANGLES;
}

void Player::create()
{
    GLuint    player_idx[PLAYER_IDX_COUNT];
    glm::vec4 player_vert_pos[PLAYER_VERT_COUNT];
    glm::vec4 player_vert_nor[PLAYER_VERT_COUNT];
    glm::vec4 player_vert_col[PLAYER_VERT_COUNT];

    createPlayerVertexPositions(player_vert_pos);
    createPlayerVertexNormals(player_vert_nor);
    createPlayerIndices(player_idx);

    for(int i = 0; i < PLAYER_VERT_COUNT; i++){
        player_vert_col[i] = glm::vec4(1, 0, 0, 1);
    }

    count = PLAYER_IDX_COUNT;

    // Create a VBO on our GPU and store its handle in bufIdx
    generateIdx();
    // Tell OpenGL that we want to perform subsequent operations on the VBO referred to by bufIdx
    // and that it will be treated as an element array buffer (since it will contain triangle indices)
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    // Pass the data stored in cyl_idx into the bound buffer, reading a number of bytes equal to
    // SPH_IDX_COUNT multiplied by the size of a GLuint. This data is sent to the GPU to be read by shader programs.
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, PLAYER_IDX_COUNT * sizeof(GLuint), player_idx, GL_STATIC_DRAW);

    // The next few sets of function calls are basically the same as above, except bufPos and bufNor are
    // array buffers rather than element array buffers, as they store vertex attributes like position.
    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, PLAYER_VERT_COUNT * sizeof(glm::vec4), player_vert_pos, GL_STATIC_DRAW);

    generateNor();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    context->glBufferData(GL_ARRAY_BUFFER, PLAYER_VERT_COUNT * sizeof(glm::vec4), player_vert_nor, GL_STATIC_DRAW);

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, PLAYER_VERT_COUNT * sizeof(glm::vec4), player_vert_col, GL_STATIC_DRAW);
}

// only use this for the agents
void Player::setTarget(float x, float y, float z)
{
    this->target = glm::vec3(x, y, z);
}

void Player::printPosition()
{
    std::cout << "Position: " <<
                 this->pos.x << " | " <<
                 this->pos.y << " | " <<
                 this->pos.z <<
    std::endl;
}
