/*
 main
 
 Copyright 2012 Thomas Dalling - http://tomdalling.com/

 Licensed under the Apache License, Version 2.0 (the "License");
 you may not use this file except in compliance with the License.
 You may obtain a copy of the License at

     http://www.apache.org/licenses/LICENSE-2.0

 Unless required by applicable law or agreed to in writing, software
 distributed under the License is distributed on an "AS IS" BASIS,
 WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 See the License for the specific language governing permissions and
 limitations under the License.
 */

#include "Helper.h"

// third-party libraries
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

// standard C++ libraries
#include <cassert>
#include <iostream>
#include <stdexcept>
#include <cmath>

// tdogl classes
// #include "tdogl/Program.h"
// #include "tdogl/Texture.h"

#include "Program.h"
#include "Texture.h"

 #include "tetmesh.hpp"

// constants
const glm::vec2 SCREEN_SIZE(800, 600);

// globals
tdogl::Texture* gTexture = NULL;
tdogl::Program* gProgram = NULL;
GLuint gVAO = 0; // vertex array object
GLuint gVBO = 0; // vertex buffer object
GLuint gIBO = 0; // index buffer object
GLfloat gDegreesRotated = 0.0f;
int windowWidth = 640;
int windowHeight = 480;
// bool quit_program = false;


// returns the full path to the file `fileName` in the resources directory of the app bundle
static std::string ResourcePath(std::string fileName) {
    return GetProcessPath() + "/../resources/" + fileName;
}

// loads the vertex shader and fragment shader, and links them to make the global gProgram
static void LoadShaders() {
    std::vector<tdogl::Shader> shaders;
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("vertex-shader.txt"), GL_VERTEX_SHADER));
    shaders.push_back(tdogl::Shader::shaderFromFile(ResourcePath("fragment-shader.txt"), GL_FRAGMENT_SHADER));
    gProgram = new tdogl::Program(shaders);

    gProgram->use();

    //set the "projection" uniform in the vertex shader, because it's not going to change
    glm::mat4 projection = glm::perspective<float>(50.0, SCREEN_SIZE.x/SCREEN_SIZE.y, 0.1, 1000.0);
    //glm::mat4 projection = glm::ortho<float>(-2, 2, -2, 2, 0.1, 10);
    gProgram->setUniform("projection", projection);

    //set the "camera" uniform in the vertex shader, because it's also not going to change
    glm::mat4 camera = glm::lookAt(glm::vec3(10,8,10), glm::vec3(0,0,0), glm::vec3(0,1,0));
    gProgram->setUniform("camera", camera);

    gProgram->stopUsing();
}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    //!Close the window if the ESC key was pressed
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

void error_callback(int error, const char* description)
{
    std::cerr<<description<<std::endl;
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    //!Resize the viewport to fit the window size - draw to entire window
    glViewport(0, 0, width, height);
}

// loads a cube into the VAO and VBO globals: gVAO and gVBO
static void LoadMesh(const double* points, const GLuint num_points,
                     const GLuint* indices, const GLuint num_indices)
{
    // std::cout << num_points << std::endl;
    // for (GLuint i = 0 ; i < num_points ; i++)
    //     std::cout << points[3*i] << " " << points[3*i+1] << " " << points[3*i+2] << std::endl;
    // make and bind the VAO
    glGenVertexArrays(1, &gVAO);
    glBindVertexArray(gVAO);
    
    // make and bind the VBO
    glGenBuffers(1, &gVBO);
    glBindBuffer(GL_ARRAY_BUFFER, gVBO);
    glBufferData(GL_ARRAY_BUFFER, 3*num_points*sizeof(double), points, GL_STATIC_DRAW);
    glEnableVertexAttribArray(gProgram->attrib("vert")); // connect the xyz to the "vert" attribute of the vertex shader
    // glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_FLOAT, GL_FALSE, 3*sizeof(GLfloat), NULL);
    glVertexAttribPointer(gProgram->attrib("vert"), 3, GL_DOUBLE, GL_FALSE, 0, NULL);
    // unbind the VAO
    glBindVertexArray(0);
    // std::cout << num_points << std::endl;
    // std::cout << num_indices << std::endl;

    // make and bind the IBO
    glGenBuffers(1, &gIBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices*sizeof(GLuint), indices, GL_STATIC_DRAW);
}


// draws a single frame
static void Render(const GLuint num_points, const GLvoid* indices, const GLuint num_indices) {
    // clear everything
    glClearColor(0, 0, 0, 1); // black
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // bind the program (the shaders)
    gProgram->use();

    // set the "model" uniform in the vertex shader, based on the gDegreesRotated global
    gProgram->setUniform("model", glm::rotate(glm::mat4(), gDegreesRotated, glm::vec3(0,1,0)));

    glBindVertexArray(gVAO); // bind the VAO
    // glDrawArrays(GL_POINTS, 0, 3*num_points); // draw the VAO
    // glBindVertexArray(0); // unbind the VAO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, gIBO);
    // std::cout << num_indices << std::endl;
    glDrawElements(GL_LINES, num_indices, GL_UNSIGNED_INT, 0);
    // glDrawElements(GL_TRIANGLES, num_indices, GL_UNSIGNED_INT, indices);
    gProgram->stopUsing(); // stop the program
}


// update the scene based on the time elapsed since last update
void Update(float secondsElapsed) {
    const GLfloat degreesPerSecond = 90.0f;
    gDegreesRotated += secondsElapsed * degreesPerSecond;
    while(gDegreesRotated > 360.0f) gDegreesRotated -= 360.0f;
}

// the program starts here
int main(int argc, char *argv[]) {
    // initialise mesh
    TetMesh m(0,0,0);
    m.read_vtk_file("resources/Lref.vtk");

    GLFWwindow* window;
    glfwSetErrorCallback(error_callback);
    if (!glfwInit()) return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4); 
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(windowWidth, windowHeight, "Mesh", NULL, NULL);
    if (!window) { glfwTerminate(); return -1; }
    glfwMakeContextCurrent(window);
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) { std::cerr<<"GLEW Init Failed : %s"<<std::endl; }
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);

    // print out some info about the graphics drivers
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
    std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;

    // OpenGL settings
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);

    // load vertex and fragment shaders into opengl
    LoadShaders();

    glPolygonMode( GL_FRONT_AND_BACK, GL_LINE );

    // create buffer and fill it with the points of the triangle
    const double* points = m.get_points();
    const GLuint num_points = m.get_num_points();
    const GLuint* indices = m.get_indices();
    const GLuint num_indices = m.get_num_indices();
    LoadMesh(points, num_points, indices, num_indices);

    // run while the window is open
    double lastTime = glfwGetTime();
    while (glfwWindowShouldClose(window) == 0)
    {
        // update the scene based on the time elapsed since last update
        double thisTime = glfwGetTime();
        Update(thisTime - lastTime);
        lastTime = thisTime;

        Render(num_points, (GLvoid*)indices, num_indices); // draw one frame

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // clean up and exit
    delete points;
    delete indices;
    glfwTerminate();
    return EXIT_SUCCESS;
}