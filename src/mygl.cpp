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
      geom_cube(this), 
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

      weather_layer13(this),
      weather_layer14(this),
      weather_layer15(this),
      weather_layer16(this),
      weather_layer17(this),
      weather_layer18(this),

      ppQuad(),
      pnQuad(),
      nnQuad(),
      npQuad(),

      rotVal(0, 0),
      r({0.0f, 0.0f, 0.0f}), 
      u({0.0f, 0.0f, 0.0f}), 
      pitch(false),
      player(this), POV(true),
      turtle(this), river_pos({0,0,0}),
      cow(this), agent_velocity({0,0,0}),
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
    geom_cube.destroy();
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
    geom_cube.create();

    BL.CreateClouds(10, 10);

    /*// Create player
    player.create();

    // Create turtle
    turtle.create();

    // Create cow
    cow.create();
    cow.setTarget(15, 15, 100)*/;

    // Create and set up the diffuse shader
    prog_lambert.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    // Create and set up the flat lighting shader
    prog_flat.create(":/glsl/flat.vert.glsl", ":/glsl/flat.frag.glsl");
    // Create and set up the texture mapping and Lambertian lighting/shading shader
    prog_texture.create(":/glsl/texture.vert.glsl", ":/glsl/texture.frag.glsl");

    //prog_player
    //prog_player.create(":/glsl/lambert.vert.glsl", ":/glsl/lambert.frag.glsl");
    //prog_player.setModelMatrix(glm::translate(glm::mat4(), glm::vec3(32.0f, 34.0f, 34.0f)));

    // Set a color with which to draw geometry since you won't have one
    // defined until you implement the Node classes.
    // This makes your geometry render green.
    prog_lambert.setGeometryColor(glm::vec4(0,1,0,1));

    prog_texture.setupTexture(minecraft_texture_atlas.toStdString().c_str(), w, h); //(const char* imagepath, int& width, int& height)
    prog_texture.setupNormalMap(minecraft_normal_atlas.toStdString().c_str(), w, h);
    prog_texture.setupTexture_General(snow_tex.toStdString().c_str(),
                                               weather_w, weather_h, 2, true);
    prog_texture.setupTexture_General(rain_tex.toStdString().c_str(),
                                               weather_w, weather_h, 3, true);

    // We have to have a VAO bound in OpenGL 3.2 Core. But if we're not
    // using multiple VAOs, we can just bind one once.
    //vao.bind();

//    ///------------------------------------for the Chunk Class -----------------------------------
//    InitialWorldSetup();
//    ///-------------------------------------------------------------------------------------------
    glBindVertexArray(vao);

    glm::vec2 Y_bounds = BL.CreateInitialBlockMap();

    BL.WorldBoundary[2] = std::min(Y_bounds[0], (float)BL.WorldBoundary[2]);
    BL.WorldBoundary[3] = std::max(Y_bounds[1], (float)BL.WorldBoundary[3]);

    BL.createInitialWorld();
    BL.PlayerPosition  = glm::vec3(0,1,0);
    flag_PlayerPositionUpdate = true;

    flag_rain = true;
    flag_snow = false;

//    prog_texture.setActiveTexture_General(2);
//    glm::vec4 n1 = glm::vec4(0,0,1,0);
//    glm::vec4 n2 = glm::vec4(0,0,-1,0);
//    glm::vec4 n3 = glm::vec4(1,0,0,0);
//    glm::vec4 n4 = glm::vec4(-1,0,0,0);
//    weather_layer1.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
//    weather_layer2.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
//    weather_layer3.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
////    weather_layer4.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
////    weather_layer5.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
////    weather_layer6.createRainOrSnow(10000, gl_camera.look, SNOW,  1, n1);
//    weather_layer7.createRainOrSnow(10000, gl_camera.look, SNOW , 2, n3);
//    weather_layer8.createRainOrSnow(10000, gl_camera.look, SNOW , 2, n3);
//    weather_layer9.createRainOrSnow(10000, gl_camera.look, SNOW , 2, n3);
////    weather_layer10.createRainOrSnow(10000, gl_camera.look, SNOW, 2, n3);
////    weather_layer11.createRainOrSnow(10000, gl_camera.look, SNOW, 2, n3);
////    weather_layer12.createRainOrSnow(10000, gl_camera.look, SNOW, 2, n3);

//    prog_texture.setActiveTexture_General(3);
//    weather_layer13.createRainOrSnow(10000, gl_camera.look, RAIN,  1, n1);
//    weather_layer14.createRainOrSnow(10000, gl_camera.look, RAIN,  1, n1);
//    weather_layer15.createRainOrSnow(10000, gl_camera.look, RAIN,  1, n1);
//    weather_layer16.createRainOrSnow(10000, gl_camera.look, RAIN , 2, n3);
//    weather_layer17.createRainOrSnow(10000, gl_camera.look, RAIN , 2, n3);
//    weather_layer18.createRainOrSnow(10000, gl_camera.look, RAIN , 2, n3);
}

