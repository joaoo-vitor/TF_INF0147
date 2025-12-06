#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <cmath>
#ifndef POINT_H
#define POINT_H
#define Y_START 0.5f 
#define M_PI   3.14159265358979323846
#define Y_ROTATION_SPEED 2*M_PI/3 //demora 3 segundos pra dar uma volta inteira
#define HEIGTH_ANIMATION_SPEED 2*M_PI/1// demora 1 segundos para ir pra cima e pra baixo 1x
class Point{
private:
    float angle_height=0;
public:
    bool active=true; // Se o usuário já pegou, fica desativa
    glm::vec4 position;
    const float radius=1.5;
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    float height = Y_START;
    Point(glm::vec2 p){
        position=glm::vec4(p.x, Y_START, p.y, 1.0f);
    }
    glm::mat4 getMatrixRotate(){
        return Matrix_Rotate_Z(rotation.z) * Matrix_Rotate_Y(rotation.y) * Matrix_Rotate_X(rotation.x);
    }
    // Atualiza animação do ponto
    void updateAnimation(float elapsedTime){
        // Gira o ponto
        rotation.y+=Y_ROTATION_SPEED*elapsedTime;
        if(rotation.y>=2*M_PI){
            rotation.y = rotation.y-(2.0f*M_PI);
        }
        // Leva pra cima e pra baixo
        angle_height += elapsedTime*HEIGTH_ANIMATION_SPEED;
        height= Y_START+ 0.3f*sin(angle_height);
    }
};

#endif