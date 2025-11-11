#include <glm/mat4x4.hpp>
#include <glm/vec4.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <matrices.h>

#define M_PI   3.14159265358979323846
#define M_PI_2 1.57079632679489661923

class Car
{
private:
    glm::vec4 position;
    glm::vec3 rotation;
    float velocity;
public:
    Car(){
        position = glm::vec4(0.0f, 0.0f, 0.0f, 0.0f);
        rotation = glm::vec3(-M_PI_2, 0.0f, 0.0f);
        float velocity=0.0;
    };

    glm::mat4 getTranslationMatrix(){
        return Matrix_Translate(position.x, position.y, position.z);
    }
    glm::mat4 getMatrixRotate(){
        return Matrix_Rotate_Z(rotation.z) * Matrix_Rotate_Y(rotation.y) * Matrix_Rotate_X(rotation.x);
    }
    void updatePosition(){
        position = position + velocity * (getMatrixRotate() * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f));
    }
    void setVelocity(float vel){
        velocity = -vel;
    }
    void rotateY(float theta){

        rotation.y += theta;

    }

};
