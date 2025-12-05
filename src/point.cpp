#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#ifndef POINT_H
#define POINT_H
#define Y_OFFSET 0.5f 

class Point{
public:
    glm::vec2 position;
    const float radius=1.5;
    Point(glm::vec2 p){
        position=p;
    }
    // Retorna true sse a distancia do centro da esfera até o plano for menor ou igual ao raio da esfera
    bool checkColisionWithPlane(glm::vec4 p, glm::vec4 n){
        // Pegamos a distancia do centro da esfera até o ponto do plano
        glm::vec4 norma = normalize(n);

        // Projetamos essa distancia no vetor normal do plano
        glm::vec4 delta = p-glm::vec4(position.x, Y_OFFSET, position.y, 1.0f);
        float dist = dot(delta, n);
        return dist<=radius;
    }
};

#endif