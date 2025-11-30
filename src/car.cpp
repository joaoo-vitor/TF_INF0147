#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <matrices.h>
#include <array>
#include <algorithm>
#include <numeric>
#include <iostream>
#include <limits>

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

#define MAX_VEL 50.0f
#define MIN_VEL 2.0f // Velocidade minima para quando estiver freiando, parar
#define ACCELERATION 10.0f
#define ACCELERATION_REVERSE 0.0001f
#define BRAKE_ACCELERATION 0.005f

#define VELOCITY_DECAY_RATIO 0.01f
#define SIDE_VELOCITY_DECAY_RATIO 0.05f
#define TURNING_DECAY_RATIO 5.0f

#define ZERO_VELOCITY_THRESHOLD 0.000006f
#define ZERO_TURNANGLE_THRESHOLD 0.000066f
#define ZERO_TURNVEL_THRESHOLD 0.000006f

#define CAMERA_INITIAL_HEIGHT 8.0f

#define MAX_SIDE_GRIP 0.30f
#define MIN_CORRELATION_GRIP 0.99f
#define SLIDING_TURN_COEFICIENT 3.00f
#define NOT_SLIDING_TURN_COEFICIENT 0.30f

#define TURN_SPEED_COEFICIENT 0.6f

#define ACCELERATE_TYRE_SPEED_COEFICIENT 60.0f

#define SLIDING_DRAG_COEFICIENT 20.0f


class Car
{
private:
    glm::vec4 position;
    glm::vec3 rotation;
    glm::vec4 velocity;
    glm::vec4 forwardsVector; // Vetor que aponta para frente do carro
    bool brake;
    bool accelerate;
    bool isSliding;
    bool reverse;
    float turnAngle;
    const float mass = 1.0f;
    const float verysmallnumber = std::numeric_limits<float>::epsilon();


public:
    // Carro começa no centro (0,0,0)
    // Velocidade zero
    // É rotacionado em X inicialmente apenas para que seu modelo fique certo (horizontal)
    Car(){
        position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        rotation = glm::vec3(0.0f, 0.0f, 0.0f);
        velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        forwardsVector = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        brake = false;
        accelerate = false;
        isSliding = false;
        turnAngle = 0.0f;
        reverse = false;
    };

    bool getIsSliding(){
    	return isSliding;
    }

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
    void turnRight(float elapsed_time){
        turnAngle -= TURN_SPEED_COEFICIENT * elapsed_time;
    }
    void turnLeft(float elapsed_time){
        turnAngle += TURN_SPEED_COEFICIENT * elapsed_time;
    }

    void updatePosition(float elapsed_time){
    	position = position + velocity * elapsed_time;
    }

    void updateRotation(float elapsed_time){
        float forca_centrifuga = powf(turnAngle, 2) * mass * norm(velocity);

        if(forca_centrifuga > MAX_SIDE_GRIP || brake) isSliding = true;
        else if(dotproduct(velocity, forwardsVector) / (norm(velocity) + 0.000001f) > MIN_CORRELATION_GRIP) isSliding = false;

        if(isSliding)
            rotation.y += turnAngle * SLIDING_TURN_COEFICIENT * elapsed_time;
        else
        rotation.y += turnAngle * NOT_SLIDING_TURN_COEFICIENT * norm(velocity) * elapsed_time;


        turnAngle *= 1 - TURNING_DECAY_RATIO * elapsed_time; // Decaimento do ângulo de curva

        // Threshhold para zerar curva
        if(std::fabs(turnAngle) < ZERO_TURNANGLE_THRESHOLD) turnAngle = 0.0f;
    }

    void updateVelocity(float elapsed_time){
        // Calcula sideVelocity e forwardsVelocity
        // sideVelocity é o vetor da velocidade que aponta para a lateral do carro (esquerda ou direita)
        glm::vec4 side = normalize(crossproduct(forwardsVector, glm::vec4(0.0f, -1.0f, 0.0f, 0.0f)));
        glm::vec4 sideVelocity = side * dotproduct(velocity, side); // sideVelocity é o vetor da componente de velocidade perpendicular ao fowards

        float tyreSpeed = dotproduct(forwardsVector, velocity);

        if(brake)
            tyreSpeed = 0.0f;
        else if(accelerate)
            tyreSpeed = ACCELERATE_TYRE_SPEED_COEFICIENT;
        if(isSliding){
            velocity -= forwardsVector * tyreSpeed;
            velocity -= elapsed_time * (norm(velocity) > mass * SLIDING_DRAG_COEFICIENT? normalize(velocity) * mass * SLIDING_DRAG_COEFICIENT : velocity);
            velocity += forwardsVector * tyreSpeed;

        }else{
            glm::vec4 forwards = forwardsVector * (dotproduct(forwardsVector, velocity) + verysmallnumber);
            forwards = normalize(forwards);
            velocity = forwards * (norm(velocity) + (accelerate ? ACCELERATION : 0.0f) * elapsed_time);

        }

        if(norm(velocity)>=MAX_VEL){
            velocity=MAX_VEL * normalize(velocity);
        }

        if((norm(velocity)<=MIN_VEL) && (brake)){
            velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        }
        
        if(!accelerate && !brake) velocity *= 1 - VELOCITY_DECAY_RATIO * elapsed_time;
        
    }

    void updateForwardsVector(){
    	forwardsVector = getMatrixRotate() * glm::vec4(0.0f, 0.0f, 1.0f, 0.0f);
    }

    glm::vec3 getRotation(){
        return rotation;
    }

    void update(float elapsed_time){
        //    +    tecla para curva é pressionada
        //    |__..->+    turnAngle é incrementado
        //           |__..->+    rotation é atualizada com o ângulo de rotação
        //                  |__..->+ vetor fowards é atualizado
        //                         |__..->+ velocity é calculado baseado em fowards e o velocity anterior (o que pode gerar um angulo)

    	updatePosition(elapsed_time);
    	updateRotation(elapsed_time);
    	updateVelocity(elapsed_time);
    	updateForwardsVector();
    }
};
