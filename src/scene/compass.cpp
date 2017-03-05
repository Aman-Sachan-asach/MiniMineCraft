#include "compass.h"

compass::compass(GLWidget277 *context): Drawable(context)
{}

void compass::create()
{
    GLuint    idx[6];
    glm::vec4 pos[4];
    glm::vec4 col[4];

    // set idx
    for(int i = 0; i < 4; i++)
    {
        idx[i] = i;
    }

    // set pos
    // might be too small
    float screen_offset = 0.0f;

    pos[0] = glm::vec4(screen_offset,     screen_offset,     0,                 1.0f);     // left point
    pos[1] = glm::vec4(screen_offset + 1, screen_offset,     0,                 1.0f);     // right point
    pos[2] = glm::vec4(screen_offset,     screen_offset + 1, 0,                 1.0f);     // top point
    pos[3] = glm::vec4(screen_offset,     0,                 screen_offset + 1, 1.0f);     // bottom point

    // set colors
    col[0] = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //-y direction  = White
    col[1] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); //-x direction  = Red
    col[2] = glm::vec4(0.0f, 1.0f, 0.0f, 1.0f); //+x direction  = Green
    col[3] = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); //+y direction  = Blue

    // set idx
    idx[0] = 0;
    idx[1] = 1;
    idx[2] = 0;
    idx[3] = 2;
    idx[4] = 0;
    idx[5] = 3;

    count = 6;

    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, count * sizeof(GLuint), idx, GL_STATIC_DRAW);

    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), pos, GL_STATIC_DRAW);

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, count * sizeof(glm::vec4), col, GL_STATIC_DRAW);
}

GLenum compass::drawMode()
{
    return GL_LINES;
}
