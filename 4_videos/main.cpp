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
#include <opencv2/opencv.hpp>



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


GLuint* init_textures(int size, GLuint shaderProgram){
    GLuint *textures = (GLuint*) (malloc(sizeof(GLuint) * size));
    glGenTextures(size, textures);

    char uniform_name[6];
    for (int i=0; i < size; ++i){
        glActiveTexture(GL_TEXTURE0 + i);
        glBindTexture(GL_TEXTURE_2D, textures[i]);
        
        // configure texture sampling options for x(s) and y (t) greater than 1.0 or lower than 0.0
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        // configure filtering options for scaling down and scaling up the texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 1280, 720, 0, GL_BGR, GL_UNSIGNED_BYTE, NULL);
        // Mejor formato segun opengl.org ya que no necesita conversiones
        //glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1280, 720, 0, GL_BGRA, GL_UNSIGNED_BYTE, NULL);
        //si no usamos mipmaps es mejor crear las texxturas de la siguiente forma
        
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, 1280, 718);
        /*
        sprintf(uniform_name, "tex_%i", i);
        GLuint uniform = glGetUniformLocation(shaderProgram, uniform_name);
        if (uniform == -1)
            cout << "ERROR EN EL UNIFORM NAME '" << uniform_name << "'" << endl;
        else
            glUniform1i(uniform, i);
        */
    }
   return textures;
}

void set_texture(GLuint texture, const unsigned char *raw_image){
    int width = 1280;
    int height = 718;
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_BGR, GL_UNSIGNED_BYTE, raw_image);
}



