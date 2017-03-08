#include "mygl.h"
#include <la.h>
#include <unistd.h>

#include <QLabel>
#include <iostream>
#include <QApplication>
#include <QKeyEvent>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/quaternion.hpp>

std::string axiom = "fx";

MyGL::MyGL(QWidget *parent)
    : GLWidget277(parent),
      BL(this),
      prog_lambert(this), 
      prog_flat(this),
      prog_texture(this),
      gl_camera(),
      crosshair(this),
      Compass(this),

      weather_layer1(this),
      weather_layer2(this),
      weather_layer3(this),
      weather_layer4(this),
      weather_layer5(this),
      weather_layer6(this),
      weather_layer7(this),
      weather_layer8(this),
      weather_layer9(this),
      weather_layer10(this),
      weather_layer11(this),
      weather_layer12(this),

      rotVal(0, 0),
      r({0.0f, 0.0f, 0.0f}), 
      u({0.0f, 0.0f, 0.0f}), 
      pitch(false),
      player(this), POV(true),
      moveForward(false), moveBackward(false), moveLeft(false), moveRight(false), jump(false)
{
    // Connect the timer to a function so that when the timer ticks the function is executed
    connect(&timer, SIGNAL(timeout()), this, SLOT(timerUpdate()));
    // Tell the timer to redraw 60 times per second
    timer.start(16);
    setFocusPolicy(Qt::ClickFocus);
}

MyGL::~MyGL()
{
    makeCurrent();
    glDeleteVertexArrays(1, &vao);
}

void MyGL::initializeGL()
{
    // Create an OpenGL context using Qt's QOpenGLFunctions_3_2_Core class
    // If you were programming in a non-Qt context you might use GLEW (GL Extension Wrangler)instead
    initializeOpenGLFunctions();
    // Print out some information about the current OpenGL context
    debugContextVersion();

    // Set a few settings/modes in OpenGL rendering
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LINE_SMOOTH);
    glEnable(GL_POLYGON_SMOOTH);
    glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
    glHint(GL_POLYGON_SMOOTH_HINT, GL_NICEST);

    //BackFace Culling enabled for better efficiency
    glFrontFace(GL_CW);
    glCullFace(GL_BACK);
    glEnable(GL_CULL_FACE);

    //////////////BLENDING FOR TRANSPARENCY////////////////////////////////////

    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDisable(GL_BLEND);

    ///////////Re enable it in paint GL for only the Transparency/////////////////

    // Set the size with which points should be rendered
    glPointSize(5);
    // Set the color with which the screen is filled at the start of each render call.
    glClearColor(0.37f, 0.74f, 1.0f, 1); //That light blue Background
    printGLErrorLog();

    // Create a Vertex Attribute Object
    glGenVertexArrays(1, &vao);

    // Create crosshair
    crosshair.create();
    Compass.create();

    //Volumetric clouds
    BL.CreateClouds(10, 10);

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    // Create and set up the texture mapping and Lambertian lighting/shading shader
    prog_texture.create(":/glsl/texture.vert.glsl", ":/glsl/texture.frag.glsl");

    //Set up texture and normal mapping
    prog_texture.setupTexture(minecraft_texture_atlas.toStdString().c_str(), w, h);
    prog_texture.setupNormalMap(minecraft_normal_atlas.toStdString().c_str(), w, h);
    prog_texture.setupTexture_General(snow_tex.toStdString().c_str(), weather_w, weather_h, 2, true);
    prog_texture.setupTexture_General(rain_tex.toStdString().c_str(), weather_w, weather_h, 3, true);

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    glBindVertexArray(vao);

    // Set the boundary of the world in terms of the Y axis
    glm::vec2 Y_bounds = BL.CreateInitialBlockMap();
    BL.WorldBoundary[2] = std::min(Y_bounds[0], (float)BL.WorldBoundary[2]);
    BL.WorldBoundary[3] = std::max(Y_bounds[1], (float)BL.WorldBoundary[3]);

    //Sets up the initial terrain that the player is in
    BL.createInitialWorld();
    BL.PlayerPosition  = glm::vec3(0,1,0);
    flag_PlayerPositionUpdate = true;

    flag_rain = true;
    flag_snow = false;

    prog_texture.setActiveTexture_General(2);
    glm::vec4 n1 = glm::vec4(0,0,1,0);
    glm::vec4 n3 = glm::vec4(1,0,0,0);
    //Create Layers of Billboards for snow
    weather_layer1.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
    weather_layer2.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
    weather_layer3.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
    weather_layer4.createRainOrSnow(10000, gl_camera.look, SNOW , 2, n3);
    weather_layer5.createRainOrSnow(10000, gl_camera.look, SNOW , 2, n3);
    weather_layer6.createRainOrSnow(10000, gl_camera.look, SNOW , 2, n3);

    //Create Layers of Billboards for rain
    prog_texture.setActiveTexture_General(3);
    weather_layer7.createRainOrSnow(10000, gl_camera.look, RAIN,  1, n1);
    weather_layer8.createRainOrSnow(10000, gl_camera.look, RAIN,  1, n1);
    weather_layer9.createRainOrSnow(10000, gl_camera.look, RAIN,  1, n1);
    weather_layer10.createRainOrSnow(10000, gl_camera.look, RAIN , 2, n3);
    weather_layer11.createRainOrSnow(10000, gl_camera.look, RAIN , 2, n3);
    weather_layer12.createRainOrSnow(10000, gl_camera.look, RAIN , 2, n3);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.

    gl_camera = Camera(w, h, glm::vec3(10,150,20),
                       glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 viewproj = gl_camera.getViewProj();

    // set camera to player here
    gl_camera = Camera(w, h, glm::vec3(player.pos.x, player.pos.y + 1, player.pos.z),
                       glm::vec3(20, 6, 10), glm::vec3(0,1,0));
    //ref should be facing negative z
    viewproj = gl_camera.getViewProj();

    // Upload the view-projection matrix to our shaders (i.e. onto the graphics card)
    prog_lambert.setViewProjMatrix(viewproj);
    prog_flat.setViewProjMatrix(viewproj);
    prog_texture.setViewProjMatrix(viewproj);

    printGLErrorLog();
}