void MyGL::resizeGL(int w, int h)
{
    //This code sets the concatenated view and perspective projection matrices used for
    //our scene's camera view.

    gl_camera = Camera(w, h, glm::vec3(10,150,20),
                       glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
    glm::mat4 viewproj = gl_camera.getViewProj();

    // set camera to player here
    gl_camera = Camera(w, h, glm::vec3(player.pos.x, player.pos.y + 1, player.pos.z), //20,5,21
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

    //prog_player.setViewProjMatrix(gl_camera.getViewProj());
    //prog_player.draw(player);

    ///---------------------------for BlockListClass --------------------------------
    UpdateWorld();
    createFullWorld();
    ///------------------------------------------------------------------------------

    ///---------------------------for Chunk Class --------------------------------
//    drawWorld();
    ///------------------------------------------------------------------------------

    ///WEATHER CREATION ----------------------------------------------------------------
/*
//    glDisable(GL_CULL_FACE);
    glEnable (GL_BLEND);
    glm::vec4 currentLookAt = glm::vec4(gl_camera.look[0], gl_camera.look[1], gl_camera.look[2], 0.0f);
    //glm::mat4 Rot_billboard = FromToRotation(fixed_billboard_reference_vector, currentLookAt);
    glm::mat4 Rot_billboard = RotationForWeather(fixed_billboard_reference_vector, currentLookAt);
    prog_texture.setModelMatrix(glm::mat4(1.0f));
    if(flag_snow)
    {
        prog_texture.setActiveTexture_General(2);
        prog_texture.DrawWeather(weather_layer1);
        prog_texture.DrawWeather(weather_layer2);
        prog_texture.DrawWeather(weather_layer3);
        prog_texture.DrawWeather(weather_layer7);
        prog_texture.DrawWeather(weather_layer8);
        prog_texture.DrawWeather(weather_layer9);

    }
    if(flag_rain)
    {
        prog_texture.setActiveTexture_General(3);
        prog_texture.DrawWeather(weather_layer13);
        prog_texture.DrawWeather(weather_layer14);
        prog_texture.DrawWeather(weather_layer15);
        prog_texture.DrawWeather(weather_layer16);
        prog_texture.DrawWeather(weather_layer17);
        prog_texture.DrawWeather(weather_layer18);
    }
    glDisable (GL_BLEND);
//    glEnable(GL_CULL_FACE);
*/
    ///WEATHER CREATION ----------------------------------------------------------------

    //Draw crosshair
    //Make CrossHair Last, so they are scene (get it? :P) over everything
    glDisable(GL_DEPTH_TEST);
    prog_flat.setViewProjMatrix(glm::mat4(1.0f));
    prog_flat.setModelMatrix(glm::mat4(1.0f));
    prog_flat.draw(crosshair);
    glEnable(GL_DEPTH_TEST);

//    POV = false;
//    if(!POV) {
//        prog_lambert.setModelMatrix(glm::translate(glm::mat4(), player.pos));
//        prog_lambert.draw(player);
//    }

//    // turtle
//    //prog_lambert.setModelMatrix(glm::translate(glm::mat4(), turtle.pos));
//    //prog_lambert.draw(turtle);

//    // cow
//    prog_lambert.setModelMatrix(glm::translate(glm::mat4(), cow.pos));
//    prog_lambert.draw(cow);

    //Keep this at the bottom, ie last thing to be drawn in paintGL
    glDisable(GL_DEPTH_TEST);
    glViewport(0,0,100,100);
    glm::vec4 currentLookAt = glm::vec4(gl_camera.look[0], gl_camera.look[1], gl_camera.look[2], 0.0f);
    glm::mat4 Rot_compass = FromToRotation(fixed_reference_vector, currentLookAt);
    prog_flat.setViewProjMatrix(glm::mat4(1.0f));
    prog_flat.setModelMatrix(Rot_compass * glm::mat4(1.0f));
    prog_flat.draw(Compass);
    glEnable(GL_DEPTH_TEST);
}

void MyGL::InitialWorldSetup()
{
    for(int i=0; i<3; i++)
    {
        for(int k=0; k<3; k++)
        {
            //push into vectors created above
            glm::vec4 center_pp = glm::vec4((i*16 + 8), 0, (k*16 + 8), 0);
            glm::vec4 center_np = glm::vec4(-(i*16 + 8), 0, (k*16 + 8), 0);
            glm::vec4 center_nn = glm::vec4(-(i*16 + 8), 0, -(k*16 + 8), 0);
            glm::vec4 center_pn = glm::vec4((i*16 + 8), 0, -(k*16 + 8), 0);

            //in ++ quadrant
            Chunk* temp_pp = new Chunk(this, center_pp);
            temp_pp->createchunk();
            ppQuad.push_back(temp_pp);

            //in -+ quadrant
            Chunk* temp_np = new Chunk(this, center_np);
            temp_np->createchunk();
            npQuad.push_back(temp_np);

            //in -- quadrant
            Chunk* temp_nn = new Chunk(this, center_nn);
            temp_nn->createchunk();
            nnQuad.push_back(temp_nn);

            //in +- quadrant
            Chunk* temp_pn = new Chunk(this, center_pn);
            temp_pn->createchunk();
            pnQuad.push_back(temp_pn);
        }

        //push into column vectors
        if(i==0)
        {
            //in ++ quadrant
            ppColumnSizes.push_back(2);
            //in -+ quadrant
            npColumnSizes.push_back(2);
            //in -- quadrant
            nnColumnSizes.push_back(2);
            //in +- quadrant
            pnColumnSizes.push_back(2);
        }
        else
        {
            //in ++ quadrant
            ppColumnSizes.push_back(ppColumnSizes[ppColumnSizes.size()-1] + 3);
            //in -+ quadrant
            npColumnSizes.push_back(npColumnSizes[npColumnSizes.size()-1] + 3);
            //in -- quadrant
            nnColumnSizes.push_back(nnColumnSizes[nnColumnSizes.size()-1] + 3);
            //in +- quadrant
            pnColumnSizes.push_back(pnColumnSizes[pnColumnSizes.size()-1] + 3);
        }

    }
}

void MyGL::ExpandWorld()
{

}

void MyGL::drawWorld()
{
    glm::mat4 model = glm::mat4(1.0f);
    prog_texture.setActiveTexture();
    prog_texture.setActiveNormalMap();
    prog_texture.setCameraEye(gl_camera.eye);
    prog_texture.setModelMatrix(model);

    //determine the chunks that need to be drawn in each Quad
    //get current position in block distance
    int drawRadius = 3;
    int index = 0;

    //++ quadrant
    if((PlayerPos[0] + 3*16) > 0 )
    {
        //in ++ quadrant
        int i=(PlayerPos[0] - 3*16);
        if (i<0)
        {
            i=0;
        }
        for(; i<(PlayerPos[0] + 3*16); i=i+16)
        {
            int x_pos = i/16;
            int temp;
            if(x_pos==0)
            {
                temp = 0;
            }
            else
            {
                temp = ppColumnSizes[x_pos-1];
            }

            int j=(PlayerPos[2] - 3*16);
            if (j<0)
            {
                j=0;
            }
            for(; j<(PlayerPos[2] + 3*16); j=j+16)
            {
                //draw chunk
                int z_pos = j/16;
                index = temp + z_pos;
                prog_texture.DrawChunk(*ppQuad[index]);
            }
        }
    }


    //-+ quadrant
    if((PlayerPos[0] - 3*16) < 0 )
    {
        //in -+ quadrant
        int i = (PlayerPos[0] - 3*16);
        for(; i<(PlayerPos[0] + 3*16); i=i+16)
        {
            if (i>=0)
            {
                break;
            }
            int x_pos = -i/16;
            int temp;
            if(x_pos<=1)
            {
                temp = 0;
            }
            else
            {
                temp = npColumnSizes[x_pos-1-1];
            }

            int j=(PlayerPos[2] - 3*16);
            if (j<0)
            {
                j=0;
            }
            for(; j<(PlayerPos[2] + 3*16); j=j+16)
            {
                //draw chunk
                int z_pos = j/16;
                index = temp + z_pos;
                prog_texture.DrawChunk(*npQuad[index]);
            }
        }
    }


    //-- quadrant
    if((PlayerPos[0] - 3*16) < 0 )
    {
        //in -- quadrant
        int i=(PlayerPos[0] - 3*16);
        for(; i<(PlayerPos[0] + 3*16); i=i+16)
        {
            int x_pos = -i/16;
            int temp;
            if(x_pos<=1)
            {
                temp = 0;
            }
            else
            {
                temp = nnColumnSizes[x_pos-1-1];
            }

            int j=(PlayerPos[2] - 3*16);
            for(; j<(PlayerPos[2] + 3*16); j=j+16)
            {
                if (j>=0)
                {
                    break;
                }
                //draw chunk
                int z_pos = -j/16;
                index = temp + z_pos;
                prog_texture.DrawChunk(*nnQuad[index]);
            }
        }
    }


    //+- quadrant
    if((PlayerPos[0] + 3*16) > 0 )
    {
        //in +- quadrant
        int i=(PlayerPos[0] - 3*16);
        if (i<0)
        {
            i=0;
        }
        for(; i<(PlayerPos[0] + 3*16); i=i+16)
        {
            int x_pos = i/16;
            int temp;
            if(x_pos==0)
            {
                temp = 0;
            }
            else
            {
                temp = pnColumnSizes[x_pos-1];
            }

            int j=(PlayerPos[2] - 3*16);
            for(; j<(PlayerPos[2] + 3*16); j=j+16)
            {
                if (j>=0)
                {
                    break;
                }
                //draw chunk
                int z_pos = -j/16;
                index = temp + z_pos;
                prog_texture.DrawChunk(*pnQuad[index]);
            }
        }
    }
}

glm::mat4 MyGL::FromToRotation(glm::vec4 fromDir, glm::vec4 toDir)
{
    glm::vec3 dir1 = glm::vec3(fromDir[0], fromDir[1], fromDir[2]);
    glm::vec3 dir2 = glm::vec3(toDir[0], toDir[1],  toDir[2]);
    dir1 = glm::normalize(dir1);
    dir2 = glm::normalize(dir2);
    glm::vec3 axis = glm::cross(dir1, dir2);
    float angle = std::acos((glm::dot(dir1, dir2))/(dir1.length() * dir2.length()));

    glm::quat q = glm::angleAxis(angle, axis);    
    glm::mat4 mat = glm::mat4_cast(q);
    //vec3 tmp1 = mat * dir1;
    //std::cout << "CHECK " << tmp1 << dir2 << std::endl;
    return mat;
}

glm::mat4 MyGL::RotationForWeather(glm::vec4 fromDir, glm::vec4 toDir)
{    
    glm::vec3 dir1 = glm::vec3(fromDir[0], fromDir[1], fromDir[2]);
    glm::vec3 dir2 = glm::vec3(toDir[0], toDir[1],  toDir[2]);
    dir1 = glm::normalize(dir1);
    dir2 = glm::normalize(dir2);
    glm::vec3 axis = glm::cross(dir1, dir2);
    float angle = std::acos((glm::dot(dir1, dir2))/(dir1.length() * dir2.length()));

    glm::quat q = glm::angleAxis(angle, axis);
    //glm::mat4 mat = GetEulerRotationMatrix(glm::vec4(q[0],q[1],q[2],q[3]));
    glm::mat4 mat = glm::mat4_cast(q);
//    glm::vec3 dir1 = glm::vec3(fromDir[0], 0, fromDir[2]);
//    glm::vec3 dir2 = glm::vec3(toDir[0], 0,  toDir[2]);
//    dir1 = glm::normalize(dir1);
//    dir2 = glm::normalize(dir2);
//    glm::vec3 cr = glm::cross(dir1, dir2);
//    float c=cr.length();
//    float d = glm::dot(dir1, dir2);
//    glm::vec4 v1 = glm::vec4(d,-c,0,0);
//    glm::vec4 v2 = glm::vec4(0,1,0,0);
//    glm::vec4 v3 = glm::vec4(c,d,0,0);
//    glm::vec4 v4 = glm::vec4(0,0,0,1);
//    glm::mat4 mat = glm::mat4(v1,v2,v3,v4);
//    //glm::mat3 tmat = glm::mat3_cast(q);
//    //glm::vec3 tmp1 = tmat * dir1;
//    std::cout << "CHECK "
//              <<" "<< tmp1[0] <<" "<< tmp1[1]<<" " << tmp1[2]<<" " <<
//                 "eewarsf"
//              <<" "<< dir2[0]<<" " <<dir2[1]<<" "<<dir2[2]<<" " <<
//                 std::endl;
    return mat;
}

glm::mat4 MyGL::GetEulerRotationMatrix(glm::vec4 quat)
{
    glm::mat4 RotationMatrix = glm::mat4();
    float s = quat[0];
    float x = quat[1];
    float y = quat[2];
    float z = quat[3];
    RotationMatrix[0][0] = 1 - 2*y*y - 2*z*z;
    RotationMatrix[0][1] = 2*x*y - 2*s*z;
    RotationMatrix[0][2] = 2*x*z + 2*s*y;

    RotationMatrix[1][0] = 2*x*y + 2*s*z;
    RotationMatrix[1][1] = 1 - 2*x*x - 2*z*z;
    RotationMatrix[1][2] = 2*y*z - 2*s*x;

    RotationMatrix[2][0] = 2*x*z - 2*s*y;
    RotationMatrix[2][1] = 2*y*z + 2*s*x;
    RotationMatrix[2][2] = 1 - 2*x*x - 2*y*y;
    return RotationMatrix;
}

void MyGL::createFullWorld()
{    
    int world_min_x = (int) (std::round(BL.WorldBoundary[0]/16.0f));
    int world_max_x = (int) (std::round(BL.WorldBoundary[1]/16.0f));
    int world_min_y = (int) (std::round(BL.WorldBoundary[2]/16.0f));
    int world_max_y = (int) (std::round(BL.WorldBoundary[3]/16.0f));
    int world_min_z = (int) (std::round(BL.WorldBoundary[4]/16.0f));
    int world_max_z = (int) (std::round(BL.WorldBoundary[5]/16.0f));

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

    //min_x = std::max(min_x, world_min_x);
    //max_x = std::min(max_x, world_max_x);
    //min_y = std::max(min_y, world_min_y);
    //max_y = std::min(max_y, world_max_y);
    //min_z = std::max(min_z, world_min_z);
    //max_z = std::min(max_z, world_max_z);

    for(int i = min_x; i <= max_x; i++) //chunks in X
    {
        for(int j = min_y; j<= max_y; j++) //chunks in Y for every X
        {
            for(int k = min_z; k<= max_z; k++) //chunks in Z for every XY
            {
                std::tuple<int, int, int> key (i, j, k);
                std::map<std::tuple<int, int, int>, int>::iterator it = BL.ChunkMap.find(key);

                if(it != BL.ChunkMap.end())
                {
                    ///draw the chunk
                    glm::mat4 model = glm::mat4(1.0f);
                    prog_texture.setActiveTexture();
                    prog_texture.setActiveNormalMap();
                    prog_texture.setCameraEye(gl_camera.eye);
                    prog_texture.setModelMatrix(model);
                    int chunkid = BL.getChunkID(key);
                    prog_texture.VBOdraw(BL, chunkid);
                }
                //else chunk doesnt exist
            }
        }
    }
}

void MyGL::UpdateWorld()
{
    if(Flag_PositionChanged)
    {
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
                    std::tuple<int, int, int> key (i, e, k);
                    std::map<std::tuple<int, int, int>, int>::iterator it = BL.ChunkMap.find(key);

                    if(it == BL.ChunkMap.end())
                    {
                        //a chunk doesn't exist at this location so create one
                        glm::vec2 y_bounds = BL.CreateMoreBlocks((i-1)*16, (i*16), ((k-1)*16), k* 16);

                        for(int j = (int)y_bounds[0]; j <= (int)y_bounds[1] + 1; j++) //chunks in Y for every X
                        {
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

    //std::cout << "block was/is in chunk position:" << chunk_pos[0] << chunk_pos[1] << chunk_pos[2] << std::endl;

    //recreate that chunk's VBO
    BL.ReCreateVBOChunk(chunk_pos[0], chunk_pos[1], chunk_pos[2]);
}

void MyGL::UpdateWorldBoundary(int x, int y, int z)
{
    BL.WorldBoundary[0] = std::min(BL.WorldBoundary[0], x);
    BL.WorldBoundary[1] = std::max(BL.WorldBoundary[1], x);
    BL.WorldBoundary[2] = std::min(BL.WorldBoundary[2], y);
    BL.WorldBoundary[3] = std::max(BL.WorldBoundary[3], y);
    BL.WorldBoundary[4] = std::min(BL.WorldBoundary[4], z);
    BL.WorldBoundary[5] = std::max(BL.WorldBoundary[5], z);
}


void MyGL::expandAxiom()
{
    for (int i = 0; i < axiom.length(); i++){
        char c = axiom.at(i);

        ////        Todo
        ///             come up with a more detailed grammar to generate rivers
        // Rules:          x -> s[-fx]+fx
        //
        // Starting axiom: fx
        // Grammar:        f    -> draw cube
        //                 +/-  -> turn +/- 15 degree
        //                 s    -> return here after completing

        if(c == 'x') {
            axiom.replace(i, 1, "s[-fx]+fx");

            i += 9;
        } else {
            continue;
        }
    }

//    std::cout << axiom << std::endl;
}

void MyGL::parseAxiom()
{
    int amount = 1.0;

    // use these to find the initial highest y value
    int start_x = player.pos.x;
    int start_y = -10000000;
    int start_z = player.pos.z;

    for(auto pos : BL.getBlockMap()) {
        int x = std::get<0>(pos.first);
        int y = std::get<1>(pos.first);
        int z = std::get<2>(pos.first);

        if(x != start_x) {
            continue;
        }

        if(z != start_z) {
            continue;
        }

        // if we made it here then find the y value
        if(y > start_y) {
            start_y = y;
//            std::cout << "start_y: " << start_y << std::endl;
        }
    }

//    std::cout << start_x << " | " << start_y << " | " << start_z << " -- " << BL.getBlockType(std::tuple<int, int, int>(start_x, start_y, start_z)) << std::endl;
    BL.setBlockType(1, std::tuple<int, int, int>(start_x, start_y, start_z));
//    std::cout << "UPDATED: " << start_x << " | " << start_y << " | " << start_z << " -- " << BL.getBlockType(std::tuple<int, int, int>(start_x, start_y, start_z)) << std::endl;

    turtle.pos = glm::vec3(start_x, start_y, start_z);
    update();

//    std::cout << start_x << " | " << start_y << " | " << start_z << std::endl;


    for(int i = 0; i < axiom.length(); i++) {
        char c = axiom.at(i);

        // todo
        // add a bounds so that it doesn't scatter

        // get all the max y values
        // traverse those
        // convert to a water block accordingly

        if(c == 'f') {
            // draw

            turtle.pos.z += amount;

            int start_x_ = turtle.pos.x;
            int start_y_ = -10000000;
            int start_z_ = turtle.pos.z;
            bool changed = false;
            for(auto pos : BL.getBlockMap()) {
                int x = std::get<0>(pos.first);
                int y = std::get<1>(pos.first);
                int z = std::get<2>(pos.first);

                if(x != start_x_) {
                    continue;
                }

                if(z != start_z_) {
                    continue;
                }

                // if we made it here then find the y value
                if(y > start_y_) {
                    start_y_ = y;
                    changed = true;
                }
            }

            if(changed) {
                BL.setBlockType(1, std::tuple<int, int, int>(start_x_, start_y_, start_z_));
            }

//            BL.AddBlock(std::tuple<int, int, int>(round(turtle.pos.x), round(turtle.pos.y), round(turtle.pos.z)), 3);

//            AddDeleteUpdation(round(turtle.pos.x), round(turtle.pos.y), round(turtle.pos.z));

            update();
        } else if(c == '-') {
            // go left

            turtle.pos.x -= amount;

            int start_x_ = turtle.pos.x;
            int start_y_ = -10000000;
            int start_z_ = turtle.pos.z;
            bool changed = false;
            for(auto pos : BL.getBlockMap()) {
                int x = std::get<0>(pos.first);
                int y = std::get<1>(pos.first);
                int z = std::get<2>(pos.first);

                if(x != start_x_) {
                    continue;
                }

                if(z != start_z_) {
                    continue;
                }

                // if we made it here then find the y value
                if(y > start_y_) {
                    start_y_ = y;
                    changed = true;
                }
            }

            if(changed) {
                BL.setBlockType(1, std::tuple<int, int, int>(start_x_, start_y_, start_z_));
            }

//            BL.AddBlock(std::tuple<int, int, int>(round(player.pos.x), round(player.pos.y), round(player.pos.z)), 3);

//            AddDeleteUpdation(round(player.pos.x), round(player.pos.y), round(player.pos.z));

            update();
        } else if(c == '+') {
            // go right

            turtle.pos.x += amount;

            int start_x_ = turtle.pos.x;
            int start_y_ = -10000000;
            int start_z_ = turtle.pos.z;
            bool changed = false;
            for(auto pos : BL.getBlockMap()) {
                int x = std::get<0>(pos.first);
                int y = std::get<1>(pos.first);
                int z = std::get<2>(pos.first);

                if(x != start_x_) {
                    continue;
                }

                if(z != start_z_) {
                    continue;
                }

                // if we made it here then find the y value
                if(y > start_y_) {
                    start_y_ = y;
                    changed = true;
                }
            }

            if(changed) {
                BL.setBlockType(1, std::tuple<int, int, int>(start_x_, start_y_, start_z_));
            }

//            BL.AddBlock(std::tuple<int, int, int>(round(player.pos.x), round(player.pos.y), round(player.pos.z)), 3);

//            AddDeleteUpdation(round(player.pos.x), round(player.pos.y), round(player.pos.z));

            update();
        } else if(c == 's') {
            // come back to this position

            river_pos = player.pos;
        } else if(c == '[') {
            // flag
            // this could be done with recursion
        } else if(c == ']') {
            // go back to s

            player.pos = river_pos;

            update();
        } else if(c == 'x') {
            // skip because x is only used for expanding the axiom

            continue;
        }
    }
}


void MyGL::createRiver()
{
    for(int i = 0; i < 5; i++) {
        expandAxiom();
    }

    parseAxiom();
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

    // xy rotations
    // don't use these
    /*else if (e->key() == Qt::Key_Right) {
        gl_camera.RotateAboutUp(-amount);
    } else if (e->key() == Qt::Key_Left) {
        gl_camera.RotateAboutUp(amount);
    } else if (e->key() == Qt::Key_Up) {
        gl_camera.RotateAboutRight(amount);
    } else if (e->key() == Qt::Key_Down) {
        gl_camera.RotateAboutRight(-amount);
    }*/

    else if (e->key() == Qt::Key_F) //decides weather or not player Position should be updated
    {
        // if POV --
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

    else if (e->key() == Qt::Key_7) //to test weather block deletion happens
    {
        std::tuple<int, int, int> block (0 ,40, 0);
        BL.DeleteBlock(block);//scene.blockslist.DeleteBlock(block);
        AddDeleteUpdation(0, 40, 0);
    }

    else if (e->key() == Qt::Key_8) //to test weather block addition happens
    {
        std::tuple<int, int, int> block (0 ,40, 0);
        BL.AddBlock(block, 2);//scene.blockslist.DeleteBlock(block);
        AddDeleteUpdation(0 ,40, 0);
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

    else if(e->key() == Qt::Key_M) {
        //createRiver();

        for(auto pos : BL.getBlockMap()) {
            std::tuple<int, int, int> block(pos.first);

            int x = std::get<0>(block);
            int y = std::get<1>(block);
            int z = std::get<2>(block);

            if(y < 10) {
                BL.setBlockType(1, block);
            } else {
                continue;
            }
        }

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

void MyGL::update_player_position_wo_collisions()
{
    if(flag_PlayerPositionUpdate)
    {
        BL.PlayerPosition = glm::vec3(gl_camera.eye[0],BL.PlayerPosition[1],gl_camera.eye[2]);
        UpdateWorldBoundary(BL.PlayerPosition[0], BL.PlayerPosition[1], BL.PlayerPosition[2]);
    }
}

void MyGL::update_player_position_w_collisions()
{
    //bouncing back shouldnt affect the world generation
    BL.PlayerPosition = glm::vec3(gl_camera.eye[0], BL.PlayerPosition[1], gl_camera.eye[2]);
}

///             Block Interaction            ///
void MyGL::mousePressEvent(QMouseEvent *e)
{
    // left click  --> destroy a block
    // right click --> add a block
/*
    if(e->button() == Qt::LeftButton) {

        // keep this so that it won't lag when you first start up
        // there's got to be a better way to do this...
        //if(player.pos.y > 10) {
        //    return;
        //}

        ///     Ray marching    ///
        // cast a ray from screen center
        // check for intersection with world blocks
        // create a ray from the camera information
//        Ray r = Ray(gl_camera);

//        // if raymarch hits a block
//        // remove a block
//        std::tuple<int, int, int> block;

//        if(r.RayMarch(BL, block)) {
//            std::cout << "remove" << std::endl;
//            BL.DeleteBlock(block);
//            AddDeleteUpdation(std::get<0>(block), std::get<1>(block), std::get<2>(block));

////            BL.AddBlock(std::tuple<int, int, int>(round(player.pos.x), round(player.pos.y), round(player.pos.z)), 3);
////            AddDeleteUpdation(round(player.pos.x), round(player.pos.y), round(player.pos.z));

//            update();
//        }
    }
    else if(e->button() == Qt::RightButton)
    {
        // place a block nearest to player
        // can hardcode type of block to place for now

        Ray r = Ray(gl_camera);

        int x = round(gl_camera.eye.x + 1);
        int y = round(gl_camera.eye.y);
        int z = round(gl_camera.eye.z + 1);

        BL.AddBlock(std::tuple<int, int, int>(x, y, z), 3);
        AddDeleteUpdation(x, y, z);

        update();

//        // screen to world point
//        glm::vec3 p = gl_camera.ref + (r.sx * r.h) + (r.sy * r.v);

//        // compute ray
//        r.origin = gl_camera.eye;
//        r.direction = glm::normalize(p - gl_camera.eye);

//        //for(float t = 0.1f; t < 16; t++) {
//            //std::cout << t << std::endl;
//            // point on ray
//            glm::vec3 ray_point = gl_camera.eye + (r.direction);

////            float x = floor(ray_point.x);
////            float y = floor(ray_point.y);
////            float z = floor(ray_point.z);

//              int x = round(ray_point.x) + 1.0f;
//              int y = round(gl_camera.eye.y);
//              int z = round(gl_camera.eye.z);

//            //std::tuple<int, int, int> block = std::make_tuple(x + 1.0f, gl_camera.eye.z, floor(gl_camera.eye.y));
//            std::tuple<int, int, int> block = std::make_tuple(x ,y, z);

//            std::cout << "Addition:" << std::get<0>(block) << " | " << std::get<1>(block) << " | " << std::get<2>(block) << std::endl;

//            BL.AddBlock(block, 2);//scene.blockslist.AddBlock(block, 0);
//            AddDeleteUpdation(std::get<0>(block), std::get<1>(block), std::get<2>(block));
    }
    */
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

    // R_rot * U_rot

    // look up/down
    if(e->y() != rotVal.y())
    {
        float phi = e->y() - rotVal.y();
        rotVal.setY(e->y());

        // phi = (-90, 90)
///             TODO           ///
        // need to fix
        // it resets the entire camera

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
        /*float */theta = e->x() - rotVal.x();

        rotVal.setX(e->x());

        //std::cout << -dx << std::endl;

        // theta = [0, 360)
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

//    std::cout << "acceleration: " << acceleration << std::endl;
//    std::cout << "dT: " << dT << std::endl;
//    std::cout << "velocity: " << velocity << std::endl;
//    std::cout << "position: " << position << std::endl;

//    // player position is already centered in the block
//    // although the y value needs to be updated
//    // actually i think that the y value is the grounded position
//    std::cout << "Player position: " << player.pos.x << " | " << player.pos.y << " | " << player.pos.z << std::endl;
//    std::cout << floor(player.pos.x) << " | " << floor(player.pos.y) << " | " << floor(player.pos.z) << std::endl;
//    std::cout << ceil(player.pos.x) << " | " << ceil(player.pos.y) << " | " << ceil(player.pos.z) << std::endl;

    // shouldn't really take 0,0,0 into account in the final implmenation

    glm::vec3 f_grounded = glm::vec3(gl_camera.look.x, 0.0f, gl_camera.look.z);
    glm::normalize(f_grounded);

    if(!isGrounded()) {
//        player.pos.y -= position;
        playerMoved = true;
        gl_camera.TranslateAlongUp(-position);
        //std::cout << "Free fall" << std::endl;
    }

    if(moveForward) {

        // player moves
        if(flag_PlayerPositionUpdate) {

            // only need to check for collisions if the player is moving

            if(checkCollision()) {
//                player.pos.z += position;

                // prevents flying
                gl_camera.look = f_grounded;

                gl_camera.TranslateAlongLook(-position);
                //std::cout << "move" << std::endl;
            } else {
//                player.pos.z -= position;
                gl_camera.TranslateAlongLook(position);
                //std::cout << "move" << std::endl;
            }

            playerMoved = true;
        }
        // camera moves
        else {
            gl_camera.TranslateAlongLook(amount);
        }

        //std::cout << "Move Forward" << std::endl;
    }

    if(moveBackward){

        // player moves
        if(flag_PlayerPositionUpdate) {

            // only need to check for collisions if the player is moving

            if(checkCollision()) {
//                player.pos.z -= position;
                gl_camera.TranslateAlongLook(position);
            } else {
//                player.pos.z += position;
                gl_camera.TranslateAlongLook(-position);
            }

            playerMoved = true;
        }
        // camera moves
        else {
            gl_camera.TranslateAlongLook(-amount);
        }

        //std::cout << "Move Backward" << std::endl;
    }

    if(moveLeft) {

        // player moves
        if(flag_PlayerPositionUpdate) {

            // only need to check for collisions if the player is moving

            if(checkCollision()) {
//                player.pos.x -= position;
                gl_camera.TranslateAlongRight(position);
            } else {
//                player.pos.x += position;
                gl_camera.TranslateAlongRight(-position);
            }

            playerMoved = true;
        }
        // camera moves
        else {
            gl_camera.TranslateAlongRight(-amount);
        }

        //std::cout << "Move Left" << std::endl;
    }

    if(moveRight){

        // player moves
        if(flag_PlayerPositionUpdate) {

            // only need to check for collisions if the player is moving

            if(checkCollision()) {
//                player.pos.x += position;
                gl_camera.TranslateAlongRight(-position);
            } else {
//                player.pos.x -= position;
                gl_camera.TranslateAlongRight(position);
            }

            playerMoved = true;
        }
        // camera moves
        else {
            gl_camera.TranslateAlongRight(amount);
        }

        //std::cout << "Move Right" << std::endl;
    }

    if(jump) {
        std::cout << "Jump" << std::endl;
    }


    // only adjust back to the player when pov is on
    if(playerMoved) {
        //gl_camera.eye = glm::vec3(player.pos.x, player.pos.y + 1, player.pos.z);
        player.pos = gl_camera.eye;
    }

    // update player for world to be drawn each time
    //BL.PlayerPosition = player.pos;
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

    // fix this

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

//                std::cout << "player: " << player.pos.y << std::endl;
//                std::cout << "block: " << block_y << std::endl;

                //player.pos.y = player.pos.y - diff;

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
        else {
    //            std::cout << "Player pos: " << player.pos.x << " | " << round(player.pos.y) << " | " << player.pos.z << std::endl;
    //            std::cout << "Block pos: " << block_x << " | " << block_y << " | " << block_z << std::endl;
        }

        int block_min_x = block_x - 1;
        int block_min_y = block_y - 1;
        int block_min_z = block_z - 1;

        int block_max_x = block_x;
        int block_max_y = block_y;
        int block_max_z = block_z;

    //        std::cout << "Block min: " << block_min_x << " | " << block_min_y << " | " << block_min_z << std::endl;
    //        std::cout << "Block max: " << block_max_x << " | " << block_max_y << " | " << block_max_z << std::endl;

    //        float block_max_x = round(block_x);
    //        float block_max_y = round(block_y);
    //        float block_max_z = round(block_z);

    //        float block_min_x = block_max_x - 1;
    //        float block_min_y = block_max_y - 1;
    //        float block_min_z = block_max_z - 1;

        // box-box aabb testing
        if(player_min_x < block_max_x &&
           block_min_x < player_max_x &&
           player_min_y < block_max_y &&
           block_min_y < player_max_y &&
           player_min_z < block_max_z &&
           block_min_z < player_max_z)
        {
    //            std::cout << "Intersecting with block" << std::endl;
    //            std::cout << block_min_x << " | " << block_min_y << " | " << block_min_z << std::endl;
    //            std::cout << block_max_x << " | " << block_max_y << " | " << block_max_z << std::endl;

            return true;
            //break;
        }
    }

    return false;
}

void roundPosition(glm::vec3 &v) {
    v.x = round(v.x);
    v.y = round(v.y);
    v.z = round(v.z);
}

void MyGL::updateAgent()
{



    //Behaviors b;
    //b.Arrive(cow);









//    Behaviors b;
//    b.Arrive(cow);

//    cow.printPosition();

//    // update velocity
//    cow.velocity += cow.acceleration;

//    // limit speed
//    //limit(cow.velocity, cow.max_speed);
//    float lenlen = pow(cow.pos.x, 2) + pow(cow.pos.y, 2) + pow(cow.pos.z, 2);

//    if(lenlen > 0){
//        if(lenlen > pow(cow.max_speed, 2)) {
//            float ratio = cow.max_speed / sqrt(lenlen);

//            cow.pos.x *= ratio;
//            cow.pos.y *= ratio;
//            cow.pos.z *= ratio;
//        }
//    }


//    // update position
//    cow.pos += cow.velocity;

//    // reset acceleration
//    cow.acceleration *= 0;


//    float velocity = acceleration * dT;
//    float position = velocity * dT;
//    float maxVelocity = acceleration * dT;


//    // if seeking player
//    // make sure there's a buffer between them
//    // but that's a tougher case because of the ground collision
//    // this won't work until isGrounded() is fixed

//    glm::vec3 target = glm::vec3(15, 15, 100);

//    // Seek Test
//    //glm::vec3 steer = cow.behavior.Seek(target, cow.pos, agent_velocity);
//    //agent_velocity += steer;
//    //cow.pos += agent_velocity;

//    // Arrive Test
//    glm::vec3 steer = cow.behavior.Arrive(target, cow.pos, agent_velocity);
//    agent_velocity += steer;
//    cow.pos += agent_velocity;

//    std::cout << cow.pos.z << std::endl;



    /// dont delete
    // don't use round
    // it'll cause it to oscillate
//    int x = ceil(cow.pos.x);
//    int y = ceil(cow.pos.y);
//    int z = ceil(cow.pos.z);

//    if(glm::vec3(x, y, z) == target) {
//        return;
//    }
    /// end dont delete


//    glm::vec3 desired_velocity = cow.behavior.Seek(glm::vec3(50, 15, 15), cow.pos);
//    desired_velocity *= position;      // velocity
//    glm::vec3 steer = desired_velocity - agent_velocity;

    //glm::vec3 steer = cow.behavior.Seek(player.pos, cow.pos);
    //steer = steer - agent_velocity;

    // limit steer to maxforce (position)
//    std::cout << "Steer: " << steer.length() << std::endl;


//    if(cow.pos.x < 50) {
//        cow.pos.x += position;
//    }

    //agent_velocity += position;
//    cow.pos += steer;
}

float MyGL::timeOfDay(int _time)
{
    //Make _time map to increments of 0.1 for every 10 units
    //_time <= 9, return 0
    //_time = time / 10, return _time / 10.0f;

    _time = _time / 10;
    return _time / 10.0f;
}

/// TODO
void MyGL::timerUpdate()
{
    updatePlayer();
    resetTranslations();
    timeCount++;
    if(timeCount %5 == 0)
    {
        timeStep = (timeStep + timeStepDelta);
        if(timeStep == 0 || timeStep == 100)
        {
            timeStepDelta *= -1;
        }
        prog_texture.setTimeCount(timeStep);
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
        float blend_LI = LI_low * (1-alpha) + LI_high * (alpha);
        prog_texture.setLIntensity(blend_LI); // set between 0 and 1 as it is just a factor
     }


    if(timeCount %2 == 0)
    {
        weatherHeight++;// = weatherHeight - 0.3f;
        int wh = weatherHeight % 30;
        //int wh = weatherHeight;
        //weatherHeight =  weatherHeight % 50;
        prog_texture.setHeight(wh);
    }

    current_rotation_degree += 15.0f;
    glm::mat4 r = glm::rotate(glm::mat4(1.0f),
                              current_rotation_degree * (M_PI / (180.0f)),
                              glm::vec3(1.0f, 0, 0));

    glm::vec4 light_dir = CenterPoint + r * CirclePointZ;
    prog_texture.setLDirection(light_dir);//glm::vec4(1,1,1,0));

    update();
}
