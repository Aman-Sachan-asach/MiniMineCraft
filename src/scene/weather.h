#pragma once
#include "drawable.h"
#include "la.h"
#include <iostream>
#include <chrono>
#include <random>
#include <time.h>
#include <stdlib.h>

struct weather_vertex_data
{
    glm::vec4 pos;
    glm::vec4 normal;
    glm::vec2 uv;
    int flag_animate;
};

enum WeatherType{ RAIN = 1, SNOW = 2};

class Weather: public Drawable
{
public:
    int radius = 5; //radius in chunks around the player where the snow or rain should fall
    float height = 31.0f;

    //VertexList and IndexList are used to populate the VBO and determine the index used by the VBO
    std::vector<weather_vertex_data> VertexList;
    std::vector<GLuint> IndexList;

    //shininesss is used to determine specularity in the shader
    float shininess_amplification = 20.0f;
    float shininess_SNOW =   1.65f * shininess_amplification;
    float shininess_RAIN =   1.55f * shininess_amplification;

public:
    Weather(GLWidget277 *context);

    void create() {} //virtual function necessitates this

    void createRainOrSnow(int n, glm::vec3 LookAt, int weather_phenomena, int type, glm::vec4 normal);
    virtual GLenum drawMode();

    void CreateVertices(glm::vec4 Normal, glm::vec4 Position, int weather_type, int type);
    void CreateVertexIndices(int face_count);
};
