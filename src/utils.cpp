#include "utils.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

void Utils::processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}