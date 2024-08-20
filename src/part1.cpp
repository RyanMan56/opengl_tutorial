#include "part1.h"
#include "shader.h"

#include <iostream>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <stb_image/stb_image.h>

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
}

void part1(GLFWwindow *window)
{
    Shader shader{"../shaders/basic.vs.glsl", "../shaders/basic.fs.glsl"};

    // We can use Element Buffer Objects to allow us to specify only the required vertices once, and then specify a separate array of indices to say which
    // vertex we should be drawing. Otherwise we would have to include the commented out vertices below, which would add an overhead of 50%
    float vertices[]{
        // First triangle
        // Positions      // Colors         // Texture coords (using 0 - 2.0 to play around with the different wrapping methods)
        0.5f, 0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 2.0f, 2.0f,  // top right
        0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 2.0f, 0.0f, // bottom right
        // -0.5f, 0.5f, 0.0f,  // top left

        // Second triangle
        // Positions        // Colors       // Texture coords (using 0 - 2.0 to play around with the different wrapping methods)
        // 0.5f, -0.5f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 2.0f   // top left
    };
    unsigned int indices[]{
        0, 1, 3, // First triangle
        1, 2, 3, // Second triangle
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

    // Create the EBO and bind it
    unsigned int EBO;
    glGenBuffers(1, &EBO);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

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
    // glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
    // Updated stride to 8 instead of 3 since we have more components in our array
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)0);
    // Now we enable the "position" vertex attribute
    glEnableVertexAttribArray(0);
    // Setting the (location = 1) aColor attributes
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(3 * sizeof(float))); // Offset is 3 * the size of a float, since the color attribute starts after the position data
    glEnableVertexAttribArray(1);
    // texture coords attribute:
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void *)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    // The call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object, so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // NOTE: Do NOT unbind the EBO while a VAO is active, as the bound element buffer object is stored in the VAO
    // glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // We can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO (but this rarely happens so isn't really necessary)
    glBindVertexArray(0);

    stbi_set_flip_vertically_on_load(true);

    // Load and create texture
    int width, height, numberOfChannels;
    unsigned char *data{stbi_load("../assets/textures/container.jpg", &width, &height, &numberOfChannels, 0)};

    unsigned int texture1;
    glGenTextures(1, &texture1); // Generate 1 texture and assigns the ID to our "texture" variable

    // Activates the given texture unit. Any glBindTexture calls will now affect the given texture unit. Allows for binding
    // multiple textures for a single drawing call (I guess similar to the relationship VAO and VBO have in managing state?)
    glActiveTexture(GL_TEXTURE0);

    glBindTexture(GL_TEXTURE_2D, texture1); // Bind it so any subsequent texture commands will configure our currently bound texture

    // Texture axes are s, t, r. Possible wrapping modes are:
    // - GL_REPEAT
    // - GL_MIRRORED_REPEAT
    // - GL_CLAMP_TO_EDGE
    // - GL_CLAMP_TO_BORDER
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data)
    {
        // Generates the texture image on the currently bound texture object at the active texture unit.
        // Params are:
        // target:          The texture target, typically one of GL_TEXTURE_1D, GL_TEXTURE_2D, GL_TEXTURE_3D
        // level:           The level-of-detail number. 0 is the base image. n is the nth mipmap reduction image
        // internalFormat:  The number of colour components in the texture
        // width:           The width of the texture image
        // height:          The height of the texture image
        // border:          This value must always be 0 (some legacy stuff apparently)
        // format:          The format of the pixel data
        // type:            The data type of the pixel data
        // data:            A pointer to the image data in memory
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);

        // Generates the mipmaps for this texture. It derives all required mipmap images from the base level which should be set
        // before calling this function. Each subsequent mipmap image is halved and generated until the mipmap image has a
        // width or height of 1px
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 1\n";
    }
    // Free the image data since we no longer need it
    stbi_image_free(data);

    // Reuse data, width, height, numberOfChannels variables for this new image
    data = stbi_load("../assets/textures/awesomeface.png", &width, &height, &numberOfChannels, 0);
    unsigned int texture2;
    glGenTextures(1, &texture2);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texture2);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    if (data)
    {
        // Because we're loading a .png (which has an alpha component) instead of a .jpg we now need to specify that the image
        // data contains an alpha channel as well by using GL_RGBA, otherwise OpenGL will incorrectly interpret the image data
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture 2\n";
    }
    stbi_image_free(data);

    // Uncomment to draw as a wireframe
    // glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // Tell OpenGL which texture unit each sampler belongs to
    shader.use();
    shader.setInt("texture1", 0); // GL_TEXTURE0
    shader.setInt("texture2", 1); // GL_TEXTURE1

    while (!glfwWindowShouldClose(window))
    {
        // Input
        processInput(window);

        // Rendering commands
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Can set a uniform which is accessible by all shaders in our shader program
        float timeValue{static_cast<float>(glfwGetTime())};
        float greenValue{(sin(timeValue) / 2.0f) + 0.5f};

        // Sets the current active shader program used in every subsequent shader and rendering call
        shader.use();
        shader.setFloat4("uniformColor", 0.0f, greenValue, 0.0f, 1.0f);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, texture1);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, texture2);

        glBindVertexArray(VAO);
        // Draw primitives using the currently active shader. Params are:
        //      mode: Specifies the kind of primitive to render, can be one of: GL_POINTS, GL_LINE_STRIP, GL_LINE_LOOP, GL_LINES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN, GL_TRIANGLES, GL_QUAD_STRIP, GL_QUADS, GL_POLYGON
        //      first: Specifies the starting index in the enabled arrays
        //      count: Specifies the number of vertices to render (3 here before there are 3 points in a triangle)
        // glDrawArrays(GL_TRIANGLES, 0, 3); // NOTE: No longer using this since we're using an EBO instead. Now we're using glDrawElements

        // Takes its indices from the EBO currently bound to the GL_ELEMENT_ARRAY_BUFFER target which means we would have to bind the corresponding EBO each time.
        // However, a VAO also keeps track of of EBO bindings. The last EBO that gets bound while a VAO is bound is stored as the VAO's Element Buffer Object.
        // So therefore, binding to a VAO then automatically binds that EBO
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // Check and call events and swap the buffers
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // De-allocate all resources once we no longer need them
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    shader.deleteShaderProgram();
}