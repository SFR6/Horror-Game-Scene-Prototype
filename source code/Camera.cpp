#include "Camera.hpp"

#include <iostream>

using namespace std;

namespace gps 
{
    glm::vec3 Camera::getCameraPosition()
    {
        return this->cameraPosition;
    }

    glm::vec3 Camera::getCameraTarget()
    {
        return this->cameraTarget;
    }

    void Camera::set_x(float new_x)
    {
        this->cameraPosition.x = new_x;
    }

    void Camera::set_y(float new_y)
    {
        this->cameraPosition.y = new_y;
    }

    void Camera::set_z(float new_z)
    {
        this->cameraPosition.z = new_z;
    }

    void Camera::setZ(float newZ)
    {
        this->cameraPosition.y = newZ;
        this->cameraTarget.y = newZ;
    }

    glm::vec3 Camera::getCameraFrontDirection()
    {
        return this->cameraFrontDirection;
    }

    glm::vec3 Camera::getCameraRightDirection()
    {
        return this->cameraRightDirection;
    }

    //Camera constructor
    Camera::Camera(glm::vec3 cameraPosition, glm::vec3 cameraTarget, glm::vec3 cameraUp) 
    {
        this->cameraPosition = cameraPosition;
        this->cameraTarget = cameraTarget;
        this->cameraFrontDirection = glm::normalize(cameraTarget - cameraPosition);
        this->cameraRightDirection = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->cameraFrontDirection));
        this->cameraUpDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraRightDirection));
    }

    //return the view matrix, using the glm::lookAt() function
    glm::mat4 Camera::getViewMatrix() 
    {
        return glm::lookAt(this->cameraPosition, this->cameraPosition + this->cameraFrontDirection, this->cameraUpDirection);
    }

    //update the camera internal parameters following a camera move event
    void Camera::move(MOVE_DIRECTION direction, float speed) 
    {
        if (direction == MOVE_FORWARD)
        {
            this->cameraPosition.x += (this->cameraFrontDirection.x * speed);
            this->cameraTarget.x += (this->cameraFrontDirection.x * speed);

            //this->cameraPosition.y += (this->cameraFrontDirection.y * speed);
            //this->cameraTarget.y += (this->cameraFrontDirection.y * speed);

            this->cameraPosition.z += (this->cameraFrontDirection.z * speed);
            this->cameraTarget.z += (this->cameraFrontDirection.z * speed);
        }
        else if (direction == MOVE_BACKWARD)
        {
            this->cameraPosition.x -= (this->cameraFrontDirection.x * speed);
            this->cameraTarget.x -= (this->cameraFrontDirection.x * speed);

            //this->cameraPosition.y -= (this->cameraFrontDirection.y * speed);
            //this->cameraTarget.y -= (this->cameraFrontDirection.y * speed);

            this->cameraPosition.z -= (this->cameraFrontDirection.z * speed);
            this->cameraTarget.z -= (this->cameraFrontDirection.z * speed);
        }
        else if (direction == MOVE_LEFT)
        {
            this->cameraPosition.x -= (this->cameraRightDirection.x * speed);
            this->cameraTarget.x -= (this->cameraRightDirection.x * speed);

            //this->cameraPosition.y -= (this->cameraRightDirection.y * speed);
            //this->cameraTarget.y -= (this->cameraRightDirection.y * speed);

            this->cameraPosition.z -= (this->cameraRightDirection.z * speed);
            this->cameraTarget.z -= (this->cameraRightDirection.z * speed);
        }
        else if (direction == MOVE_RIGHT)
        {
            this->cameraPosition.x += (this->cameraRightDirection.x * speed);
            this->cameraTarget.x += (this->cameraRightDirection.x * speed);

            //this->cameraPosition.y += (this->cameraRightDirection.y * speed);
            //this->cameraTarget.y += (this->cameraRightDirection.y * speed);

            this->cameraPosition.z += (this->cameraRightDirection.z * speed);
            this->cameraTarget.z += (this->cameraRightDirection.z * speed);
        }
    }

    //update the camera internal parameters following a camera rotate event
    //yaw - camera rotation around the y axis
    //pitch - camera rotation around the x axis
    void Camera::rotate(float pitch, float yaw) 
    {
        glm::vec3 direction;
        direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction.y = sin(glm::radians(pitch));
        direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->cameraFrontDirection = glm::normalize(direction);
     
        this->cameraRightDirection = glm::normalize(glm::cross(glm::vec3(0.0f, 1.0f, 0.0f), this->cameraFrontDirection));
        this->cameraUpDirection = glm::normalize(glm::cross(this->cameraFrontDirection, this->cameraRightDirection));
    }
}