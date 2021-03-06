#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "camera.hpp"

Camera::Camera(float screenWidth, float screenHeight, int forward, int back, int left, int right, int up, int down, float mouseStartX, float mouseStartY, bool constrainPitch, bool fpsStyle) {
    width = screenWidth;
    height = screenHeight;
    forwardKey = forward;
    backKey = back;
    leftKey = left;
    rightKey = right;
    upKey = up;
    downKey = down;
    lastX = mouseStartX;
    lastY = mouseStartY;
    pitchConstrained = constrainPitch;
    fps = fpsStyle;

    firstMouse = true;

    yaw = -90.0f;
    pitch = 0.0f;
    zoomValue = 45.0f;

    speed = 6.0f;
    sensitivity = 0.1f;
    
    cameraPos = glm::vec3(0.0f, 0.0f, 5.0f);
    cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    view = glm::mat4(1.0f);
    projection = glm::mat4(1.0f);

}

void Camera::movePosition(GLFWwindow *window, float frameTime) {
    float ms = speed * frameTime;
    glm::vec3 front = cameraFront;
    if (fps) {
        // Don't change the y height when moving forward/backward/sideways in fps style
        front.y = 0.0f;

        if (glfwGetKey(window, upKey) == GLFW_PRESS) {
            cameraPos.y += ms;
            cameraFront.y += ms;
        } else if (glfwGetKey(window, downKey) == GLFW_PRESS) {
            cameraPos.y -= ms;
            cameraFront.y -= ms;
        }

    }
    if (glfwGetKey(window, leftKey) == GLFW_PRESS) {
        cameraPos -= glm::normalize(glm::cross(front, cameraUp)) * ms;
    } else if (glfwGetKey(window, rightKey) == GLFW_PRESS) {
        cameraPos += glm::normalize(glm::cross(front, cameraUp)) * ms;
    } else if (glfwGetKey(window, forwardKey) == GLFW_PRESS) {
        cameraPos += ms * front;
    } else if (glfwGetKey(window, backKey) == GLFW_PRESS) {
        cameraPos -= ms * front;
    }
}

void Camera::moveDirection(GLFWwindow* window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
        return;
    }

    float xOffset = xpos - lastX;
    float yOffset = lastY - ypos;

    lastX = xpos;
    lastY = ypos;

    xOffset *= sensitivity;
    yOffset *= sensitivity;

    yaw += xOffset;
    pitch += yOffset;

    if (pitchConstrained) {
        if (pitch > 89.0) {
            pitch = 89.0;
        } else if (pitch < -89.0) {
            pitch = -89.0;
        }
    }
}

void Camera::zoom(double xoffset, double yoffset) {
    zoomValue -= (float)yoffset;
    if (zoomValue < 1.0f) {
        zoomValue = 1.0f;
    } else if (zoomValue > 45.0f) {
        zoomValue = 45.0f;
    }
}

void Camera::updateProjection() {
    projection = glm::perspective(glm::radians(zoomValue), (width / height), 0.1f, 100.0f);
}

void Camera::updateCameraFront() {
    float x, y, z;
    x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    y = sin(glm::radians(pitch));
    z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraFront = glm::normalize(glm::vec3(x, y, z));
}

void Camera::updateView() {
    glm::mat4 focus = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    // view = glm::translate(focus, cameraFront);
    view = focus;
}

void Camera::update() {
    updateCameraFront();
    updateProjection();
    updateView();
}

glm::mat4 Camera::getProjection() {
    return projection;
}

glm::mat4 Camera::getView() {
    return view;
}