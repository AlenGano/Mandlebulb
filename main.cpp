#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include "glad/glad.h"
#include <GLFW/glfw3.h>
#include <iostream>
#include <cmath>

// initialize constants such as screen dimensions and the actual glfw window
const float WIDTH = 1000.0;
const float HEIGHT = 1000.0;
float zoom = 0.0;
int max_iter = 50;
GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "window", NULL, NULL);
float angle = 55.0;


// option for escaping out of glfw window (by pressing escape)

  void processInput(GLFWwindow *window)
{
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    
}


//basic vertex shader

const char *vertexShaderSource ="#version 460 core\n"
    "layout (location = 0) in vec3 aPos;\n" 
    "void main()\n"
    "{\n"
    "   gl_Position = vec4(aPos, 1.0);\n"
    "}\0";




const char *fragmentShaderSource = R"(


// input important matrices and dynamic variables such as 'work' into the fragment shader
#version 460 core
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 rotation_matrix;
out vec4 FragColor;
float angle = 55.0;
uniform float work;
float WIDTH = 1000.0;
float HEIGHT = 1000.0;


// the actual mandlebulb set algorithm using spherical coordinates

float get_iter(vec3 p, float power) {
    int iter = 0;
    int max_iter = 100;
    float x = p.x;
    float y = p.y;
    float z = p.z;
    float dr = 1.0;
    float r = 0.0;
    float n = power + 0.50 * work;

    while (iter < max_iter) {
        r = sqrt(x * x + y * y + z * z);
        if (r > 2.0)
            break;

        // Convert to polar coordinates
        float theta = acos(z / r);
        float phi = atan(y, x);

        // Update dr
        dr = pow(r, n - 1.0) * n * dr + 1.0;

        // Scale and rotate the point
        float zr = pow(r, n);
        theta = theta * n;
        phi = phi * n;

        // Convert back to cartesian coordinates
        float sinTheta = sin(theta);
        float zeta_x = zr * sinTheta * cos(phi);
        float zeta_y = zr * sinTheta * sin(phi);
        float zeta_z = zr * cos(theta);

        // Update position
        x = zeta_x + p.x;
        y = zeta_y + p.y;
        z = zeta_z + p.z;

        iter++;
    }

    return 0.5 * log(r) * r / dr;
}


void main() {

    // initialize object struct containing several constant variables for diffuse lighting calculations 

   struct Object {
    vec3 ambient;
    float light_color;
   };

    Object object_one;
    object_one.ambient = vec3(0.05, 0.05, 0.05);
    object_one.light_color = 0.20;

    // the object rotation

    mat3 rotation = mat3(cos(work), -sin(-work), 0.0, sin(work), cos(-work), 0.0, 0.0, 0.0, 1.0);
    vec2 frags = vec2(gl_FragCoord.x / WIDTH, gl_FragCoord.y / HEIGHT) * 2.1 - 1.0;
    vec3 cameraPos = rotation * vec3(-1.0, 2.0, -3.0);
    vec3 cam = normalize(-cameraPos);
    vec3 c_up = vec3(0.0, 1.0, 0.0);
    vec3 c_x = cross(cam, c_up);
    vec3 r_d = normalize(cam + frags.x * c_x + frags.y * c_up);


    // the ray marching algorithm


    float total = 0.0;
    const int max_iters = 100;
    float dist = 0.0;
    float n = 5.0;
    float epsilon = 0.001;
    vec3 pos = cameraPos;
    int i;
    for (i = 0; i < max_iters; i++) {
        dist = get_iter(pos, n);
        total += dist;
        pos += dist * r_d;
        if (dist < epsilon)
            break;

        if (total > 100.0)
            break;
    }

    // use the iterations to color the object

    float intensity = float(i) / 256.0;
    float R = intensity * 1.9;
    float G = intensity * 1.6;
    float B = intensity * 1.3;
    

    // diffuse lighting calculations

    vec3 normal = pos; 
    vec3 norm = normalize(normal); 

    vec3 LightPos = cameraPos; 
    vec3 LightDir = normalize(LightPos - pos); 

    float diff = max(dot(norm, LightDir), 0.0); 
    vec3 diffuse = diff * object_one.light_color * vec3(1.0, 1.0, 1.0); 

    vec4 result = vec4((object_one.ambient + diffuse) * 10, 1.0); 


    FragColor = result * model * vec4(R, G, B, 1.0);


    
}
)";

// this shader code is for a mandlebrot set, felt like keeping it in 


