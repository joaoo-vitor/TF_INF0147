
#ifndef COLL_H
#define COLL_H
#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <matrices.h>
#include <array>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <limits>
#include "lamp.cpp"

const glm::vec3 FINISH_LINE_CENTER(0.0f, 1.5f, 1280.0f); // Ponto central da linha de chegada



bool checkPlaneLineCollision(const glm::vec4& carPosition, const glm::vec4& carForwardVector){
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
    // substituição da equação da reta na eq do plano
    float t = dot((c-a), n)/dot((b-a), n);
    if(t>=0.0 && t<=1.0) return true;
    else return false;
}

bool checkPlaneCilinderCollision(const std::vector<glm::vec4>& plane, glm::vec2 positionXZ){
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

bool checkAABBSphereCollision(glm::vec4 position, float BB_MIN_X, float BB_MIN_Y, float BB_MIN_Z, float BB_MAX_X,
                                float BB_MAX_Y, float BB_MAX_Z, glm::vec4 sphere_c, float radius){
    glm::vec3 p_closest; // Ponto mais perto do AABB até a esfera
        
    p_closest.x = std::max(position.x+BB_MIN_X, std::min(sphere_c.x, position.x+BB_MAX_X));
    p_closest.y = std::max(position.y+BB_MIN_Y, std::min(sphere_c.y, position.y+BB_MAX_Y));
    p_closest.z = std::max(position.z+BB_MIN_Z, std::min(sphere_c.z, position.z+BB_MAX_Z));

    
    // Distância do centro da esfera até o ponto mais próximo
    glm::vec3 diff = glm::vec3(sphere_c) - p_closest;
    float distSqr = glm::dot(diff, diff);

    return distSqr<= radius*radius;
}


#endif