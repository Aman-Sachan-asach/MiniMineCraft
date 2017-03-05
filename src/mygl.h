#pragma once

#include <glwidget277.h>
#include <utils.h>
#include <shaderprogram.h>

#include <scene/cube.h>
#include "camera.h"
#include <scene/scene.h>
#include <scene/blocklist.h>
#include <scene/chunk.h>
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
    Cube geom_cube;// The instance of a unit cube we can use to render any cube. Should NOT be used in final version of your project.
    Crosshair crosshair;
    compass Compass;
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

    Weather weather_layer13;
    Weather weather_layer14;
    Weather weather_layer15;
    Weather weather_layer16;
    Weather weather_layer17;
    Weather weather_layer18;

    bool Flag_PositionChanged = false;

    int changecount =0;
    bool flag_PlayerPositionUpdate = false;

    glm::vec4 fixed_reference_vector = glm::vec4(0,0,-1,0);
    glm::vec4 fixed_billboard_reference_vector = glm::vec4(0,0,-1,0);

    bool flag_snow;
    bool flag_rain;

    ShaderProgram prog_lambert;// A shader program that uses lambertian reflection
    ShaderProgram prog_flat;// A shader program that uses "flat" reflection (no shadowing at all)
    ShaderProgram prog_texture; //A shader program that uses lambertian lighting/shading and texture mapping

    GLuint vao; // A handle for our vertex array object. This will store the VBOs created in our geometry classes.
                // Don't worry too much about this. Just know it is necessary in order to render geometry.

    Camera gl_camera;
    Scene scene;
    float theta = 0.0f;
    float a =0.0f;

    QPoint rotVal;
    glm::vec3 r;
    glm::vec3 u;
    bool pitch;

    float acceleration = 9.8f;
    float dT = (.2f);

    Player player;      // two cubes just stacked on top of each other
    bool POV;           // toggle between player and sandbox POV
    bool moveForward, moveBackward, moveLeft, moveRight, jump;

    // River generation
    void createRiver();
    void parseAxiom();
    void expandAxiom();
    Player turtle;          // don't draw this, just use for the position
    glm::vec3 river_pos;

    Player cow;
    glm::vec3 agent_velocity;
    void updateAgent();

////////////////////////////////////////////////////////////////////////////

    //Chunk generation
//    std::vector<std::vector<Chunk*>> ppQuad; //4 quadrants on the XZ plane
//    std::vector<std::vector<Chunk*>> pnQuad; //order is x -> z
//    std::vector<std::vector<Chunk*>> nnQuad; //y is a 256 block long thing
//    std::vector<std::vector<Chunk*>> npQuad;

    std::vector<Chunk*> ppQuad; //4 quadrants on the XZ plane
    std::vector<Chunk*> pnQuad; //order is x -> z
    std::vector<Chunk*> nnQuad; //y is a 256 block long thing
    std::vector<Chunk*> npQuad; //even though its a 2 dimensional structure
    std::vector<int> ppColumnSizes; //ppColumnSizes.size() will give you the number of columns.
    std::vector<int> pnColumnSizes; //ppColumnSizes.size() will give you the number of columns.
    std::vector<int> nnColumnSizes; //ppColumnSizes.size() will give you the number of columns.
    std::vector<int> npColumnSizes; //ppColumnSizes.size() will give you the number of columns.
    //we represent it as a contiguos 1D array, in a column major fashion,
    //storing the totallength of all columns up to that column for each column in a separate array

    glm::vec4 PlayerPos = glm::vec4(0,0,0,1);
    glm::vec4 OldPos = glm::vec4(0,0,0,1);
////////////////////////////////////////////////////////////////////////////
    QString localDir = QDir::currentPath();

    //const char* minecraft_texture_atlas = "../mini-minecraft/minecraft_textures_all/minecraft_textures_all.png";
    QString minecraft_texture_atlas = localDir + "/minecraft_textures_all/minecraft_textures_all.png";
    //const char* minecraft_normal_atlas = "../mini-minecraft/minecraft_textures_all/minecraft_normals_all.png";
    QString minecraft_normal_atlas = localDir + "/minecraft_textures_all/minecraft_normals_all.png";
    QString snow_tex = localDir + "/Other Textures/snow_alpha.png";
    QString rain_tex = localDir + "/Other Textures/rain1.png";
    //QString minecraft_texture_atlas = snow_tex;
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
    glm::mat4 RotationForWeather(glm::vec4 fromDir, glm::vec4 toDir);
    glm::mat4 GetEulerRotationMatrix(glm::vec4 quat);

    //---------------------------------------- Chunk class based functionality ---------------------------------------

    void InitialWorldSetup();
    void ExpandWorld();
    void drawWorld();

    //----------------------------------------------------------------------------------------------------------------

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
    void update_player_position_wo_collisions();
    void update_player_position_w_collisions();

protected:
    void keyPressEvent(QKeyEvent *e);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);

private slots:
    /// Slot that gets called ~60 times per second
    virtual void timerUpdate();
};
