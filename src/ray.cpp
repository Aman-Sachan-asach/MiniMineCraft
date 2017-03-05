#include "ray.h"

Ray::Ray()
{}

Ray::Ray(Camera c) :
    //len((c.ref - c.eye).length()),
    len(sqrt(pow(c.ref.x - c.eye.x, 2) + pow(c.ref.y - c.eye.y, 2) + pow(c.ref.z - c.eye.z, 2))),
    alpha(c.fovy /2 ),
    v(c.up * (float)len * (float)tan(alpha)),
    h(c.right * (float)len * (float)c.aspect * (float)tan(alpha)),
    sx(0),
    sy(0),
    p(c.ref + (sx * h) + (sy * v)),
    origin(c.eye),
    direction(c.look)
{}

Ray::Ray(glm::vec3 eye, glm::vec3 ref)
{
    origin = eye;
    direction = glm::normalize(ref - eye);
}

Ray::~Ray()
{}

bool Ray::RayMarch(BlockList &blocklist, std::tuple<int, int, int> &block)
{
     //          Block                         Player
     //          -------                       -------
     //         |       | <--------- Ray ---- |       |
     //         |       |                     |       |
     //          -------                       -------
     //                                        -------
     //                                       |       |
     //                                       |       |
     //                                        -------

    for(float t = 0.1f; t < 32; t += 0.1f) {
        glm::vec3 ray_point = this->origin + ((float)t * this->direction);

        float x = round(ray_point.x);
        float y = round(ray_point.y);
        float z = round(ray_point.z);

        // traverse blocklist to find block
        for(auto pos : blocklist.getBlockMap()) {
            if(std::get<0>(pos.first) != x) {
                continue;
            }

            if(round(std::get<1>(pos.first)) != y) {
                continue;
            }

            if(std::get<2>(pos.first) != z) {
                continue;
            }

//            std::cout << "Comparing" << std::endl;
//            std::cout << "Ray: " << x << " | " << y << " | " << z << std::endl;
//            std::cout << "Block: " << std::get<0>(pos.first) << " | " << round(std::get<1>(pos.first)) << " | " << std::get<2>(pos.first) << std::endl;
//            std::cout << "--------" << std::endl;

            // made it here then a block was found
            // set the block to the positions in the map
            // return to mygl to remove
            block = std::make_tuple(std::get<0>(pos.first), std::get<1>(pos.first), std::get<2>(pos.first));

            return true;
        }

    }

    return false;
}

bool Ray::RayMarchAdd(BlockList &blocklist, std::tuple<int, int, int> &block)
{
    // point on ray
    glm::vec3 ray_point = this->origin + this->direction;

    float x = floor(ray_point.x);
    float y = floor(ray_point.y);
    float z = floor(ray_point.z);

    float _x = ceil(ray_point.x);
    float _y = ceil(ray_point.y);
    float _z = ceil(ray_point.z);

    float x_pos;
    float y_pos;
    float z_pos;

    for(auto pos : blocklist.getBlockMap()) {

        if(x == std::get<0>(pos.first)) {
            if(y == floor(std::get<2>(pos.first))) {
                if(z = std::get<1>(pos.first)) {

                    // position of the block to be removed
                    block = std::make_tuple(std::get<0>(pos.first),
                                            std::get<1>(pos.first),
                                            std::get<2>(pos.first));

                    // see if it exists in the blocklist
                    if(blocklist.getBlockMap().find(block) == blocklist.getBlockMap().end()) {

                        // remove the block from the world
                        //blocklist.removeBlock(block);

                        //removed = true;

                        return true;
                    }
                    else {
                        std::cout << "put a block here" << std::endl;

                        return false;
                    }
                }
            }
        }

        x = ceil(ray_point.x);
        y = ceil(ray_point.y);
        z = ceil(ray_point.z);
    }

    //std::cout << "adding this block" << std::endl;
}



/// delete this function
bool Ray::Collides(BlockList &blocklist)
{
    float x = this->origin.x;//std::get<0>(pos.first);
    float y = this->origin.y;//std::get<1>(pos.first);
    float z = this->origin.z;//std::get<2>(pos.first);

    for(auto p : blocklist.getBlockMap()) {
        // not all too responsive
        // will fix
        if(x == std::get<0>(p.first)) {
            if(y == floor(std::get<2>(p.first))) {
                if(z == std::get<1>(p.first)) {

                    // remove block
                    //std::map<std::tuple<int, int, int>, int> map = scene.blockslist.getBlockmap();
                    std::map<std::tuple<int, int, int>, int>::iterator it;
                    std::tuple<int, int, int> block = std::make_tuple(std::get<0>(p.first), std::get<1>(p.first), std::get<2>(p.first));

                    it = blocklist.getBlockMap().find(block);

                    // see if it exists in the blocklist
                    if(blocklist.getBlockMap().find(block) != blocklist.getBlockMap().end()) {
                        std::cout << "collision" << std::endl;
                        // translate = false;
                    }
                }
            }
        }
    }
}