// const char *fragmentShaderSource = "#version 460 core\n"
// "in vec4 gl_FragCoord;\n"
// "out vec4 FragColor;\n"
// "\n"
// "uniform int WIDTH;\n"
// "uniform int HEIGHT;\n"
// "uniform int max_iter;\n"
// "\n"
// "int get_iterator() {\n"
// "    int iter = 0;\n"
// "    int zoom = 1;\n"
// "    float x = gl_FragCoord.x;\n"
// "    float y = gl_FragCoord.y;\n"
// "    float real = ((x / 1000.0) - 0.5) *2;\n"
// "    float imaginary = ((y / 1000.0) - 0.5) *2;\n"
// "\n"
// "    while (iter < 100.0) {\n"
// "        float real_comp = (real * real) - (imaginary * imaginary);\n"
// "        float imagine_comp = 2.0 * real * imaginary;\n"
// "        real = real_comp;\n"
// "        imaginary = imagine_comp;\n"
// "        float dist = (real * real) + (imaginary * imaginary);\n"
// "\n"
// "        if (dist > 4.0)\n"
// "            break;\n"
// "\n"
// "        iter++;\n"
// "    }\n"
// "\n"
// "    return iter;\n"
// "}\n"
// "\n"
// "vec4 coloring() {\n"
// "    int color = get_iterator();\n"
// "    float scale = float(color) / float(max_iter);\n"
// "    if (color == 99.0) {\n"
// "        gl_FragDepth = 0.0f;\n"        
// "        return vec4(scale, scale, 0.0, 1.0);\n"
// "    }\n"
// "\n"
// "    return vec4(0, scale, 1.0, 1.0);\n"
// "}\n"
// "\n"
// "void main() {\n"
// "    FragColor = coloring();\n"
// "}\0";

int main() {

    // initializes GLFW 
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "window", NULL, NULL);
    
    
    glfwMakeContextCurrent(window);

     if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        glfwTerminate();
        return -1;
    }

    // intialize glfw window and the vertex/fragment shaders, linking them together

    glViewport(0, 0, WIDTH, HEIGHT);
    // create Vertex Shader

    unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    // create Fragment Shader

    unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    // link shaders

    unsigned int ShaderProgram = glCreateProgram();
    glAttachShader(ShaderProgram, vertexShader);
    glAttachShader(ShaderProgram, fragmentShader);
    glLinkProgram(ShaderProgram);

    // vertices variable for the actual object, where the mandlebulb is contained

    float vertices[] = {
    // Front face
    -0.8f, -0.8f,  0.8f,
     0.8f, -0.8f,  0.8f,
     0.8f,  0.8f,  0.8f,
     0.8f,  0.8f,  0.8f,
    -0.8f,  0.8f,  0.8f,
    -0.8f, -0.8f,  0.8f,

    // Back face
    -0.8f, -0.8f, -0.8f,
     0.8f, -0.8f, -0.8f,
     0.8f,  0.8f, -0.8f,
     0.8f,  0.8f, -0.8f,
    -0.8f,  0.8f, -0.8f,
    -0.8f, -0.8f, -0.8f,

    // Left face
    -0.8f,  0.8f,  0.8f,
    -0.8f,  0.8f, -0.8f,
    -0.8f, -0.8f, -0.8f,
    -0.8f, -0.8f, -0.8f,
    -0.8f, -0.8f,  0.8f,
    -0.8f,  0.8f,  0.8f,

    // Right face
     0.8f,  0.8f,  0.8f,
     0.8f,  0.8f, -0.8f,
     0.8f, -0.8f, -0.8f,
     0.8f, -0.8f, -0.8f,
     0.8f, -0.8f,  0.8f,
     0.8f,  0.8f,  0.8f,

    // Top face
    -0.8f,  0.8f, -0.8f,
     0.8f,  0.8f, -0.8f,
     0.8f,  0.8f,  0.8f,
     0.8f,  0.8f,  0.8f,
    -0.8f,  0.8f,  0.8f,
    -0.8f,  0.8f, -0.8f,

    // Bottom face
    -0.8f, -0.8f, -0.8f,
     0.8f, -0.8f, -0.8f,
     0.8f, -0.8f,  0.8f,
     0.8f, -0.8f,  0.8f,
    -0.8f, -0.8f,  0.8f,
    -0.8f, -0.8f, -0.8f
    };
    
    // send vertex data to shaders

    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);


    while (!glfwWindowShouldClose(window)) {

        processInput(window);
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);

        //glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        //glClear(GL_COLOR_BUFFER_BIT);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


        glUseProgram(ShaderProgram);


        // intializes variables and send them to shaders

        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        glm::mat4 view = glm::mat4(1.0f);
        
        
        model = glm::rotate(model, (float)glfwGetTime() * glm::radians(-55.0f) , glm::vec3(0.0f, 1.0f, 0.0f));
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, zoom -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float(WIDTH) / float(HEIGHT)), 0.1f, 100.0f);
        uint modelLoc;
        uint viewLoc;
        uint projLoc;
        uint time;
        time = glGetUniformLocation(ShaderProgram, "work");
        modelLoc = glGetUniformLocation(ShaderProgram, "model");
        viewLoc = glGetUniformLocation(ShaderProgram, "view");
        projLoc = glGetUniformLocation(ShaderProgram, "projection");
        glUniform1f(time, float(glfwGetTime()));
        
        glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(viewLoc, 1 , GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
      
        glBindVertexArray(VAO);
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        glDrawArrays(GL_TRIANGLES, 0, 36);



        glfwPollEvents();
        glfwSwapBuffers(window);

    }
    glfwTerminate();
    return 0;

}
