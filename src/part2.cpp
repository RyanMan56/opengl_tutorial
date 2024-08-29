// Starts from this chapter:
// https://learnopengl.com/Getting-started/Transformations
#include "part2.h"
#include "utils.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

void part2(GLFWwindow *window)
{
    glm::vec4 vec(1.0f, 0.0f, 0.0f, 1.0f);                      // Final param sets the homogenous coordinate to 1.0f
    glm::mat4 trans{glm::mat4(1.0f)};                           // Creates an identity matrix (all diagonal elements set to 1.0f, everything else set to 0.0f)
    trans = glm::translate(trans, glm::vec3(1.0f, 1.0f, 0.0f)); // Translates the identity matrix by vector (1.0f, 1.0f, 0.0f), creating our translation matrix
    vec = trans * vec;                                          // Multiply the vector by the translation matrix
    std::cout << "(" << vec.x << ", " << vec.y << ", " << vec.z << ")\n";

    while (!glfwWindowShouldClose(window))
    {
        Utils::processInput(window);

        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}