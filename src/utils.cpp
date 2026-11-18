#include "utils.h"

#include <GLFW/glfw3.h>
#include <glad/glad.h>

void Utils::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}