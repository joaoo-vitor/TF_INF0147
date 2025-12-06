#ifndef COLLISIONS
#define COLLISIONS

#include <glm/vec4.hpp>
#include "DistanciaBezier.cpp"

const glm::vec3 FINISH_LINE_CENTER(0.0f, 1.5f, 1280.0f); // Plane point 'c'

bool checkCarCrossFinishLine(const glm::vec4& carPosition, const glm::vec4& carForwardVector){
    // reta que passa pelo carro             v--- plano de chegada
    //  ______                             |
    // ( ---> )                            |
    //  o    o                             |
    glm::vec3 a = glm::vec3(carPosition)- 1.5f * glm::vec3(carForwardVector);
    glm::vec3 b = glm::vec3(carPosition)+ 1.5f * glm::vec3(carForwardVector);

    glm::vec3 n = glm::vec3(0.0f, 0.0f, -1.0f);// normal do plano de chegada
    glm::vec3 c = FINISH_LINE_CENTER;

    // caso for paralelo
    if(!dot((b-a), n)) return 0;
    // se nao for, ve qual o valor de t
    // substituição da eq da reta na eq do plano
    float t = dot((c-a), n)/dot((b-a), n);
    if(t>=0.0 && t<=1.0) return true;
    else return false;
}

bool checkColisionCylinderPlane(const std::vector<glm::vec4>& plane, glm::vec2 cylinder){
    const float radius = 1.0f;

    for (const glm::vec4& v : plane)
    {
        // Projetamos o ponto no XZ
        glm::vec2 pontoXZ = glm::vec2(v.x, v.z);

        float dist = glm::distance(pontoXZ, cylinder);
        if (dist <= radius)return true;
    }
    return false;
}

bool collideSphereBezier(glm::vec4 P1, glm::vec4 V1, glm::vec4 P2, glm::vec4 V2, glm::vec4 Center, double radius){

    return distanciaBezierCubica(P1, V1, P2, V2, Center) <= radius;

}

#endif
