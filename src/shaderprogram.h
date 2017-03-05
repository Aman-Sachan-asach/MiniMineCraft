#pragma once

#include <glwidget277.h>
#include <la.h>
#include <glm/glm.hpp>

#include "drawable.h"
#include "scene/blocklist.h"
#include "scene/weather.h"
#include "scene/chunk.h"
#include "soil/Simple OpenGL Image Library/src/SOIL.h"
#include <stdio.h>
#include <stdlib.h>
#include <iostream>

class ShaderProgram
{
public:
    GLuint vertShader;    // A handle for the vertex shader stored in this shader program
    GLuint fragShader;    // A handle for the fragment shader stored in this shader program
    GLuint prog;          // A handle for the linked shader program stored in this class
    GLuint textureHandle; // A handle for the texture used in the prog_texture shader program
    GLuint normalHandle; // A handle for the normal used in the prog_texture shader program
    GLuint texture_general_handle;
    GLuint snow_handle;
    GLuint rain_handle;

    int attrPos; // A handle for the "in" vec4 representing vertex position in the vertex shader
    int attrNor; // A handle for the "in" vec4 representing vertex normal in the vertex shader
    int attrCol; // A handle for the "in" vec4 representing vertex color in the vertex shader

    int unifModel; // A handle for the "uniform" mat4 representing model matrix in the vertex shader
    int unifModelInvTr; // A handle for the "uniform" mat4 representing inverse transpose of the model matrix in the vertex shader
    int unifViewProj; // A handle for the "uniform" mat4 representing combined projection and view matrices in the vertex shader
    int unifColor; // A handle for the "uniform" vec4 representing color of geometry in the vertex shader
    int unifCameraEye; // A handle for the "uniform" vec4 representing the position of the eye of the camera

    int attrUVs;
    int attrtangent;
    int attrbitangent;
    int attrshininess;
    int attranimatetexture_flag;

    int unifTextureSampler;
    int unifNormalSampler;
    int unifSnowSampler;
    int unifRainSampler;

    int unifTimeCount;
    int unifHeight;
    int unifLIntensity;
    int unifLColor;
    int unifLDirection;

    unsigned char *image;

public:
    ShaderProgram(GLWidget277* context);
    // Sets up the requisite GL data and shaders from the given .glsl files
    void create(const char *vertfile, const char *fragfile);
    // Tells our OpenGL context to use this shader to draw things
    void useMe();

    void setActiveTexture_General(int tex_slot_num);
    void setupTexture_General(const char* imagepath, int& width, int& height, int tex_slot_num, bool transparnecy_flag);

    void setActiveTexture();
    void setupTexture(const char* imagepath, int& width, int& height);

    void setActiveNormalMap();
    void setupNormalMap(const char* imagepath, int& width, int& height);

    void setTimeCount(int timecount);
    void setHeight(int h);
    void setCameraEye(glm::vec3 camera_eye);

    void setLIntensity(float lintensity);
    void setLColor(glm::vec4 lcolor);
    void setLDirection(glm::vec4 ldir);

    // Pass the given model matrix to this shader on the GPU
    void setModelMatrix(const glm::mat4 &model);
    // Pass the given Projection * View matrix to this shader on the GPU
    void setViewProjMatrix(const glm::mat4 &vp);
    // Pass the given color to this shader on the GPU
    void setGeometryColor(glm::vec4 color);
    // Draw the given object to our screen using this ShaderProgram's shaders
    void draw(Drawable &d);

    void VBOdraw(Drawable &d, int index);
    void DrawWeather(Drawable &d);
    void DrawChunk(Drawable &d);

    // Utility function used in create()
    char* textFileRead(const char*);
    // Utility function that prints any shader compilation errors to the console
    void printShaderInfoLog(int shader);
    // Utility function that prints any shader linking errors to the console
    void printLinkInfoLog(int prog);


    QString qTextFileRead(const char*);

private:
    GLWidget277* context;   // Since Qt's OpenGL support is done through classes like QOpenGLFunctions_3_2_Core,
                            // we need to pass our OpenGL context to the Drawable in order to call GL functions
                            // from within this class.
};
