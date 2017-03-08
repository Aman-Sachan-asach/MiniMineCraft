#pragma once

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>

#include "camera.h"
#include <scene/scene.h>
#include <scene/blocklist.h>
#include <scene/crosshair.h>
#include "ray.h"
#include "scene/player.h"
#include "scene/compass.h"
#include "scene/weather.h"
#include <stdlib.h>
#include <la.h>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

#include <QOpenGLVertexArrayObject>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>

#include <QDir>

#include "behaviors.h"
#include <math.h>
#define M_PI 3.14159f


class MyGL
    : public GLWidget277
{
private:
    Camera gl_camera;
    Scene scene;
    Crosshair crosshair;
    compass Compass;
    Player player;      // two cubes just stacked on top of each other
    Weather weather_layer1;
    Weather weather_layer2;
    Weather weather_layer3;
    Weather weather_layer4;
    Weather weather_layer5;
    Weather weather_layer6;
    Weather weather_layer7;
    Weather weather_layer8;
    Weather weather_layer9;
    Weather weather_layer10;
    Weather weather_layer11;
    Weather weather_layer12;

    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram prog_texture; //A shader program that uses lambertian lighting/shading and texture mapping

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.

    bool Flag_PositionChanged = false;
    bool flag_PlayerPositionUpdate = false;
    bool flag_snow;
    bool flag_rain;
    bool POV; // toggle between player and sandbox POV
    bool moveForward, moveBackward, moveLeft, moveRight, jump;

    glm::vec4 fixed_reference_vector = glm::vec4(0,0,-1,0);
    glm::vec4 fixed_billboard_reference_vector = glm::vec4(0,0,-1,0);

    float theta = 0.0f;
    float a =0.0f;

    QPoint rotVal;
    glm::vec3 r;
    glm::vec3 u;
    bool pitch;

    float acceleration = 9.8f;
    float dT = (.2f);

    QString localDir = QDir::currentPath();
    QString minecraft_texture_atlas = localDir + "/minecraft_textures_all/minecraft_textures_all.png";
    QString minecraft_normal_atlas = localDir + "/minecraft_textures_all/minecraft_normals_all.png";
    QString snow_tex = localDir + "/Other Textures/snow_alpha.png";
    QString rain_tex = localDir + "/Other Textures/rain1.png";

    int weather_w = 600;
    int weather_h = 600;
    int w = 256;
    int h = 256;    

    /// Timer linked to timerUpdate(). Fires approx. 60 times per second
    QTimer timer;
    int timeCount = 0;
    int timeStep = 0;
    int timeStepDelta = 1;
    float alpha = 0;
    int weatherHeight = 0;

    //Day and Night Cycle Control Variables
    glm::vec4 BLEND_BACKGROUND  = glm::vec4(0.37f, 0.74f, 1.0f, 1);
    glm::vec4 LIGHTBLUE = glm::vec4(0.37f, 0.74f, 1.0f, 1);
    glm::vec4 DARKBLUE = glm::vec4(25, 25, 112, 1.0f);

    glm::vec4 BLEND_LIGHTCOLOR = glm::normalize(glm::vec4(255, 255, 153, 1.0f));
    glm::vec4 LIGHTYELLOW = glm::vec4(255, 255, 153, 1.0f);
    glm::vec4 DARKYELLOW = glm::vec4(102, 102, 0, 1.0f);
    glm::vec4 GREYISHYELLOW = glm::vec4(115, 117, 62, 1.0f);
    glm::vec4 BLACK = glm::vec4(0, 0, 0, 1.0f);
    glm::vec4 WHITE = glm::vec4(1, 1, 1, 1.0f);

    float LI_low = 0.7f;
    float LI_high = 1.0f;

    glm::vec4 CirclePointZ = glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    glm::vec4 CenterPoint = glm::vec4(0.0f,0.0f,0.0f, 1.0f);
    float current_rotation_degree = 0.0f;

public:
    explicit MyGL(QWidget *parent = 0);
    ~MyGL();

    BlockList BL;

    void initializeGL();
    void resizeGL(int w, int h);
    void paintGL();

    float timeOfDay(int _time);
    glm::mat4 FromToRotation(glm::vec4 fromDir, glm::vec4 toDir);

    //---------------------------------------- VBO Creation and Updation, and drawing the world ---------------------------------------
    void createFullWorld();
    void UpdateWorld();
    void AddDeleteUpdation(int x, int y, int z);
    void UpdateWorldBoundary(int x, int y, int z);
    //----------------------------------------------------------------------------------------------------------------

    //---------------------------------------- PERLIN NOISE TERRAIN GENERATION ---------------------------------------
    void drawPerlinScene();
    //----------------------------------------------------------------------------------------------------------------

    void orientCamera();
    void updatePlayer();
    bool checkCollision();
    bool isGrounded();
    void resetTranslations();

protected:
    void keyPressEvent(QKeyEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private slots:
    /// Slot that gets called ~60 times per second
    virtual void timerUpdate();
};
