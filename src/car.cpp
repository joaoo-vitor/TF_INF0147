#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <matrices.h>
#include <array>
#include <algorithm>
#include <numeric>
#include <iostream>

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

#define MAX_VEL 0.05f // 3e-3 unidades por frame
#define ACCELERATION 0.0005f
#define ACCELERATION_REVERSE 0.0001f
#define BRAKE_ACCELERATION 0.0005f

#define VELOCITY_DECAY_RATIO 0.01f
#define SIDE_VELOCITY_DECAY_RATIO 0.025f
#define TURNING_DECAY_RATIO 0.1f

#define ZERO_VELOCITY_THRESHOLD 0.00006f
#define ZERO_TURNANGLE_THRESHOLD 0.0066f
#define ZERO_TURNVEL_THRESHOLD 0.0006f

#define CAMERA_INITIAL_HEIGHT 8.0f

class Car
{
private:
    glm::vec4 position;
    glm::vec3 rotation;
    glm::vec4 velocity;
    glm::vec4 forwardsVector; // Vetor que aponta para frente do carro
    bool brake;
    bool accelerate;
    bool slide;
    bool reverse;
    float turnAngle;

    
public:
    // Carro começa no centro (0,0,0)
    // Velocidade zero
    // É rotacionado em X inicialmente apenas para que seu modelo fique certo (horizontal)
    Car(){
        position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        // rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        rotation = glm::vec3(-M_PI_2, 0.0f, 0.0f);
        velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        forwardsVector = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        brake = false;
        accelerate = false;
        slide = false;
        turnAngle = 0.0f;
        reverse = false;
    };

    // Valor de theta e phi calculados para esse modelo:
    //          ___3___.(camera)
    //         |      /          
    //         |     /       
    //         |    /     
    //         6   /    
    //         |  /
    //         | /
    //     .-> ./
    //     |           |
    //    (0,3,0)      |
    //    (no inicio)  3
    //         .       |
    //   (car position)    
    //      <--------
    glm::vec3 calculateCameraViewVector(){
        glm::vec3 cameraLookAt = glm::vec3(0.0f,3.0f,0.0f);
        glm::vec3 cameraPos = glm::vec3(0.0f,CAMERA_INITIAL_HEIGHT,-7.0f);
        return cameraLookAt - cameraPos;
    }
    float getCameraTheta(){
        glm::vec3 v = normalize(calculateCameraViewVector());

        // When retrieving the theta, consider the car rotation as well
        return std::atan2(v.x, v.y)+rotation.y;
    }
    float getCameraPhi(){
        glm::vec3 v = normalize(calculateCameraViewVector());
        return std::acos(v.z);
    }
    glm::mat4 getTranslationMatrix(){
        return Matrix_Translate(position.x, position.y, position.z);
    }
    glm::mat4 getMatrixRotate(){
        return Matrix_Rotate_Z(rotation.z) * Matrix_Rotate_Y(rotation.y) * Matrix_Rotate_X(rotation.x);
    }
    glm::vec4 getForwardsVector(){
    	return forwardsVector;
    }
    
    void setVelocity(float vel){
    }
    void setAccelerate(bool value){
        accelerate = value;
    }
    void setBrake(bool value){
        brake = value;
    }
    void setReverse(bool value){
        reverse = value;
    }
    glm::vec4 getVelocity(){
        return velocity;
    }
    glm::vec4 getPosition(){
        return position;
    }
    void turnRight(){
        turnAngle -= 0.001f;
    }
    void turnLeft(){
        turnAngle += 0.001f;
    }

    void updatePosition(){
    	position = position + velocity;
    }
    void updateRotation(bool slide){
        if(slide)
            rotation.y += turnAngle * norm(velocity);
        else
            //Se tiver derrapando, 
            rotation.y += turnAngle * 3.0f * norm(velocity);
        turnAngle *= 1-TURNING_DECAY_RATIO; // Decaimento do ângulo de curva

        // Threshhold para zerar curva
        if(std::fabs(turnAngle) < 0.0008f) turnAngle = 0.0f;
    }
    void updateVelocity(){
        // Calculate sideVel 
        // sideVel é o vetor que aponta para a lateral do carro (esquerda ou direita)
        slide = false; // Slide por default é false, é settado nos ifs abaixo
        glm::vec4 side = normalize(crossproduct(forwardsVector, glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)));
        glm::vec4 sideVel = side * dotproduct(velocity, side); // sideVel é o vetor paralelo ao fowards com tamanho proporcional ao 

