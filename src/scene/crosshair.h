#ifndef CROSSHAIR_H
#define CROSSHAIR_H

#include "drawable.h"
#include "la.h"
#include <iostream>

/*
 *
 * Crosshair at the center of the screen
 *
 */

class Crosshair : public Drawable
{
private:
protected:

public:
    Crosshair(GLWidget277 *context);
    ~Crosshair() {}

    int w, h;

    virtual void create();
    virtual GLenum drawMode();

    void CreateVertexPositions();
    void CreateVertexIndices();
    void CreateVertexColors();
};


#endif // CROSSHAIR_H
