#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <matrices.h>
#include <array>
#include <algorithm>
#include <numeric>

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

class Car
{
private:
    glm::vec4 position;
    glm::vec3 rotation;
    glm::vec4 velocity;
    glm::vec4 forwardsVector;
    bool brake;
    bool accelerate;
    bool slide;
    float turnAngle;

public:
    Car(){
        position = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
        rotation = glm::vec3(-M_PI_2, 0.0f, 0.0f);
        velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        forwardsVector = glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);
        brake = false;
        accelerate = false;
        slide = false;
        turnAngle = 0.0f;
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
    void setAccelerate(bool value){
    
        accelerate = value;
    
    }
    void setBrake(bool value){
    
        brake = value;
    
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
    void turnRight(){

        turnAngle -= 0.001f;

    }
    void turnLeft(){

        turnAngle += 0.001f;

    }

    void updatePosition(){
    
    	position = position + velocity;
    	
    }
    void updateRotation(bool drift){
    
        if(drift)
            rotation.y += turnAngle;

        else
            rotation.y += turnAngle * 3.0f * norm(velocity);
        
        turnAngle *= 0.9f;
        
        if(std::fabs(turnAngle) < 0.0008f) turnAngle = 0.0f;
    
    }
    void updateVelocity(){
      
        slide = false;
      
        glm::vec4 side = glm::normalize(crossproduct(forwardsVector, glm::vec4(0.0f, - 1.0f, 0.0f, 0.0f)));
	    
        glm::vec4 sideVel = side * dotproduct(velocity, side);
        
        if(brake){
          
            slide = true;
            
            if(norm(velocity) < 0.0066f) 
                velocity = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
            
            else
                velocity -= glm::normalize(velocity) * 0.006f * (0.1f + norm(velocity) / (norm(sideVel) + norm(velocity) + 0.001f));
        
        }else{
	    
	    if(norm(sideVel) > 0.0072f){
	    
	        slide = true;
          
        	velocity -= sideVel;

        	velocity *= 0.99f;
        	sideVel *= 0.975f;
        	    
        	if(accelerate){
        	    
        	    velocity += (0.2f + powf(norm(velocity), 0.3f)) * forwardsVector * 0.005f * (0.5f + norm(velocity) / (norm(sideVel) + norm(velocity) + 0.001f));
        	    
        	}
        	    
        	velocity += sideVel;
        	    
    	    }else{
    	        
    	        velocity = forwardsVector * norm(velocity);
    	        
    	        if(accelerate){
        	    
        	    velocity += (0.2f + powf(norm(velocity), 0.3f)) * forwardsVector * 0.005f * (0.5f + norm(velocity) / (norm(sideVel) + norm(velocity) + 0.001f));
        	    
        	}
    	        
    	        velocity *= 0.988f;
    	    
    	    }
        
        }


    }
    
    void updateForwardsVector(){
    
    	forwardsVector = getMatrixRotate() * glm::vec4(0.0f, - 1.0f, 0.0f, 0.0f);
    
    }

    void update(){

    	updatePosition();
    	
    	updateRotation(slide);

    	updateVelocity();
    	
    	updateForwardsVector();

    }


};
