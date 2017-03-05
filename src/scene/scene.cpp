#include <scene/scene.h>

#include <scene/cube.h>
#include <iostream>
#define M_PI 3.1415


Scene::Scene() : dimensions(64, 64, 64)//, mygl(nullptr)//blockslist(nullptr)
{}

void Scene::CreateTestScene()
{
    for(int x = 0; x < dimensions.x; x++)
    {
        QList<QList<bool>> Ys;
        for(int y = 0; y < dimensions.y; y++)
        {
            QList<bool> Zs;
            for(int z = 0; z < dimensions.z; z++)
            {
                float perlin_z = std::abs(perlinNoise2D(x, z));
                if(y <= perlin_z)//== dimensions.y/2)//
                {
                    //prog_tex.useMe();
                    //glActiveTexture((GL_TEXTURE0);
                    Zs.push_back(true);

                }
                else
                {
                    Zs.push_back(false);
                }
            }
            Ys.push_back(Zs);
        }


        objects.push_back(Ys);
    }
    objects[0][dimensions.y/2+2][0] = true;
    objects[dimensions.x-1][dimensions.y/2+2][0] = true;
    objects[0][dimensions.y/2+2][dimensions.z-1] = true;
    objects[dimensions.x-1][dimensions.y/2+2][dimensions.z-1] = true;
}





//---------------------------------------- PERLIN NOISE TERRAIN GENERATION ---------------------------------------
/*
 * Using a continuous 2D Perlin noise function, create terrain blocks using a height field.

 * When a new chunk of terrain is generated (when the player moves near the bounds of the currently generated world,
            for some definition of "near") its contents should blend seamlessly with the chunks of terrain
            generated at the current world boundary.

 * Note that the newly generated terrain need not blend nicely with terrain that the player has modified themselves.

 * The world that is generated when the program first runs should be 64x64x64 blocks at minimum.

 * You may choose to make it larger if you think your machine can handle it.

 * You should also modify the way your terrain is stored. Rather than storing your world in a 3D array,
            you should store it in a map of position -> block type.

 * We recommend representing the position as a std::tuple since they can be easily hashed.

 * This is a sparse way of storing your world, so that you don't waste memory storing empty (air) blocks.

 * Note that we specify the value of the map to be a block type as the next milestone of the project
            incorporates different block types such as dirt, grass, and lava.

 * We recommend using an enum as the map value.

*/

/*
//void Scene::createPerlinNoiseScene(BlockList* blocklist)
//{
//    //iterate through the map and create tuple position values with:
//    //x until dimensions.x
//    //y until dimensions.y
//    //z with the value generated from Perlin Noise equation
//    //for now instantiate z value to be 0.0f

//    float x_offset = 0.0f;
//    float y_offset = 0.0f;
//    //blockslist = new BlockList();

//    for(int x = 0; x < dimensions.x; x++)
//    {
//        for(int y = 0; y < dimensions.y; y++)
//        {
//            //for(int z = 0; z < dimensions.z; z++)
//            //{
//                float perlin_z = std::abs(perlinNoise2D(x_offset, y_offset));


//                //FILL THE BLOCKLIST CLASS MAP
//                //std::map<std::tuple<float, float, float>, int> block_map = blockslist->getBlockmap();

//                //insert the x, y, and z values into the tuple part of the block_map
//                //we'll just make block_type be all grass for now (aka index 0)

//                //block_map.insert(std::pair<std::tuple<float, float, float>, int> (std::tuple<float, float, float>(x, y, perlin_z), 0));
//                for (int i = 0; i < perlin_z; ++i) {
//                    //blockslist.AddBlock(std::tuple<float, float, float>(x, y, i), 0);
//                    //blockslist->AddBlock(std::tuple<float, float, float>(x, y, i), 0);
//                    //mygl->BL.AddBlock(std::tuple<float, float, float>(x, y, i), 0);
//                    blocklist->AddBlock(std::tuple<float, float, float>(x, y, i), 0);
//                }
//            //}
//            y_offset += .25;
//        }
//        x_offset += .25;
//    }
//}
*/

float randomNoise(float x, float y);    //this should return a value from 0 - 1
float interpolatedNoise(float x, float y);
float smoothedNoise(float x, float y);
float cosineInterp(float x, float y, float z);


