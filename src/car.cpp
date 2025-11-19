#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <matrices.h>
#include <array>

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

class Car
{
private:
    glm::vec4 position;
    glm::vec3 rotation;
    glm::vec4 velocity;
    glm::vec4 forwardsVector;

public:
    Car(){
        position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        rotation = glm::vec3(-M_PI_2, 0.0f, 0.0f);
        velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        forwardsVector = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
    };

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
    void accelerate(){

    	velocity += 0.05f * forwardsVector;
    	
    }
    glm::vec4 getVelocity(){
        return velocity;
    }
    glm::vec4 getPosition(){
        return position;
    }
    std::array<glm::vec4, 3> getCam1(){

    	std::array<glm::vec4, 3> camera{};

    	glm::vec4 forwards = glm::length(velocity) == 0.0f ? forwardsVector : glm::normalize(velocity);

    	camera[0] = position + glm::vec4(0.0f, 6.0f, 0.0f, 0.0f) - 9.0f * forwards;

    	camera[1] = position + glm::vec4(0.0f, 3.0f, 0.0f, 0.0f) - camera[0];

    	camera[2] = glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);

        return camera;
    }
    glm::vec4 getCam1Lookat(){
    
        return position + glm::vec4(0.0f, 3.5f, 0.0f, 0.0f);
        
    }
    void rotateY(float theta){

        rotation.y += theta;

    }

    void updatePosition(){
    
    	position = position + velocity;
    	
    }
    void updateVelocity(){
	    
    	glm::vec4 side = glm::normalize(crossproduct(forwardsVector, glm::vec4(0.0f, - 1.0f, 0.0f, 0.0f)));
    	
    	glm::vec4 sideVel = side * dotproduct(velocity, side);
	
    	velocity -= sideVel;

    	velocity *= 0.99f;
    	sideVel *= 0.95f;

    	velocity += sideVel;


    }
    
    void updateForwardsVector(){
    
    	forwardsVector = getMatrixRotate() * glm::vec4(0.0f, - 1.0f, 0.0f, 0.0f);
    
    }

    void update(){

    	updatePosition();

    	updateVelocity();
    	
    	updateForwardsVector();

    }


};