//This function is called by Qt any time your GL window is supposed to update
//For example, when the function updateGL is called, paintGL is called implicitly.
void MyGL::paintGL()
{
    // Clear the screen so that we only see newly drawn images
    glClearColor(BLEND_BACKGROUND[0], BLEND_BACKGROUND[1], BLEND_BACKGROUND[2], 1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    prog_flat.setViewProjMatrix(gl_camera.getViewProj());
    prog_lambert.setViewProjMatrix(gl_camera.getViewProj());
    prog_texture.setViewProjMatrix(gl_camera.getViewProj());

    ///---------------------------for BlockListClass --------------------------------
    UpdateWorld();
    createFullWorld();
    ///------------------------------------------------------------------------------

    ///-------------------------- WEATHER CREATION --------------------------------------
    glDisable(GL_CULL_FACE);
    glEnable (GL_BLEND);
    prog_texture.setModelMatrix(glm::mat4(1.0f));
    if(flag_snow)
    {
        prog_texture.setActiveTexture_General(2);
        prog_texture.DrawWeather(weather_layer1);
        prog_texture.DrawWeather(weather_layer2);
        prog_texture.DrawWeather(weather_layer3);
        prog_texture.DrawWeather(weather_layer4);
        prog_texture.DrawWeather(weather_layer5);
        prog_texture.DrawWeather(weather_layer6);

    }
    if(flag_rain)
    {
        prog_texture.setActiveTexture_General(3);
        prog_texture.DrawWeather(weather_layer7);
        prog_texture.DrawWeather(weather_layer8);
        prog_texture.DrawWeather(weather_layer9);
        prog_texture.DrawWeather(weather_layer10);
        prog_texture.DrawWeather(weather_layer11);
        prog_texture.DrawWeather(weather_layer12);
    }
    glDisable (GL_BLEND);
    glEnable(GL_CULL_FACE);
    ///-------------------------- WEATHER CREATION -----------------------------------------

    //Draw crosshair
    //Make CrossHair Last, so they are scene (get it? :P) over everything
    glDisable(GL_DEPTH_TEST);
    prog_flat.setViewProjMatrix(glm::mat4(1.0f));
    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(crosshair);
    glEnable(GL_DEPTH_TEST);

    //Draw a Tiny Compass
    //Keep this at the bottom, ie last thing to be drawn in paintGL
    glDisable(GL_DEPTH_TEST);
    glViewport(0,0,100,100); //resize the viewport being drawn to to only
                            //have a tiny compass instead of over the entire screen
    //The code below gets the right rotation matrix to orient the compass appropriately
    glm::vec4 currentLookAt = glm::vec4(gl_camera.look[0], gl_camera.look[1], gl_camera.look[2], 0.0f);
    glm::mat4 Rot_compass = FromToRotation(fixed_reference_vector, currentLookAt);
    prog_flat.setViewProjMatrix(glm::mat4(1.0f));
    prog_flat.setModelMatrix(Rot_compass * glm::mat4(1.0f));
    prog_flat.draw(Compass);
    glEnable(GL_DEPTH_TEST);
}

glm::mat4 MyGL::FromToRotation(glm::vec4 fromDir, glm::vec4 toDir)
{
    //Take 2 vectors, determine the quaternion needed to go from the first vector to the second
    glm::vec3 dir1 = glm::vec3(fromDir[0], fromDir[1], fromDir[2]);
    glm::vec3 dir2 = glm::vec3(toDir[0], toDir[1],  toDir[2]);
    dir1 = glm::normalize(dir1);
    dir2 = glm::normalize(dir2);

    //creating quaternion
    glm::vec3 axis = glm::cross(dir1, dir2);
    float angle = std::acos((glm::dot(dir1, dir2))/(dir1.length() * dir2.length()));
    glm::quat q = glm::angleAxis(angle, axis);

    //convert back to matrix
    glm::mat4 mat = glm::mat4_cast(q);
    return mat;
}

void MyGL::createFullWorld()
{
    //only draw chunks within a certain radius around the
    //player to avoid rendering the entire world
    int radius_around_player = 4.0f;

    int playerChunk_x = (int)(BL.PlayerPosition[0]/16.0f);
    int playerChunk_y = (int)(BL.PlayerPosition[1]/16.0f);
    int playerChunk_z = (int)(BL.PlayerPosition[2]/16.0f);

    int min_x = playerChunk_x - radius_around_player;
    int max_x = playerChunk_x + radius_around_player;
    int min_y = playerChunk_y - radius_around_player;
    int max_y = playerChunk_y + radius_around_player;
    int min_z = playerChunk_z - radius_around_player;
    int max_z = playerChunk_z + radius_around_player;

    for(int i = min_x; i <= max_x; i++) //chunks in X
    {
        for(int j = min_y; j<= max_y; j++) //chunks in Y for every X
        {
            for(int k = min_z; k<= max_z; k++) //chunks in Z for every XY
            {
                //find chunk in the hash map of world_position and chunk_id
                std::tuple<int, int, int> key (i, j, k);
                std::map<std::tuple<int, int, int>, int>::iterator it = BL.ChunkMap.find(key);

                if(it != BL.ChunkMap.end())
                {
                    ///draw the chunk
                    glm::mat4 model = glm::mat4(1.0f);
                    //using multiple texture slots,so we have to keep
                    //telling OpenGL which the active texture is
                    prog_texture.setActiveTexture();
                    prog_texture.setActiveNormalMap();
                    prog_texture.setCameraEye(gl_camera.eye);
                    prog_texture.setModelMatrix(model);

                    int chunkid = BL.getChunkID(key);
                    prog_texture.VBOdraw(BL, chunkid); //draw call
                }
                //else chunk doesnt exist so don't draw it
            }
        }
    }
}

void MyGL::UpdateWorld()
{
    //The flag only checks if the player has moved
    if(Flag_PositionChanged)
    {
        //Once you know the player has actually moved,
        //check if the player has moved half a chunk away from
        //the last time the chunk VBO's were updated
        int a = (int)round(BL.OldPlayerPosition[0]/16.0f);
        int b = (int)round(BL.OldPlayerPosition[1]/16.0f);
        int c = (int)round(BL.OldPlayerPosition[2]/16.0f);
        int d = (int)round(BL.PlayerPosition[0]/16.0f);
        int e = (int)round(BL.PlayerPosition[1]/16.0f);
        int f = (int)round(BL.PlayerPosition[2]/16.0f);

        glm::vec3 OldPos = glm::vec3(a, b, c);
        glm::vec3 NewPos = glm::vec3(d, e, f);

        //if old pos and new pos are different then we have crossed the
        //half way point of a chunk and may need to create new chunks
        if(OldPos != NewPos)
        {
            int k = 0;
            int i = 0;

            for(i=((NewPos[0]-2)); i<=(NewPos[0]+2) ;i++) //chunks in X
            {
                for(int k=(NewPos[2]-2); k<=(NewPos[2]+2) ;k++) //chunks in Z for every XY
                {
                    //search for chunk in hash map based on world space position
                    std::tuple<int, int, int> key (i, e, k);
                    std::map<std::tuple<int, int, int>, int>::iterator it = BL.ChunkMap.find(key);

                    if(it == BL.ChunkMap.end())
                    {
                        //a chunk doesn't exist at this location so create one
                        glm::vec2 y_bounds = BL.CreateMoreBlocks((i-1)*16, (i*16), ((k-1)*16), k* 16);

                        for(int j = (int)y_bounds[0]; j <= (int)y_bounds[1] + 1; j++) //chunks in Y for every X
                        {
                            //only create chunks within these particular Y axis boundaries for the world
                            BL.CreateChunkVBOs(i, j, k);
                        }
                    }
                }
            }
        }
    }
    Flag_PositionChanged = false;
    BL.OldPlayerPosition = BL.PlayerPosition;
}

void MyGL::AddDeleteUpdation(int x, int y, int z)
{
    ///Call this function after Deleting or Adding a block
    UpdateWorldBoundary(x,y,z);

    //determine which chunk it's in and change that chunks VBO
    glm::vec3 chunk_pos ((int)round(x/16.0f), (int)round(y/16.0f), (int)round(z/16.0f));
    //recreate that chunk's VBO
    BL.ReCreateVBOChunk(chunk_pos[0], chunk_pos[1], chunk_pos[2]);
}

void MyGL::UpdateWorldBoundary(int x, int y, int z)
{
    //recomputes the world boundary for draw calls
    BL.WorldBoundary[0] = std::min(BL.WorldBoundary[0], x);
    BL.WorldBoundary[1] = std::max(BL.WorldBoundary[1], x);
    BL.WorldBoundary[2] = std::min(BL.WorldBoundary[2], y);
    BL.WorldBoundary[3] = std::max(BL.WorldBoundary[3], y);
    BL.WorldBoundary[4] = std::min(BL.WorldBoundary[4], z);
    BL.WorldBoundary[5] = std::max(BL.WorldBoundary[5], z);
}

void MyGL::keyPressEvent(QKeyEvent *e)
{
    ///         FPS walking         ///
    // remove y from forward vector (gl_camera.look)
    // normalize and reset
    // prevents character from flying when looking up
    // and prevents them from going into the plane when looking down
    //      might have to move this to their own condition
    //      but it should be ok


    int amount = 2;
    if(e->modifiers() & Qt::ShiftModifier){
        amount = 10.0f;
    }
    // http://doc.qt.io/qt-5/qt.html#Key-enum

    if (e->key() == Qt::Key_Escape) {
        QApplication::quit();
    }

    else if (e->key() == Qt::Key_F) //decides weather or not player Position should be updated
    {
        // if in POV mode or in flying mode
        if(POV) {
            POV = false;
        } else {
            POV = true;
        }

        if(flag_PlayerPositionUpdate)
        {
           flag_PlayerPositionUpdate = false;
        }
        else
        {
           flag_PlayerPositionUpdate = true;

           // reset camera to player pov
           gl_camera.eye = glm::vec3(player.pos.x, player.pos.y + 1, player.pos.z + 1);

           gl_camera.RecomputeAttributes();

           update();
        }
    }

    // fovy
    else if (e->key() == Qt::Key_1) {
        gl_camera.fovy += amount;
    } else if (e->key() == Qt::Key_2) {
        gl_camera.fovy -= amount;
    }

    else if (e->key() == Qt::Key_3)
    {
        flag_snow = true;
        flag_rain = false;
    }
    else if (e->key() == Qt::Key_4)
    {
        flag_snow = false;
        flag_rain = true;
    }

    // WASD translation conditions
    //
    //              -----
    //             | --------> eye
    //             -----
    //              -----
    //             |    |
    //             -----
    //

    // When keys are pressed a flag is set for updatePlayer()
    // updatePlayer() checks for collisions and if the player is grounded
    // Positions are updated with velocity and acceleration
    else if (e->key() == Qt::Key_W)
    {
        moveForward = true;
    }
    else if (e->key() == Qt::Key_S)
    {
        moveBackward = true;
    }
    else if (e->key() == Qt::Key_D)
    {
        moveRight = true;
    }
    else if (e->key() == Qt::Key_A)
    {
        moveLeft = true;
    }

    // jump/duck
    else if (e->key() == Qt::Key_Q)
    {

        if(POV) {
            gl_camera.TranslateAlongUp(-amount);
            player.pos = gl_camera.eye;
        }

        if(!Flag_PositionChanged) {
            gl_camera.TranslateAlongUp(-amount);
        }
    }
    else if (e->key() == Qt::Key_E)
    {
        if(POV) {
            gl_camera.TranslateAlongUp(amount);
            player.pos = gl_camera.eye;
        }

        if(!Flag_PositionChanged) {
            gl_camera.TranslateAlongUp(amount);
        }
    }

    // reframe
    else if (e->key() == Qt::Key_R)
    {
        gl_camera = Camera(this->width(), this->height());
    }

    gl_camera.RecomputeAttributes();
    update();  // Calls paintGL, among other things
}

void MyGL::mouseMoveEvent(QMouseEvent *e)
{
    // this allows to get the mouse position without click/drag
    setMouseTracking(true);

    // first time user is moving
    if(rotVal == QPoint(0, 0))
    {
        rotVal = QPoint(e->x(), e->y());
        return;
    }

    // look up/down
    if(e->y() != rotVal.y())
    {
        float phi = e->y() - rotVal.y();
        rotVal.setY(e->y());

        if(phi > 90)
        {
            phi = 89;

            if(pitch)
            {
                return;
            }
            else
            {
                pitch = true;
            }
        }
        else if(phi < -90)
        {
            phi = -89;

            if(pitch)
            {
                return;
            }
            else
            {
                pitch = true;
            }
        }

        gl_camera.RotateAboutRight(-phi * .50);
    }

    // look left/right
    if(e->x() != rotVal.x())
    {
        theta = e->x() - rotVal.x();

        rotVal.setX(e->x());
        if(theta >= 360)
        {
            theta -= 360;
        }

        gl_camera.RotateAboutUp(-theta * .50);
    }

    // locks mouse to center
    QPoint glob = mapToGlobal(QPoint(gl_camera.width / 2, gl_camera.height /2));
    QCursor::setPos(glob);
    rotVal = QPoint(gl_camera.width / 2, gl_camera.height / 2);
}

void MyGL::orientCamera()
{
    gl_camera.RecomputeAttributes();
    update();
}

/// Gets called from timerUpdate()
/// Checks for collision and if the player is grounded
/// Position is calculated based on acceleration, velocity, and dT
void MyGL::updatePlayer()
{
    float velocity = acceleration * dT;
    float position = velocity * dT;

    float amount = 2.0f;

    bool playerMoved = false;

    glm::vec3 f_grounded = glm::vec3(gl_camera.look.x, 0.0f, gl_camera.look.z);
    glm::normalize(f_grounded);

    if(!isGrounded()) {
        playerMoved = true;
        gl_camera.TranslateAlongUp(-position);
    }

    if(moveForward) {

        // player moves
        if(flag_PlayerPositionUpdate) {

            // only need to check for collisions if the player is moving

            if(checkCollision()) {

                // prevents flying
                gl_camera.look = f_grounded;

                gl_camera.TranslateAlongLook(-position);
            } else {
                gl_camera.TranslateAlongLook(position);
            }

            playerMoved = true;
        }
        // camera moves
        else {
            gl_camera.TranslateAlongLook(amount);
        }
    }

    if(moveBackward){

        // player moves
        if(flag_PlayerPositionUpdate)
        {
            // only need to check for collisions if the player is moving

            if(checkCollision()) {
                gl_camera.TranslateAlongLook(position);
            } else {
                gl_camera.TranslateAlongLook(-position);
            }

            playerMoved = true;
        }
        // camera moves
        else {
            gl_camera.TranslateAlongLook(-amount);
        }
    }

    if(moveLeft) {

        // player moves
        if(flag_PlayerPositionUpdate)
        {
            // only need to check for collisions if the player is moving

            if(checkCollision()) {
                gl_camera.TranslateAlongRight(position);
            } else {
                gl_camera.TranslateAlongRight(-position);
            }

            playerMoved = true;
        }
        // camera moves
        else {
            gl_camera.TranslateAlongRight(-amount);
        }
    }

    if(moveRight){

        // player moves
        if(flag_PlayerPositionUpdate)
        {
            // only need to check for collisions if the player is moving

            if(checkCollision()) {
                gl_camera.TranslateAlongRight(-position);
            } else {
                gl_camera.TranslateAlongRight(position);
            }

            playerMoved = true;
        }
        // camera moves
        else {
            gl_camera.TranslateAlongRight(amount);
        }
    }

    // only adjust back to the player when pov is on
    if(playerMoved) {
        player.pos = gl_camera.eye;
    }

    // update player for world to be drawn each time
    BL.PlayerPosition = glm::vec3(gl_camera.eye[0] ,0 ,gl_camera.eye[2]) ;
    Flag_PositionChanged = true;

    gl_camera.RecomputeAttributes();
    orientCamera();
}

bool MyGL::isGrounded()
{
    //
    //      -------
    //     | head |
    //     |      |
    //     --------
    //      -------
    //     | torso|
    //     |      |
    //     --------
    //      -------  -------  ------*  ---->  block position
    //     |      | |      | |      |
    //     |      G R O U N D       |
    //     -------- -------  -------


    // player information
    int player_min_x = round(player.pos.x - 1);
    int player_min_y = round(player.pos.y - 1);
    int player_min_z = round(player.pos.z - 1);

    int player_max_x = round(player.pos.x);
    int player_max_y = round(player_min_y);
    int player_max_z = round(player.pos.z);

    std::map<std::tuple<int, int, int>, int>::iterator it;
    std::tuple<int, int, int> block = std::make_tuple(1, 32, 1);
    it = BL.getBlockMap().find(block);

    for(auto pos : BL.getBlockMap()) {
        // block maxima
        int block_x = std::get<0>(pos.first);
        int block_y = std::get<1>(pos.first);
        int block_z = std::get<2>(pos.first);


        // block minima
        int block_min_x = block_x - 1;
        int block_min_y = block_y - 1;
        int block_min_z = block_z - 1;

        // if player is above highest block
        // find next block
        if(player_min_y > block_y) {
            continue;
        }

        // need to fix this
        if(player_min_x < block_x &&
           block_min_x < player_max_x &&
           player_min_z < block_z &&
           block_min_z < player_max_z)
        {
            // intersected x-z
            // check for y

            if(player_min_y == block_y) {
                int type = pos.second;

                // if water height = 0
                if(type == 1) {
                    return false;
                }

                float diff = player.pos.y - block_y;
                return true;
            }

        } else {
            continue;
        }
    }

    return false;
}

void MyGL::resetTranslations()
{
    moveForward  = false;
    moveBackward = false;
    moveLeft     = false;
    moveRight    = false;
    jump         = false;
}

bool MyGL::checkCollision()
{
    ///         Player Information          ///

    //          ---------  max(x, y, z)                 ---------
    //         |  head  |                              |        |
    //         |        |  eye                         |        |
    //         |        |                              |        |
    //          ---------                               ---------
    //          ---------  player.pos
    //         |        |
    //         |        |
    //         |  torso |
    //          ---------

    // assuming floor(y) gets me the ground
    // definitely need to do some extensive checking with this
    // it seems to work for now

    int player_min_x = round(player.pos.x) - 1;
    int player_min_y = round(player.pos.y) - 2;
    int player_min_z = round(player.pos.z) - 1;

    int player_max_x = round(player.pos.x);
    int player_max_y = player_min_y + 1;
    int player_max_z = round(player.pos.z);

    for(auto pos : BL.getBlockMap()) {
        float block_x = std::get<0>(pos.first);
        float block_y = std::get<1>(pos.first);
        float block_z = std::get<2>(pos.first);

        // skip the blocks that aren't within the y value
        if(block_y != round(player.pos.y)) {
            continue;
        }

        int block_min_x = block_x - 1;
        int block_min_y = block_y - 1;
        int block_min_z = block_z - 1;

        int block_max_x = block_x;
        int block_max_y = block_y;
        int block_max_z = block_z;

        // box-box aabb testing
        if(player_min_x < block_max_x &&
           block_min_x < player_max_x &&
           player_min_y < block_max_y &&
           block_min_y < player_max_y &&
           player_min_z < block_max_z &&
           block_min_z < player_max_z)
        {
            return true;
        }
    }

    return false;
}

void roundPosition(glm::vec3 &v) {
    v.x = round(v.x);
    v.y = round(v.y);
    v.z = round(v.z);
}

float MyGL::timeOfDay(int _time)
{
    //Make _time map to increments of 0.1 for every 10 units
    _time = _time / 10;
    return _time / 10.0f;
}

void MyGL::timerUpdate()
{
    updatePlayer();
    resetTranslations();
    timeCount++;
    if(timeCount %5 == 0)
    {
        //every so many counts do the following:
        //set the time variable used in the shader and for variables that are updated periodically
        timeStep = (timeStep + timeStepDelta);
        if(timeStep == 0 || timeStep == 100)
        {
            timeStepDelta *= -1;
        }
        prog_texture.setTimeCount(timeStep);
        //set the alpha value used for blending the background color for the day and night cycle
        alpha = timeOfDay(timeStep);
//-------------------------------------------------------------------------------
        BLEND_BACKGROUND = (glm::normalize(LIGHTBLUE) * (1-alpha) +
                 glm::normalize(DARKBLUE) * (alpha)) ;
        BLEND_BACKGROUND = glm::normalize(BLEND_BACKGROUND);
//-------------------------------------------------------------------------------
        BLEND_LIGHTCOLOR = (glm::normalize(WHITE) * (1-alpha) +
                            glm::normalize(BLACK) * (alpha)) ;
        BLEND_LIGHTCOLOR = glm::normalize(BLEND_LIGHTCOLOR);
        BLEND_LIGHTCOLOR[3] = 1.0f;
        prog_texture.setLColor(glm::normalize(BLEND_LIGHTCOLOR));
//-------------------------------------------------------------------------------
        //use the timestep value to control light intensity
        float blend_LI = LI_low * (1-alpha) + LI_high * (alpha);
        prog_texture.setLIntensity(blend_LI); // set between 0 and 1 as it is just a factor
     }


    if(timeCount %2 == 0)
    {
        //move clouds
        weatherHeight++;
        int wh = weatherHeight % 30;
        prog_texture.setHeight(wh);
    }

    //move light source (simply a light direction) by rotating about a point
    current_rotation_degree += 15.0f;
    glm::mat4 r = glm::rotate(glm::mat4(1.0f),
                              current_rotation_degree * (M_PI / (180.0f)),
                              glm::vec3(1.0f, 0, 0));

    glm::vec4 light_dir = CenterPoint + r * CirclePointZ;
    prog_texture.setLDirection(light_dir);

    update();
}