float Scene::perlinNoise2D(float x, float y)//, int divide_freq_by, int height_factor_at_end)
{
    float z = 0.0f;
    double persistence = .5; //making persistence bigger makes it more spikey
    int numOctaves = 2;
    float frequency = 0;
    float amplitude = 0;

    x = x * .3;
    y = y * .3;

    for (int i = 0; i < numOctaves; i++)
    {
        frequency = std::pow(2, i);        //increasing the frequency coefficient will create more white pixels
        amplitude = std::pow(persistence, i) * 2.0f; //don't multiply persistence here

        //z = z + randomNoise(x * frequency, y * frequency) * amplitude;

        z = z + interpolatedNoise(x * frequency, y * frequency) * amplitude;    //divide_freq_by here
    }

    //std::cout << "Z Value:" << z-1 << std::endl;

    float result = z - 1;
    if (result > 1.0f)
    {
        result = 1.0f;
    }

    return result;   //multiply by height_factor_at_end here
}

//FYI returns -1 to 1
//add 1 and divide by 2 to normalize
float randomNoise(float x, float y)
{
    int int_x = (int)(x + y * 57);
    int_x = (int_x<<13) ^ int_x;
    return ((1.0 - ( (int_x * (int_x * int_x * 15731 + 789221) + 1376312589) & 0x7fffffff) / 1073741824.0f) + 1) / 2.0f;
}

float interpolatedNoise(float x, float y)
{
    int int_x = int(x);
    float difference_x = x - int_x;

    int int_y = int(y);
    float difference_y = y - int_y;

    float v1 = smoothedNoise(int_x, int_y);
    float v2 = smoothedNoise(int_x + 1, int_y);
    float v3 = smoothedNoise(int_x, int_y + 1);
    float v4 = smoothedNoise(int_x + 1, int_y + 1);

    float interp_1 = cosineInterp(v1, v2, difference_x);
    float interp_2 = cosineInterp(v3, v4, difference_x);

    return cosineInterp(interp_1, interp_2, difference_y);
}


float smoothedNoise(float x, float y)
{
    float corners = (randomNoise(x - 1, y - 1) + randomNoise(x + 1, y - 1) + randomNoise(x - 1, y + 1) + randomNoise(x + 1, y + 1)) / 16.0f;
    float sides = (randomNoise(x - 1, y) + randomNoise(x + 1, y) + randomNoise(x , y - 1) + randomNoise(x , y + 1)) / 8.0f;
    float center = randomNoise(x , y) / 4.0f;

    return corners + sides + center;
}


float cosineInterp(float x, float y, float z)
{
    float t = (1 - std::cos(z * M_PI)) * 0.5;
    return (x * (1 - t)) + (y * t);
}








std::vector<glm::vec3> Scene::makeSphere(glm::vec3 center, float radius)
{
    std::vector<glm::vec3> sphere_pts;

    for(double phi = 0.0; phi < 2 * M_PI; phi += M_PI/(2.0f * radius))
    {
        for(double theta = 0.0; theta < M_PI; theta += M_PI/(2.0f * radius))
        {
            glm::vec3 point = glm::vec3(0.0f);

            point.x = radius * std::cos(phi) * std::sin(theta) + center.x;
            point.y = radius * std::sin(phi) * std::sin(theta) + center.y;
            point.z = radius * std::cos(theta) + center.z;

            sphere_pts.push_back(point);
        }
    }
    return sphere_pts;
}//end makeSphere

std::vector<glm::vec3> Scene::makeEllipse(glm::vec3 center, float thickness, float width, float height)
{
    std::vector<glm::vec3> ellipse_pts;

    for(int z = - height; z <= height; z++)
    {
        for(int y = -thickness; y <= thickness; y++)
        {
            for(int x = -width; x <= width; x++)
            {
                double dx = (double)x / (double)width;
                double dy = (double)y / (double)thickness;
                double dz = (double)z / (double)height;

                if(dx*dx + dy*dy + dz*dz <= 1.0f)
                {
                    glm::vec3 point = glm::vec3(0.0f);
                    point = glm::vec3(center.x + x, center.y + y, center.z + z);
                    ellipse_pts.push_back(point);
                }//end if
            }//end for x
        }//end for y
    }//end for z

    return ellipse_pts;
}//end makeEllipse


//---------------------------------------- PERLIN WORM CAVE GENERATION ---------------------------------------
void Scene::perlinWorm()
{
    //map output of noise [0, 1] to direction [0, 360]
    //every step along the cave, turn in a direction determined by noise at current position
}