int main(int argc, char **argv)
{

//     char *vfiles[4];
//     if (argc < 2){
//         cout << "usage " << argv[0] << " <video_file_1> <video_file_2> " << endl;
//        exit(1);
//     }
//     for (int i=1; i < argc; ++i)
//         vfiles[i-1] = argv[i];
//     for (int i=(argc-1); i < 4; ++i)
//         vfiles[i] = argv[1];
    
    char *vfiles[] = {
        "/home/laucha/media/series/twd/The.Walking.Dead.S04E11.720p.WEB-DL.AAC2.0.H.264-Cyphanix.mkv",
        "/home/laucha/media/series/breaking_bad/Breaking.Bad.S02.Season.2.720p.BRrip.x264-VisionX/breaking.bad.s02e01.720p.brrip.x264-visionx.mkv",
        "/home/laucha/media/series/breaking_bad/Breaking.Bad.S02.Season.2.720p.BRrip.x264-VisionX/breaking.bad.s02e02.720p.brrip.x264-visionx.mkv",
        "/home/laucha/media/series/breaking_bad/Breaking.Bad.S02.Season.2.720p.BRrip.x264-VisionX/breaking.bad.s02e03.720p.brrip.x264-visionx.mkv"
    };

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

    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    GLfloat vertices[] = {
    //  position           color      tex coords 
        -1.0f, 1.0f,   1.0, 0.0, 0.0,   0.0, 0.0,   // top-left 
        0.0f, 1.0f,    0.0, 1.0, 0.0,   1.0, 0.0,   // top-right
        0.0f, 0.0f,   0.0, 0.0, 1.0,   1.0, 1.0,   // bottom-right
        0.0f, 0.0f,   0.0, 0.0, 1.0,   1.0, 1.0,   // bottom-right 
        -1.0f, 0.0f,  1.0, 1.0, 1.0,   0.0, 1.0,   // bottom-left
        -1.0f, 1.0f,   1.0, 0.0, 0.0,   0.0, 0.0,    // top-left

        0.0f, 1.0f,   1.0, 0.0, 0.0,   0.0, 0.0,   // top-left 
        1.0f, 1.0f,    0.0, 1.0, 0.0,   1.0, 0.0,   // top-right
        1.0f, 0.0f,   0.0, 0.0, 1.0,   1.0, 1.0,   // bottom-right
        1.0f, 0.0f,   0.0, 0.0, 1.0,   1.0, 1.0,   // bottom-right 
        0.0f, 0.0f,  1.0, 1.0, 1.0,   0.0, 1.0,   // bottom-left
        0.0f, 1.0f,   1.0, 0.0, 0.0,   0.0, 0.0,    // top-left
        
        -1.0f, 0.0f,   1.0, 0.0, 0.0,   0.0, 0.0,   // top-left 
        0.0f, 0.0f,    0.0, 1.0, 0.0,   1.0, 0.0,   // top-right
        0.0f, -1.0f,   0.0, 0.0, 1.0,   1.0, 1.0,   // bottom-right
        0.0f, -1.0f,   0.0, 0.0, 1.0,   1.0, 1.0,   // bottom-right 
        -1.0f, -1.0f,  1.0, 1.0, 1.0,   0.0, 1.0,   // bottom-left
        -1.0f, 0.0f,   1.0, 0.0, 0.0,   0.0, 0.0,    // top-left
        
        0.0f, 0.0f,   1.0, 0.0, 0.0,   0.0, 0.0,   // top-left 
        1.0f, 0.0f,    0.0, 1.0, 0.0,   1.0, 0.0,   // top-right
        1.0f, -1.0f,   0.0, 0.0, 1.0,   1.0, 1.0,   // bottom-right
        1.0f, -1.0f,   0.0, 0.0, 1.0,   1.0, 1.0,   // bottom-right 
        0.0f, -1.0f,  1.0, 1.0, 1.0,   0.0, 1.0,   // bottom-left
        0.0f, 0.0f,   1.0, 0.0, 0.0,   0.0, 0.0    // top-left
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
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), 0);
    // binds color input for vertex shader
    GLint colorAttrib = glGetAttribLocation(shaderProgram, "vcolor");
    glEnableVertexAttribArray(colorAttrib);
    glVertexAttribPointer(colorAttrib, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (2 * sizeof(float)));
    // binds tex coord input for vertex shader
    GLint texcoordAttrib = glGetAttribLocation(shaderProgram, "vtexcoord");
    glEnableVertexAttribArray(texcoordAttrib);
    glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(float), (void*) (5 * sizeof(float)));

    GLuint *textures = init_textures(4, shaderProgram);
  
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
   

    cv::VideoCapture *capturers[4];
    for (int i=0; i < 4; ++i){
        capturers[i] = new cv::VideoCapture(vfiles[i]); 
        if(!capturers[i]->isOpened()){
            cout << "cannot open file '" << vfiles[i] << "'" << endl;
            exit(1);
        }
    }

    GLuint uniform = glGetUniformLocation(shaderProgram, "tex_0");
    if (uniform == -1)
        cout << "ERROR EN EL UNIFORM NAME '" << "tex_0" << "'" << endl;
    GLenum lerror;
    while(!glfwWindowShouldClose(window))
    {


        //begin = chrono::high_resolution_clock::now();
        begin = chrono::high_resolution_clock::now();
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, GL_TRUE);
        }

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        
        GLint value;
        for (GLint t=0; t < 4; ++t){
            cv::Mat current_frame;
            *capturers[t] >> current_frame;
            value = GL_TEXTURE0 + t;
            glUniform1i(uniform, t);
            glActiveTexture(value);  // con o sin el active funciona igual
            
            lerror = glGetError();
            if (lerror != GL_NO_ERROR)
                cout << "hubo un error seteando el uniform:" << err2str(lerror) << endl;
            
            set_texture(textures[t], (unsigned char*) current_frame.data);
            lerror = glGetError();
            if (lerror != GL_NO_ERROR)
                cout << "hubo un error seteando la textura" << err2str(lerror) << endl;
            
            glDrawArrays(GL_TRIANGLES, t*6, 6);
            lerror = glGetError();
            if (lerror != GL_NO_ERROR)
                cout << "hubo un error dibujando los triangulos:" << err2str(lerror) << endl;
        }

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
    glDeleteTextures(4, textures);

    glDeleteVertexArrays(1, &vao);

    free(vertexSource);
    free(fragmentSource);
    // TODO: free textures
    glfwTerminate();
    return 0;
}

