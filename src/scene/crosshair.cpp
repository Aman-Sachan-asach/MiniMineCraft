#include "crosshair.h"

static const int CROSSHAIR_IDX_COUNT  = 4;
static const int CROSSHAIR_VERT_COUNT = 4;

Crosshair::Crosshair(GLWidget277 *context) :
    Drawable(context), w(), h()
{}

GLenum Crosshair::drawMode()
{
    return GL_LINES;
}

void Crosshair::create()
{
    GLuint    idx[CROSSHAIR_IDX_COUNT];
    glm::vec4 pos[CROSSHAIR_VERT_COUNT];
    glm::vec4 col[CROSSHAIR_VERT_COUNT];

    // set index
    for(int i = 0; i < 4; i++) {
        idx[i] = i;
    }

    // set pos
    pos[0] = glm::vec4(w/2 - .05, h/2, 0.0f, 1.0f);     // left point
    pos[1] = glm::vec4(w/2 + .05, h/2, 0.0f, 1.0f);     // right point
    pos[2] = glm::vec4(w/2, h/2 + .05, 0.0f, 1.0f);     // top point
    pos[3] = glm::vec4(w/2, h/2 - .05, 0.0f, 1.0f);     // bottom point

    // set colors
    for(int i = 0; i < 4; i++) {
        col[i] = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
    }

    // set idx
    for(int i = 0; i < 4; i++) {
        idx[i] = i;
    }

    count = CROSSHAIR_IDX_COUNT;

    generateIdx();
    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufIdx);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER, CROSSHAIR_IDX_COUNT * sizeof(GLuint), idx, GL_STATIC_DRAW);

    generatePos();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufPos);
    context->glBufferData(GL_ARRAY_BUFFER, CROSSHAIR_VERT_COUNT * sizeof(glm::vec4), pos, GL_STATIC_DRAW);

    generateCol();
    context->glBindBuffer(GL_ARRAY_BUFFER, bufCol);
    context->glBufferData(GL_ARRAY_BUFFER, CROSSHAIR_VERT_COUNT * sizeof(glm::vec4), col, GL_STATIC_DRAW);
}


