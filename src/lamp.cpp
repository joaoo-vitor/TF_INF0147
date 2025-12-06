#ifndef LAMP_H
#define LAMP_H

#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#include "collisions.cpp"

class Lamp {
public:
    // Posição do poste no plano XZ
    glm::vec2 positionXZ;
    Lamp(glm::vec2 positionXZ)
    {
        this->positionXZ = positionXZ;
    }

    // Função que verifica colisão entre o poste (cilindro) e um plano
    bool checkColisionWithPlane(const std::vector<glm::vec4>& plane){
        return checkPlaneCilinderCollision(plane, positionXZ);
    }
};

#endif