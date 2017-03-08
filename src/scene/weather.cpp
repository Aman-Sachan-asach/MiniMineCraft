#include "weather.h"

Weather::Weather(GLWidget277 *context): Drawable(context)
{}

void Weather::createRainOrSnow(int n, glm::vec3 LookAt, int weather_phenomena, int type, glm::vec4 normal)
{
    IndexList.clear();
    VertexList.clear();

    unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
    srand (seed);

    for(int i = 0; i < n; i++) //n is the number of billboards that will be created
    {
        //generate a random number which will be the spot in the XZ plane at some
        //predetermined height from which snow or rain will fall

        float x = rand() % 200 - 100;
        float z = rand() % 200 - 100;
        float y = rand() % 200 - 35;

        //now that you have a random position in the XZ plane actually create billboards
        glm::vec4 pos = glm::vec4(x, y, z, 1.0f);
        CreateVertices(normal, pos, weather_phenomena, type);
        CreateVertexIndices(i);
    }

    //after all the billboards have been created and pushed back we need to do openGL things
    generateGeneralVBOs(IndexList.size());
    context->glBindBuffer(GL_ARRAY_BUFFER, VBO_handle);
    context->glBufferData(GL_ARRAY_BUFFER,
                          VertexList.size()*sizeof(weather_vertex_data), //size of the thing
                          VertexList.data(), //from where
                          GL_STATIC_DRAW);

    context->glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IndexVBO_handle);
    context->glBufferData(GL_ELEMENT_ARRAY_BUFFER,
                          IndexList.size()*sizeof(GLuint),
                          IndexList.data(),
                          GL_STATIC_DRAW);
}

GLenum Weather::drawMode()
{
    return GL_TRIANGLES;
}

void Weather::CreateVertices(glm::vec4 Normal, glm::vec4 Position, int weather_type, int type)
{
    //This funcction populates a vertexList that is passed into the VBO as vertex data

    //don't need an input for UVs as i'm using images not a texture atlas
    weather_vertex_data BillBoard;
    float shiny = 0.0f;
    int animate = 11;

    switch(weather_type)
    {
        case RAIN:
            shiny = shininess_RAIN;
            animate = 11;
            break;
        case SNOW:
            shiny = shininess_SNOW;
            animate = 12;
            break;
    }

    if(type==1)
    {
        //Front face
        //UR
        BillBoard.pos = glm::vec4(Position[0] + 1, Position[1] + 1, Position[2], 1.0f);
        BillBoard.normal = Normal;
        BillBoard.uv = glm::vec2(1.0f, 0.0f);
        BillBoard.flag_animate = animate;
        VertexList.push_back(BillBoard);
        //LR
        BillBoard.pos = glm::vec4(Position[0] + 1, Position[1] - 1, Position[2], 1.0f);
        BillBoard.normal = Normal;
        BillBoard.uv = glm::vec2(1.0f, 1.0f);
        BillBoard.flag_animate = animate;
        VertexList.push_back(BillBoard);
        //LL
        BillBoard.pos = glm::vec4(Position[0] - 1, Position[1] - 1, Position[2], 1.0f);
        BillBoard.normal = Normal;
        BillBoard.uv = glm::vec2(0.0f, 1.0f);
        BillBoard.flag_animate = animate;
        VertexList.push_back(BillBoard);
        //UL
        BillBoard.pos = glm::vec4(Position[0] - 1, Position[1] + 1, Position[2], 1.0f);
        BillBoard.normal = Normal;
        BillBoard.uv = glm::vec2(0.0f, 0.0f);
        BillBoard.flag_animate = animate;
        VertexList.push_back(BillBoard);
    }
    else if(type==2)
    {
        //Front face
        //UR
        BillBoard.pos = glm::vec4(Position[0], Position[1] + 1, Position[2] + 1, 1.0f);
        BillBoard.normal = Normal;
        BillBoard.uv = glm::vec2(0.0f, 0.0f);
        BillBoard.flag_animate = animate;
        VertexList.push_back(BillBoard);
        //LR
        BillBoard.pos = glm::vec4(Position[0], Position[1] + 1, Position[2] - 1, 1.0f);
        BillBoard.normal = Normal;
        BillBoard.uv = glm::vec2(1.0f, 0.0f);
        BillBoard.flag_animate = animate;
        VertexList.push_back(BillBoard);
        //LL
        BillBoard.pos = glm::vec4(Position[0], Position[1] - 1, Position[2] - 1, 1.0f);
        BillBoard.normal = Normal;
        BillBoard.uv = glm::vec2(1.0f, 1.0f);
        BillBoard.flag_animate = animate;
        VertexList.push_back(BillBoard);
        //UL
        BillBoard.pos = glm::vec4(Position[0], Position[1] - 1, Position[2] + 1, 1.0f);
        BillBoard.normal = Normal;
        BillBoard.uv = glm::vec2(0.0f, 1.0f);
        BillBoard.flag_animate = animate;
        VertexList.push_back(BillBoard);
    }
}

void Weather::CreateVertexIndices(int face_count)
{
    //populate the index used by the VBO
    IndexList.push_back(face_count*4);
    IndexList.push_back(face_count*4+1);
    IndexList.push_back(face_count*4+2);
    IndexList.push_back(face_count*4);
    IndexList.push_back(face_count*4+2);
    IndexList.push_back(face_count*4+3);
}
