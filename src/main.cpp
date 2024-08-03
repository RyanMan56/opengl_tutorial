#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void checkShaderCompilation(unsigned int &shader, std::string_view shaderType)
{
    // Check if the shader compiled successfully
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    // Display an error if the shader failed to compile
    if (!success)
    {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::" << shaderType << "::COMPILATION_FAILED\n"
                  << infoLog << '\n';
    }
}

void checkShaderProgramLinkage(unsigned int &shaderProgram)
{
    // Check if the shader program linked successfully
    int success;
    char infoLog[512];
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);

    // Display an error if the shader program failed to link
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER_PROGRAM::LINKAGE_FAILED\n"
                  << infoLog << '\n';
    }
}

int main()
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // According to the tutorial this is needed for mac, but seems to work fine without for me

    GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Tutorial", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window\n";
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD\n";
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    float vertices[]{
        -0.5f,
        -0.5f,
        0.0f,

        0.5f,
        -0.5f,
        0.0f,

        0.0f,
        0.5f,
        0.0f,
    };

    // Create a Vertex Array Object and bind it. When a VAO is bound, any subsequent vertex attribute calls from that point onwards will be
    // stored inside the VAO. This means that whenever we're configuring vertex attribute pointers we only need to make those calls once, and
    // whenever we want to draw the object, we can just bind the corresponding VAO
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // Create a Vertex Buffer Object to store large amount of vertex data
    unsigned int VBO;
    glGenBuffers(1, &VBO);

    // Binds a buffer object to a buffer type target. Only a single buffer can be bound for each buffer type.
    // Then any buffer calls to GL_ARRAY_BUFFER will be used to configure the currently bound buffer,
    // in this case VBO
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // Allocates GPU memory and copies the vertex data into the buffer's memory.
    // The final parameter can be one of:
    //  GL_STREAM_DRAW: the data is set only once and used by the GPU at most a few times.
    //  GL_STATIC_DRAW: the data is set only once and used many times.
    //  GL_DYNAMIC_DRAW: the data is changed a lot and used many times.
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    const char *vertexShaderSource{
        "#version 330 core\n"                   // Specify the GLSL version (this matches the OpenGL version)
        "layout (location = 0) in vec3 aPos;\n" // Specify input vertex attributes using the "in" keyword. Also specifically set the location of the input variable using "layout"
        "void main()\n"
        "{\n"
        "   gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);\n" // Assign the position to the predefined gl_Position vec4
        "}\0"};

    // Create the shader, assign the source code to the shader object, and compile the shader
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    checkShaderCompilation(vertexShader, "VERTEX");

    const char *fragmentShaderSource{
        "#version 330 core\n"
        "out vec4 FragColor;\n"
        "void main()\n"
        "{\n"
        "   FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
        "}\n"};

    unsigned int fragmentShader;
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    checkShaderCompilation(fragmentShader, "FRAGMENT");

    // Create a shader program, which is multiple shaders linked together. Outputs of each shader are linked to the inputs of the next shader
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram); // Shaders are linked together here
    checkShaderProgramLinkage(shaderProgram);

    // Tells OpenGL how to interpret the vertex data (per vertex attribute). Params are:
    //      index:      The index of the vertex attribute
    //      size:       The number of components per vertex attribute (1, 2, 3, or 4)
    //      type:       The data type of each component in the array
    //      normalized: Whether the data should be normalized (clamped to -1 to 1 for signed values, and 0 to 1 for unsigned values)
    //      stride:     The byte offset between consecutive vertex attributes. If stride is 0, the vertex attributes are understood to be tightly packed in the array
    //      pointer:    The offset of the first component of the first vertex attribute in the array
    //
    // In this case,
    //      index is 0 because we specified the location of the position vertex attribute in the vertex shader with layout (location = 0)
    //      size is 3 because a vec3 has 3 values
    //      type is GL_FLOAT because a vec* in GLSL consists of floating point values
    //      normalized is GL_FALSE because our data is already normalized
    //      stride is 3 * sizeof(float) since the next set of position data is located exactly 3 times the size of a float away. In this case we could have also specified stride to be 0 since our array is tightly packed
    //      pointer is a void* since there is no offset to the position data, it is at the beginning of the array
    //
    //  Since the previously defined VBO is still bound to GL_ARRAY_BUFFER when calling glVertexAttribPointer, vertex attribute 0 is now associated with its vertex data
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);

    // Now we enable the "position" vertex attribute
    glEnableVertexAttribArray(0);

    // The call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object, so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // We can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO (but this rarely happens so isn't really necessary)
    glBindVertexArray(0);

    // Now the shaders are linked to the shader program we no longer need them, so are free to delete them
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Sets the current active shader program used in every subsequent shader and rendering call
        glUseProgram(shaderProgram);
        glBindVertexArray(VAO);
        // Draw primitives using the currently active shader. Params are:
        //      mode: Specifies the kind of primitive to render, can be one of: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_QUAD_STRIP, GL_QUADS, GL_POLYGON
        //      first: Specifies the starting index in the enabled arrays
        //      count: Specifies the number of vertices to render (3 here before there are 3 points in a triangle)
        glDrawArrays(GL_TRIANGLES, 0, 3);

        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwTerminate();

    std::cout << "Hello OpenGL!\n";

    return 0;
}