        if (brake)
        {
            slide = true;

            float speed = norm(velocity);
            float sideSpeed = norm(sideVel);

            if (speed < ZERO_VELOCITY_THRESHOLD)
            {
                velocity = glm::vec4(0.0f);
                brake=false;
            }
            else
            {
                // Portion of velocity that is straight (not drifting)
                float forwardPercent = speed > 0.0f
                    ? speed / (speed + sideSpeed + 0.0001f)
                    : 0.0f;

                // Exponential braking multiplier:
                // The slower the car, the bigger this value becomes.
                const float k = 2.5f; // feel free to tune 1.5–4.0
                float brakeBoost = expf(k * (1.0f - speed / MAX_VEL));

                // Final braking vector
                glm::vec4 decel =
                    velocity *
                    BRAKE_ACCELERATION *
                    (0.5f + 0.5f * forwardPercent) *
                    brakeBoost;

                velocity -= decel;

                if (norm(velocity) < ZERO_VELOCITY_THRESHOLD)
                    velocity = glm::vec4(0.0f);
            }
        }
        else
        {   
            // Se estiver dando ré com velocidade não-nula, entende como uma freagem
            float acc = ACCELERATION;
            if(reverse){
                if(norm(velocity)>0){
                    brake=true;
                }else{
                    brake=false;
                    acc = -ACCELERATION_REVERSE;
                }
            }

            // Se estiver com um vetor de curva minimo
            if (norm(sideVel) > ZERO_TURNVEL_THRESHOLD)
            {
                slide = true;
                // Antes de recalcular a velocidade, tira o componente lateral
                velocity-=sideVel; 
                sideVel *= (1-SIDE_VELOCITY_DECAY_RATIO);
                if (accelerate){
                    velocity += forwardsVector * acc * // aceleração básica
                        (0.2f + powf(norm(velocity), 0.3f)) *  // Simula inércia (inicialmente é difícil acelerar, aumenta
                        (0.5f + 0.5f *    // Se não esitver virando, acelera mais rapido
                            (norm(velocity)!=0.0f ? // % de velocidade reta
                            (norm(velocity)/(norm(velocity) + norm(sideVel)))
                            : 0.0f)
                        ); 
                }
                velocity += sideVel; // Readiciona componente lateral
            }
            else
            {
                if (accelerate){
                    velocity += forwardsVector * acc * // aceleração básica
                        (0.2f + powf(norm(velocity), 0.3f)) *  // Simula inércia (inicialmente é difícil acelerar, aumenta
                        (0.5f + 0.5f *    // Se não esitver virando, acelera mais rapido
                            (norm(velocity)!=0.0f ? // % de quanto está virando
                            (norm(velocity)/(norm(velocity) + norm(sideVel)))
                            : 0.0f)
                        ); 
                }
                
            }
            // decaimento da velocidade por 2% (fricção das rodas)
            // Se não estiver acelerando, a velocidade decai (atrito das rodas com o chão)
            // Se estiver, controla a velocidade máxima
            if(!accelerate){
                // Se atingir minimo, velocidade é zero
                if (norm(velocity) < ZERO_VELOCITY_THRESHOLD)
                    velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
                else
                velocity *= (1-VELOCITY_DECAY_RATIO);
            }
            else if(norm(velocity)>=MAX_VEL){
                velocity=MAX_VEL * normalize(velocity);
            }
        }
    }
    
    void updateForwardsVector(){
    	forwardsVector = getMatrixRotate() * glm::vec4(0.0f, - 1.0f, 0.0f, 0.0f);    
    }

    glm::vec3 getRotation(){
        return rotation;
    }
    
    void update(){ 

    //    +    tecla para curva é pressionada
    //    |__..->+    turnAngle é incrementado
    //           |__..->+    rotation é atualizada com o ângulo de rotação
    //                  |__..->+ vetor fowards é atualizado
    //                         |__..->+ velocity é calculado baseado em fowards e o velocity anterior (o que pode gerar um angulo)
                                
    	updatePosition();
    	updateRotation(slide);
    	updateVelocity();
    	updateForwardsVector();
    }
};
