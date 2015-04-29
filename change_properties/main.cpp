#define GLEW_STATIC
#include <GL/glew.h>

#include <GLFW/glfw3.h>
#include <cstdio>
#include <cstdlib>
#include <unistd.h>
#include <string>
#include <iostream>
#include <FreeImage.h>
#include <chrono>
#include <ctime>



using namespace std;
using namespace std::chrono;

const char* VERTEX_SHADER_FILE = "vertex_shader.vert";
const char* FRAGMENT_SHADER_FILE = "fragment_shader.frag";

char* err2str(GLenum error)
{
    switch(error) {
        case GL_NO_ERROR:
            return "GL_NO_ERROR";
        case GL_INVALID_ENUM:
            return "GL_INVALID_ENUM";
        case GL_INVALID_VALUE:
            return "GL_INVALID_VALUE";
        case GL_INVALID_OPERATION:
            return "GL_INVALID_OPERATION";
        case GL_INVALID_FRAMEBUFFER_OPERATION:
            return "GL_INVALID_FRAMEBUFFER_OPERATION";
        case GL_OUT_OF_MEMORY:
            return "GL_OUT_OF_MEMORY";
        default:
            return "Valor de error no reconocido";
    }
}

char* load_shader(const char *shader_file)
{
    char *buffer = NULL;
    FILE *shader_src = fopen(shader_file, "r");

    if (shader_src != NULL){
        // obtain file size:
        fseek (shader_src, 0 , SEEK_END);
        long lSize = ftell(shader_src);
        fseek(shader_src, 0, SEEK_SET);
        
        buffer = (char*) malloc(sizeof(char) * (lSize + 1));
        size_t readed = fread (buffer, 1, lSize, shader_src);
        if (readed != lSize) {
            cout << "Reading error while reading '" << shader_file << "'" << endl;
        }
        buffer[lSize] = 0;
        fclose(shader_src);    
    } else {
        cout << "cannot open file '" << shader_file << "'" << endl;;
    }
    return buffer;
}

GLuint compile_shader(GLenum shader_type, char *shader_src) {
    // Create and compile the vertex shader
    GLuint shader = glCreateShader(shader_type);
    glShaderSource(shader, 1, &shader_src, NULL);
    glCompileShader(shader);
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE){
        GLint log_length = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &log_length);
        char *shader_log = (char*) malloc(log_length);
        glGetShaderInfoLog(shader, log_length, NULL, shader_log);
        cout << "shader has errors" << endl;
        cout << shader_log << endl;
        free(shader_log);
        return 0;
    }
    return shader;
}


