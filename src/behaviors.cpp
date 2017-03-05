#include "behaviors.h"
#include <algorithm>



/// --- THIS WILL HAPPEN
// returns steering force to be added to the agent's acceleration

// Agent will go back and forth between the target position
glm::vec3 Behaviors::Seek(glm::vec3 target, glm::vec3 pos, glm::vec3 agent_velocity) const
{
    glm::vec3 desired_velocity = glm::normalize(target - pos) * maxVelocity;     // desired vector

    glm::vec3 steer = desired_velocity - agent_velocity;

    return steer;
}

float truncate(float d, float b, float c) {
    return fmax(fmin(d, c), b);
}

float map(float value, float old_min, float old_max, float new_min, float new_max)
{
    float old_range = old_max - old_min;
    float new_range = new_max - new_min;
    float new_val   = (value - old_min) * new_range;
    new_val /= old_range;
    new_val += new_min;

    return new_val;
}

void setMag(glm::vec3 &v, float new_len)
{
    float old_len = v.length();

    v.x = (v.x * new_len) / old_len;
    v.y = (v.y * new_len) / old_len;
    v.z = (v.z * new_len) / old_len;
}

void limit(glm::vec3 &v, float max) {
    float len_sqrd = pow(v.x, 2) + pow(v.y, 2) + pow(v.z, 2);

    if(len_sqrd > pow(max, 2) && len_sqrd > 0) {
        float ratio = max / sqrt(len_sqrd);

        v.x *= ratio;
        v.y *= ratio;
        v.z *= ratio;
    }
}

glm::vec3 truncate(glm::vec3 v, float maxSpeed)
{
    float i = maxSpeed / v.length();

    i = i < 1.0 ? i : 1.0;  // if less than 1 return i, otherwise return 1
    v *= i;

    return v;
}

void Behaviors::Arrive(Player &agent)
{
    glm::vec3 desired = agent.target - agent.pos;
    float dist = sqrt(pow(desired.x, 2) + pow(desired.y, 2) + pow(desired.z, 2));

    //std::cout << "dist: " << dist << std::endl;
    if(dist < slowing_radius) {
        desired = glm::normalize(desired) * agent.max_speed * (dist / slowing_radius);
    } else {
        desired = glm::normalize(desired) * agent.max_speed;
    }

    glm::vec3 steer = desired - agent.velocity;
    agent.velocity = truncate(agent.velocity + steer, agent.max_speed);
    agent.pos = agent.pos + agent.velocity;



//    glm::vec3 desired = agent.target - agent.pos;

//    float d = desired.length();

//    if(d < 50) {
//        float m = map(d, 0, 50, 0, agent.max_speed);

//        setMag(desired, m);
//    } else {
//        setMag(desired, agent.max_speed);
//    }

//    glm::vec3 steer = desired - agent.velocity;

//    limit(steer, agent.max_force);

//    // apply force
//    agent.acceleration += steer;
}

// Agent will come to a slow stop at the target
//glm::vec3 Behaviors::Arrive(glm::vec3 target, glm::vec3 pos, glm::vec3 agent_velocity)
//{
//    glm::vec3 desired = target - pos;

//    float d = sqrt(pow(desired.x, 2) + pow(desired.y, 2) + pow(desired.z, 2));

//    if(d < 50) {
//        float m = map(d, 0, 50, 0, max_speed);

//        setMag(desired, m);
//    } else {
//        setMag(desired, max_speed);
//    }

//    glm::vec3 steer = desired - velocity;


////    glm::vec3 desired_velocity = target - pos;
////    //dist = desired_velocity.length();

////    float dist = sqrt(pow(desired_velocity.x, 2) + pow(desired_velocity.y, 2) + pow(desired_velocity.z, 2));

////    std::cout << "dist: " << dist << std::endl;

////    if(dist < 50) {
////        // within the slowing radius

////        //std::cout << " In Slowing Radius: " << std::endl;
////        //std::cout << "Pos: " << pos.x << " | " << pos.y << "| " << pos.z << std::endl;

////        desired_velocity =  glm::normalize(desired_velocity) *
////                            maxVelocity *
////                            (dist / slowing_radius);

////    } else {
////        // outside the slowing radius

////        std::cout << "MAX" << std::endl;

////        desired_velocity=   glm::normalize(desired_velocity) *
////                            maxVelocity;
////    }

////    glm::vec3 steer = desired_velocity - agent_velocity;

////    return steer;
//}

void Behaviors::Wander()
{

}
