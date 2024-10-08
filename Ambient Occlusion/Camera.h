#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>

#include "RenderData.h"

enum Camera_Direction
{
    FRONT = 0,
    BACK,
    LEFT,
    RIGHT
};

class Camera
{
public:
    glm::vec3 Position;
    glm::vec3 Front;
    glm::vec3 Up;
    glm::vec3 Right;
    glm::vec3 WorldUp;

    float Yaw;
    float Pitch;

    float MovementSpeed;
    float MouseSensitivity;
    float Zoom;

    // defult values
    static const float DEFULT_YAW;
    static const float DEFULT_PITCH;
    static const float DEFULT_SPEED;
    static const float DEFULT_SHIFT_MULTIPLIER;
    static const float DEFULT_SENSITIVITY;
    static const float DEFULT_ZOOM;

    Camera(glm::vec3 _position = glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3 _up = glm::vec3(0.0f, 1.0f, 0.0f), float _yaw = DEFULT_YAW, float _pitch = DEFULT_PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float _yaw, float _pitch);
    glm::mat4 GetViewMatrix();
    void ProcessKeyboard(enum Camera_Direction dir, float deltaTime);
    void ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch = true);
    void ProcessMouseScroll(float yoffset);

private:
    void updateCameraVectors();
};