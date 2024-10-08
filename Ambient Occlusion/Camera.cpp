#include "Camera.h"

//const float Camera::DEFULT_YAW = -90.0f;
const float Camera::DEFULT_YAW = 0.0f;
const float Camera::DEFULT_PITCH = 0.0f;
const float Camera::DEFULT_SPEED = 10.5f;
const float Camera::DEFULT_SHIFT_MULTIPLIER = 2.0f;
const float Camera::DEFULT_SENSITIVITY = 0.1f;
const float Camera::DEFULT_ZOOM = 45.0f;

Camera::Camera(glm::vec3 _position, glm::vec3 _up, float _yaw, float _pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(DEFULT_SPEED), MouseSensitivity(DEFULT_SENSITIVITY), Zoom(DEFULT_ZOOM)
{
    Position = _position;
    WorldUp = _up;
    Yaw = _yaw;
    Pitch = _pitch;
    updateCameraVectors();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float _yaw, float _pitch) : Front(glm::vec3(0.0f, 0.0f, -1.0f)), MovementSpeed(DEFULT_SPEED), MouseSensitivity(DEFULT_SENSITIVITY), Zoom(DEFULT_ZOOM)
{
    Position = glm::vec3(posX, posY, posZ);
    WorldUp = glm::vec3(upX, upY, upZ);
    Yaw = _yaw;
    Pitch = _pitch;
    updateCameraVectors();
}

glm::mat4 Camera::GetViewMatrix()
{
    return glm::lookAt(Position, Position + Front, Up);
}

void Camera::ProcessKeyboard(enum Camera_Direction dir, float deltaTime)
{
    float velocity = MovementSpeed * deltaTime;
    bool shiftHeld = glfwGetKey(RenderData::window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS;
    if (dir == FRONT)
        Position += Front * velocity * ((shiftHeld) ? (DEFULT_SHIFT_MULTIPLIER) : (1.f));
    if (dir == BACK)
        Position -= Front * velocity * ((shiftHeld) ? (DEFULT_SHIFT_MULTIPLIER) : (1.f));
    if (dir == LEFT)
        Position -= Right * velocity * ((shiftHeld) ? (DEFULT_SHIFT_MULTIPLIER) : (1.f));
    if (dir == RIGHT)
        Position += Right * velocity * ((shiftHeld) ? (DEFULT_SHIFT_MULTIPLIER) : (1.f));
}

void Camera::ProcessMouseMovement(float xoffset, float yoffset, bool constrainPitch)
{
    xoffset *= MouseSensitivity;
    yoffset *= MouseSensitivity;

    Yaw += xoffset;
    Pitch += yoffset;

    if (constrainPitch)
    {
        if (Pitch > 89.0f)
            Pitch = 89.0f;
        if (Pitch < -89.0f)
            Pitch = -89.0f;
    }

    updateCameraVectors();
}

void Camera::ProcessMouseScroll(float yoffset)
{
    Zoom -= (float)yoffset;
    if (Zoom < 1.0f)
        Zoom = 1.0f;
    if (Zoom > 45.0f)
        Zoom = 45.0f;
}

void Camera::updateCameraVectors()
{
    // calculate the new Front vector
    glm::vec3 front;
    front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    front.y = sin(glm::radians(Pitch));
    front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
    Front = glm::normalize(front);
    // also re-calculate the Right and Up vector
    Right = glm::normalize(glm::cross(Front, WorldUp));  // normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
    Up = glm::normalize(glm::cross(Right, Front));
}