#include <stdio.h>
#include <stdbool.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <time.h>
#include <math.h>
#include <cgl_math/cgl_math.c>
#include "shaders.h"
#include "textures.h"

double startTime;
float speed = 0.0;
float x = 0.0;
float y = 1;
double lastTime = 0.0f;
double DAMPENER = 1;
float transformBuff[4] = { 0 };
bool hovering = false;
bool previouslyHovering = false;
bool mousePressed = false;
GLFWwindow* window = NULL;

vec4 mouseRay;

// Check if mouse ray intersects with the cube (simplified for orthographic projection)
bool checkRayCubeIntersection(double mouseX, double mouseY) {
    int windowWidth, windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    
    // Convert mouse coordinates to world coordinates
    // Since we're using orthographic projection, we can directly map screen to world
    float worldX = (2.0f * mouseX / windowWidth - 1.0f);
    float worldY = -(2.0f * mouseY / windowHeight - 1.0f);
    
    // Apply the view transformation (translation by x, y)
    worldX -= x;
    worldY -= y;
    
    // Check if the point is within the cube bounds
    // The cube is scaled by 0.25, so its bounds are ±0.125 in each direction
    float cubeSize = 0.125f;
    
    return (worldX >= -cubeSize && worldX <= cubeSize &&
            worldY >= -cubeSize && worldY <= cubeSize);
}

void processInput(GLFWwindow* window) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

void framebufferSizeCallback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

void createMouseRay(double x, double y, vec4 out) {
    int windowWidth;
    int windowHeight;
    glfwGetWindowSize(window, &windowWidth, &windowHeight);
    double xpos = isaacnormalize(0, windowWidth, x);
    double ypos = isaacnormalize(0, windowHeight, y);
    double zpos = 1.0f;
    ypos = -ypos;

    out[0] = xpos;
    out[1] = ypos;
    out[2] = zpos;
    out[3] = 0.0;
}

void cursorCallback(GLFWwindow* window, double xpos, double ypos) {
    createMouseRay(xpos, ypos, mouseRay);
    
    // Check for continuous hovering when mouse is pressed
    if (mousePressed && checkRayCubeIntersection(xpos, ypos)) {
        hovering = true;
    } else if (!mousePressed) {
        hovering = false;
    }
}

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
        mousePressed = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        
        if (checkRayCubeIntersection(xpos, ypos)) {
            hovering = true;
        }
    } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_RELEASE) {
        mousePressed = false;
        hovering = false;
    }
}

void initGLFW(GLFWwindow** window) {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    *window = glfwCreateWindow(800, 600, "I love microsoft", NULL, NULL);
    if (*window == NULL)
    {
        glfwTerminate();
    }
    glfwMakeContextCurrent(*window);
    glfwSetFramebufferSizeCallback(*window, framebufferSizeCallback);
    glfwSetCursorPosCallback(*window, cursorCallback);
    glfwSetMouseButtonCallback(*window, mouseButtonCallback);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        *window = NULL;
        glfwTerminate();
    }
}

void initCube(unsigned int* VBO, unsigned int* VAO, unsigned int* texture) {
    *texture = createTexture();
    float vertices[] = {
        // positions          // tex coords
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
         0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
         0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
         0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
         0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
        -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
        -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
    };

    glGenVertexArrays(1, VAO);
    glBindVertexArray(*VAO);

    glGenBuffers(1, VBO);
    glBindBuffer(GL_ARRAY_BUFFER, *VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0); // Positions
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float))); // Textures
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
}

void initShaders(unsigned int* shaderProgram, unsigned int* vertexShader, unsigned int* fragmentShader) {
    char vertex[] = "vertex_main.glsl";
    unsigned int s1 = compileShaderFile(vertex, GL_VERTEX_SHADER);

    char fragment[] = "fragment_main.glsl";
    unsigned int s2 = compileShaderFile(fragment, GL_FRAGMENT_SHADER);


    unsigned int shaders[] = { s1, s2 };
    unsigned int newProgram = createGPUProgram(shaders, sizeof(shaders) / sizeof(unsigned int));

    *shaderProgram = newProgram;
}

void updateYVelocity() {
    if (previouslyHovering) {
        previouslyHovering = false;
        lastTime = glfwGetTime();
    }
    double currentTime = glfwGetTime();
    double deltaTime = currentTime - lastTime;
    lastTime = currentTime;
    speed -= 9.8 * deltaTime * DAMPENER;
    y += speed * deltaTime;
    float objectBottom = y - 0.25f;
    if (objectBottom <= -1.0f) {
        y = -1.0f + 0.25f;
        speed = -speed * 0.7f;
    }
}

void transformCube(int shaderProgram, vec3 position, bool rotate) {
    mat4 model;

    identitymat4(model);
    translatemat4(model, position[0], position[1], position[2]);

    if (rotate) {
        vec3 rotate = { 0.5, 0.5, 1.0 };
        rotatemat4(model, glfwGetTime(), rotate);
    }
    scalemat4(model, 0.25, 0.25, 0.25);
    int modelLoc = glGetUniformLocation(shaderProgram, "Model");
    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);

    mat4 view;
    identitymat4(view);

    if (hovering) {
        previouslyHovering = true;
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        int windowWidth;
        int windowHeight;
        glfwGetWindowSize(window, &windowWidth, &windowHeight);
        xpos = isaacnormalize(0, windowWidth, xpos);
        ypos = isaacnormalize(0, windowHeight, ypos);

        speed = 0;
        x = xpos;
        y = -ypos;
    }
    else {
        updateYVelocity();
    }

    translatemat4(view, x, y, 0.0f);
    int viewLoc = glGetUniformLocation(shaderProgram, "View");
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, view);

    mat4 orthog;
    ortho(-1.0f, 1.0f, -1.0f, 1.0f, 0.00f, 10.0f, orthog);
    int translateLoc = glGetUniformLocation(shaderProgram, "Orthographic");
    glUniformMatrix4fv(translateLoc, 1, GL_FALSE, orthog);
}

void bindProgram(int shaderProgram, int texture) {
    glUseProgram(shaderProgram);
    if (texture) {
        glBindTexture(GL_TEXTURE_2D, texture);
    }
}

void draw(int VAO) {
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void drawCube(GLFWwindow* window, unsigned int shaderProgram, unsigned int VAO, unsigned int texture) {
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    vec3 cubePositions[] = {
        {-0.3, -0.30, -0.2},
        {0.3, 0.0, -0.9},
        {-0.5, 0.2, -0.19},
        {0.7, 0.7, -0.29}
    };
    int length = sizeof(cubePositions) / sizeof(cubePositions[0]);
    for (int i = 0; i < length; i++) {
        bindProgram(shaderProgram, texture);
        transformCube(shaderProgram, cubePositions[i], i % 2 == 0 ? true : false);
        draw(VAO);
    }
}

int main() {
    initGLFW(&window);
    if (window == NULL) {
        return -1;
    }

    unsigned int shaderProgram, vertexShader, fragmentShader;
    initShaders(&shaderProgram, &vertexShader, &fragmentShader);

    unsigned int VBO, VAO, texture;
    initCube(&VBO, &VAO, &texture);

    startTime = glfwGetTime();
    glEnable(GL_DEPTH_TEST);
    while (!glfwWindowShouldClose(window)) {
        processInput(window);
        drawCube(window, shaderProgram, VAO, texture);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();

    return 0;
}