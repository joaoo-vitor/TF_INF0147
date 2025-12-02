#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#ifndef LAMP_H
#define LAMP_H
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
        const float radius = 1.0f;

        for (const glm::vec4& v : plane)
        {
            // Projetamos o ponto no XZ
            glm::vec2 pontoXZ = glm::vec2(v.x, v.z);

            float dist = glm::distance(pontoXZ, positionXZ);
            if (dist <= radius)return true;
        }
        return false;
    }
};

#endif