void update_vbo(
    GLuint vbo,
    float xoffset, float yoffset ,float zoffset, float alphaoffset,
    float xoffset_2, float yoffset_2 ,float zoffset_2, float alphaoffset_2
) {
    GLfloat vertices[] = {
    //      position          color    
        -1.0f + xoffset, 0.0f + yoffset, 0.0 + zoffset,    1.0, 0.0, 0.0, 1.0 + alphaoffset,
         0.0f + xoffset, 1.0f + yoffset, 0.0 + zoffset,    1.0, 0.0, 0.0, 1.0 + alphaoffset,
         1.0f + xoffset, 0.0f + yoffset, 0.0 + zoffset,    1.0, 0.0, 0.0, 1.0 + alphaoffset,
         
        -1.0f + xoffset_2, -1.0f + yoffset_2, 0.0 + zoffset_2,  0.0, 1.0, 0.0, 1.0 + alphaoffset_2,
         0.0f + xoffset_2,  0.0f + yoffset_2, 0.0 + zoffset_2,  0.0, 1.0, 0.0, 1.0 + alphaoffset_2,
         1.0f + xoffset_2, -1.0f + yoffset_2, 0.0 + zoffset_2,  0.0, 1.0, 0.0, 1.0 + alphaoffset_2
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
}



int main(int argc, char **argv)
{

    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(1280, 720, "OpenGL", NULL, NULL); // Windowed
    //GLFWwindow* window = glfwCreateWindow(800, 600, "OpenGL", glfwGetPrimaryMonitor(), NULL); // full-screen

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    glewInit();

    glEnable(GL_DEPTH_TEST);
    glEnable (GL_BLEND);
    glBlendFunc (GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    
    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {
    //      position          color    
        -1.0f, 0.0f, 0.0,    1.0, 0.0, 0.0, 1.0,
         0.0f, 1.0f, 0.0,    1.0, 0.0, 0.0, 1.0,
         1.0f, 0.0f, 0.0,    1.0, 0.0, 0.0, 1.0,
         
         -1.0f, -1.0f, 0.0,  0.0, 1.0, 0.0, 1.0,
          0.0f,  0.0f, 0.0,  0.0, 1.0, 0.0, 1.0,
          1.0f, -1.0f, 0.0,  0.0, 1.0, 0.0, 1.0
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    char *vertexSource = load_shader(VERTEX_SHADER_FILE);
    char *fragmentSource = load_shader(FRAGMENT_SHADER_FILE);

    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 or fragmentShader == 0) {
        exit(1);
    }

   // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // Specify the layout of the vertex data
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
    // binds color input for vertex shader
    GLint colorAttrib = glGetAttribLocation(shaderProgram, "vcolor");
    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib, 4, GL_FLOAT, GL_FALSE, 7 * sizeof(float),
                          (void*) (3 * sizeof(float)));
  
    int index = -1;
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int frames = 0;
    //const double RENDER_RATE_MS = 1000000.0 / 23.976;
    const long int RENDER_RATE_MS = 41708375;

    chrono::nanoseconds zero (0);
    chrono::nanoseconds one_second (1000000000l);
    chrono::high_resolution_clock::time_point begin; //= std::chrono::high_resolution_clock::now();
    //chrono::nanoseconds end; //auto end = std::chrono::high_resolution_clock::now();
    chrono::nanoseconds time_elapsed; //duration_cast<std::chrono::nanoseconds>(end-begin).count() 
    chrono::nanoseconds time_to_prepare_frame; //duration_cast<std::chrono::nanoseconds>(end-begin).count() 
    const chrono::nanoseconds time_to_render(RENDER_RATE_MS); 
    chrono::nanoseconds total_time(0); 
    timespec to_sleep = {0,0};
   

    GLenum lerror;
    float xoffset = 0.0; 
    float yoffset = 0.0;
    float zoffset = 0.0;
    float alphaoffset = 0.0;
    
    float xoffset_2 = 0.0; 
    float yoffset_2 = 0.0;
    float zoffset_2 = 0.0;
    float alphaoffset_2 = 0.0;
    bool do_update = true;
    while(!glfwWindowShouldClose(window))
    {

        do_update = true;
        //begin = chrono::high_resolution_clock::now();
        begin = chrono::high_resolution_clock::now();
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
            xoffset -= 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
            xoffset += 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
            yoffset += 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
            yoffset -= 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){
            zoffset -= 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS){
            zoffset += 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
            alphaoffset -= 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
            alphaoffset += 0.01;
            
        } else if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS){
            xoffset_2 -= 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){
            xoffset_2 += 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS){
            yoffset_2 += 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS){
            yoffset_2 -= 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS){
            zoffset_2 -= 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS){
            zoffset_2 += 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS){
            alphaoffset_2 -= 0.01;
        } else if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS){
            alphaoffset_2 += 0.01;
            
        } else {
            do_update = false;
        }

        
        glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (do_update)
            update_vbo(vbo,
                xoffset, yoffset , zoffset, alphaoffset,
                xoffset_2, yoffset_2 , zoffset_2, alphaoffset_2
            );
        glDrawArrays(GL_TRIANGLES, 0, 6);

        time_to_prepare_frame = duration_cast<std::chrono::nanoseconds>(chrono::high_resolution_clock::now() - begin);//.count();

        time_elapsed = duration_cast<std::chrono::nanoseconds>(chrono::high_resolution_clock::now() - begin);//.count();

        while (time_elapsed < time_to_render){
          to_sleep.tv_nsec = ((time_to_render - time_elapsed).count()) / 2;
          //to_sleep.tv_nsec = ((time_to_render - time_elapsed).count());
          nanosleep(&to_sleep, NULL);
          time_elapsed = duration_cast<std::chrono::nanoseconds>(chrono::high_resolution_clock::now() - begin);//.count();
        }
        glfwSwapBuffers(window);
        ++frames;
        total_time += time_elapsed;
        if (total_time >= one_second){
            cout << frames << "fps" << endl;
            frames = 0;
            total_time = zero;
        }

    }

    glDeleteProgram(shaderProgram);
    glDeleteShader(fragmentShader);
    glDeleteShader(vertexShader);

    glDeleteBuffers(1, &vbo);
    glDeleteVertexArrays(1, &vao);

    free(vertexSource);
    free(fragmentSource);
    glfwTerminate();
    return 0;
}

