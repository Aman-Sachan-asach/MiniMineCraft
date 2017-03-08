#include "drawable.h"
#include <la.h>

Drawable::Drawable(GLWidget277* context)
    : bufIdx(), bufPos(), bufNor(), bufCol(),
      idxBound(false), posBound(false), norBound(false), colBound(false),
      context(context)
{}


void Drawable::destroy()
{
    context->glDeleteBuffers(1, &bufIdx);
    context->glDeleteBuffers(1, &bufPos);
    context->glDeleteBuffers(1, &bufNor);
    context->glDeleteBuffers(1, &bufCol);

    for(int i=0; i<VBOList.size();i++)
    {
        context->glDeleteBuffers(1, &VBOList[i]);
    }
}

GLenum Drawable::drawMode()
{
    return GL_TRIANGLES;
}

int Drawable::elemCount()
{
    return count;
}

//functions with General in their name are used for things like weather,
//i.e things that aren't standard blocks that make up the minecraft world

//the funvtions that don't have General in their name are used to deal
//with actual blocks in the minecraft world
void Drawable::generateGeneralVBOs(int IndexVBO_size)
{
    General_VBOBound = true;
    VBOindex_count = IndexVBO_size;
    context->glGenBuffers(1, &VBO_handle);
    context->glGenBuffers(1, &IndexVBO_handle);
}

void Drawable::generateNewUVs(GLuint index)
{
    UVBound.push_back(true);
    bufUV.push_back((GLuint) index);
    context->glGenBuffers(1, &bufUV[index]);
}

void Drawable::generateNewVBO(GLuint index)
{
    VBOBound.push_back(true);
    //VBOList.push_back((GLuint) index); //junk temporary value to ensure
    //that memory has been reserved for generated Buffer handle
    // Create a VBO on our GPU and store its handle in VBOList[index]
    VBOList.push_back((GLuint) index);
    context->glGenBuffers(1, &VBOList[index]);
}

void Drawable::generateNewVBOIndex(int index)
{
    VBOIndexList.push_back((GLuint) index);
    context->glGenBuffers(1, &VBOIndexList[index]);
}

void Drawable::RegenerateNewUVs(GLuint index)
{
    UVBound[index] = true;
    bufUV[index] = (GLuint) index;
    context->glGenBuffers(1, &bufUV[index]);
}

void Drawable::RegenerateNewVBO(GLuint index)
{
    VBOBound[index] = true;
    //VBOList.push_back((GLuint) index); //junk temporary value to ensure
    //that memory has been reserved for generated Buffer handle
    // Create a VBO on our GPU and store its handle in VBOList[index]
    VBOList[index] = (GLuint)index;
    context->glGenBuffers(1, &VBOList[index]);
}

void Drawable::RegenerateNewVBOIndex(int index)
{
    VBOIndexList[index] = (GLuint)index;
    context->glGenBuffers(1, &VBOIndexList[index]);
}

void Drawable::DeleteUVsHandle(GLuint index)
{
    UVBound[(int)index] = false;
    bufUV[index] = (GLuint) (-99);
    context->glDeleteBuffers(1, &bufUV[index]);
}

void Drawable::DeleteVBOHandle(GLuint index)
{
    VBOBound[(int)index] = false;
    //VBOList.push_back((GLuint) index); //junk temporary value to ensure
    //that memory has been reserved for generated Buffer handle
    // Create a VBO on our GPU and store its handle in VBOList[index]
    VBOList[index] =(GLuint) (-99);
    context->glDeleteBuffers(1, &VBOList[index]);
}

void Drawable::DeleteVBOIndex(int index)
{
    VBOIndexList[index] = -99;
    context->glDeleteBuffers(1, &VBOIndexList[index]);
}

bool Drawable::bindGeneralVBO()
{
    if(General_VBOBound)
    {
        context->glBindBuffer(GL_ARRAY_BUFFER, VBO_handle);
        context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBO_handle);
    }
    return General_VBOBound;
}

bool Drawable::bindVBO(int index)
{
    if(VBOBound[index]) {
        context->glBindBuffer(GL_ARRAY_BUFFER, VBOList[index]);
        context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, VBOIndexList[index]);
    }
    return VBOBound[index];
}

//------------- The non-interleaved VBO setup;
//used for simple geometry like crosshairs, and the player (just a long cube),etc ------------

//Function calls that deal with creating handles for buffers

void Drawable::generateIdx()
{
    idxBound = true;
    // Create a VBO on our GPU and store its handle in bufIdx
    context->glGenBuffers(1, &bufIdx);
}

void Drawable::generatePos()
{
    posBound = true;
    // Create a VBO on our GPU and store its handle in bufPos
    context->glGenBuffers(1, &bufPos);
}

void Drawable::generateNor()
{
    norBound = true;
    // Create a VBO on our GPU and store its handle in bufNor
    context->glGenBuffers(1, &bufNor);
}

void Drawable::generateCol()
{
    colBound = true;
    // Create a VBO on our GPU and store its handle in bufCol
    context->glGenBuffers(1, &bufCol);
}

//Function calls that deal with binding buffers

bool Drawable::bindIdx()
{
    if(idxBound) {
        context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    }
    return idxBound;
}

bool Drawable::bindPos()
{
    if(posBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    }
    return posBound;
}

bool Drawable::bindNor()
{
    if(norBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufNor);
    }
    return norBound;
}

bool Drawable::bindCol()
{
    if(colBound){
        context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    }
    return colBound;
}

//-------------------------------------------------------------------------------------
