#pragma once
#include "drawable.h"
#include "la.h"
#include <iostream>

class compass: public Drawable
{
public:
    compass(GLWidget277 *context);

    void create();
    virtual GLenum drawMode();
};
