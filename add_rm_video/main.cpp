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
//#include <opencv2/opencv.hpp>

#include <vector>
#include <algorithm>
#include "framesource.h"
#include "gl_utils.h"


using namespace std;
using namespace std::chrono;

const char* VERTEX_SHADER_FILE = "vertex_shader.vert";
const char* FRAGMENT_SHADER_FILE = "fragment_shader.frag";

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
        long readed = fread (buffer, 1, lSize, shader_src);
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

GLFWwindow* init_window_system(int w_width, int w_height)
{
    glfwInit();

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);

    GLFWwindow* window = glfwCreateWindow(w_width, w_height, "OpenGL", NULL, NULL);
    glfwMakeContextCurrent(window);
    
    return window;
}

void init_glew()
{
    glewExperimental = GL_TRUE;
    glewInit();
}    


bool init_opengl(GLint *shader_program, GLint *vertex_shader, GLint *fragment_shader,
    GLuint *vao_, GLuint *vbo_)
{
    // Create Vertex Array Object
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // Create a Vertex Buffer Object and copy the vertex data to it
    GLuint vbo;
    glGenBuffers(1, &vbo);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    //glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    
    char *vertexSource = load_shader(VERTEX_SHADER_FILE);
    char *fragmentSource = load_shader(FRAGMENT_SHADER_FILE);

    GLuint vertexShader = compile_shader(GL_VERTEX_SHADER, vertexSource);
    GLuint fragmentShader = compile_shader(GL_FRAGMENT_SHADER, fragmentSource);
    if (vertexShader == 0 or fragmentShader == 0) {
        return false;
    }

   // Link the vertex and fragment shader into a shader program
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glBindFragDataLocation(shaderProgram, 0, "outColor");
    glLinkProgram(shaderProgram);
    glUseProgram(shaderProgram);

    // binds pos coord input for vertex shader
    GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
    glEnableVertexAttribArray(posAttrib);
    glVertexAttribPointer(posAttrib, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);
  
    // binds tex coord input for vertex shader
    GLint texcoordAttrib = glGetAttribLocation(shaderProgram, "vtexcoord");
    glEnableVertexAttribArray(texcoordAttrib);
    glVertexAttribPointer(texcoordAttrib, 2, GL_FLOAT, GL_FALSE,
                          4 * sizeof(float), (void*) (2 * sizeof(float)));   
    
    free(vertexSource);
    free(fragmentSource);
    
    *shader_program = shaderProgram;
    *vertex_shader = vertexShader;
    *fragment_shader = fragmentShader;
    *vao_ = vao;
    *vbo_ = vbo;
    return true;
}


vector<FrameSource*> create_sources()
{
//    string vfiles[] = {
//        string("/home/laucha/media/videos/video_0_720.mkv"),
//        string("/home/laucha/media/videos/video_1_720.mkv"),
//        string("/home/laucha/media/videos/video_2_720.mkv"),
//        string("/home/laucha/media/videos/video_3_720.mkv")
//    };
    
     string vfiles[] = {
         string("/home/laucha/media/series/twd/The.Walking.Dead.S04E11.720p.WEB-DL.AAC2.0.H.264-Cyphanix.mkv"),
         string("/home/laucha/media/series/breaking_bad/Breaking.Bad.S02.Season.2.720p.BRrip.x264-VisionX/breaking.bad.s02e01.720p.brrip.x264-visionx.mkv"),
         string("/home/laucha/media/series/breaking_bad/Breaking.Bad.S02.Season.2.720p.BRrip.x264-VisionX/breaking.bad.s02e02.720p.brrip.x264-visionx.mkv"),
         string("/home/laucha/media/peliculas/Machete Kills (2013) [1080p]/Machete.Kills.2013.1080p.BluRay.x264.YIFY.mp4")
     };
    vector<FrameSource*> sources;
    sources.push_back(new FrameSource(0, vfiles[0], -1.0, 1.0, 1.0, 1.0));
    sources.push_back(new FrameSource(1, vfiles[1],  0.0, 1.0, 1.0, 1.0));
    sources.push_back(new FrameSource(2, vfiles[2], -1.0, 0.0, 1.0, 1.0));    
    sources.push_back(new FrameSource(3, vfiles[3],  0.0, 0.0, 1.0, 1.0));
    return sources;
}

int create_vbo_data(vector<FrameSource*> &sources, float **vbo_data)
{
    vector<FrameSource*>::iterator it;
    int vertices_len = sources.size() * (6 * 4  * sizeof(float));
    float *vertices = (float*) malloc(vertices_len);
    
    int i = 0;
    for (it=sources.begin(); it != sources.end(); ++it) {
        vector<t_vertex> s_vertices = (*it)->get_vertices();
        vector<t_vertex>::iterator v_it;
        for (v_it = s_vertices.begin(); v_it != s_vertices.end(); ++v_it){
            vertices[i] = v_it->x;
            vertices[i + 1] = v_it->y;
            vertices[i + 2] = v_it->tex_x;
            vertices[i + 3] = v_it->tex_y;
            i += 4;
        }
    }
    *vbo_data = vertices;
    return vertices_len;
}

int main(int argc, char **argv)
{   


    GLFWwindow* window = init_window_system(1280, 720);
    init_glew();

    GLint shader_program, vertex_shader, fragment_shader;
    GLuint vao, vbo;
    if (not init_opengl(&shader_program, &vertex_shader, &fragment_shader, &vao, &vbo)) {
        cout << "Cannot initialize opengl" << endl;
        exit(1);
    }
    
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    int frames = 0;
    const long int RENDER_RATE_MS = 41708375;  // 1000000.0 / 23.976;

    chrono::nanoseconds zero (0);
    chrono::nanoseconds one_second (1000000000l);
    chrono::high_resolution_clock::time_point begin; //= std::chrono::high_resolution_clock::now();
    //chrono::nanoseconds end; //auto end = std::chrono::high_resolution_clock::now();
    chrono::nanoseconds time_elapsed; //duration_cast<std::chrono::nanoseconds>(end-begin).count() 
    chrono::nanoseconds time_to_prepare_frame; //duration_cast<std::chrono::nanoseconds>(end-begin).count() 
    const chrono::nanoseconds time_to_render(RENDER_RATE_MS); 
    chrono::nanoseconds total_time(0); 
    timespec to_sleep = {0,0};

    GLuint uniform = glGetUniformLocation(shader_program, "tex_0");
    if (uniform == (GLuint) (-1))
        cout << "ERROR EN EL UNIFORM NAME '" << "tex_0" << "'" << endl;
    GLenum lerror;
    
    vector<FrameSource*> sources = create_sources();
    float *vertices; 
    int vertices_len = create_vbo_data(sources, &vertices);
    glBufferData(GL_ARRAY_BUFFER, vertices_len, vertices, GL_DYNAMIC_DRAW);
    free(vertices);
    
    unsigned int vbo_offset = 0;
    vector<FrameSource*>::iterator it;
    
    vector<FrameSource*> real_sources;
    bool update_vbo_data = false;
    bool added[4] = {false, false, false, false};
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    while(!glfwWindowShouldClose(window))
    {
        begin = chrono::high_resolution_clock::now();
        glfwPollEvents();
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS){
            if (not added[0]){
                real_sources.push_back(sources[0]);
                update_vbo_data = true;
                added[0] = true;
            }
        } else if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            if (added[0]){
                it = find(real_sources.begin(), real_sources.end(), sources[0]);
                if (it != real_sources.end()){
                    real_sources.erase(it);
                    update_vbo_data = true;
                    added[0] = false;
                }
            }
        } else if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS){
            if (not added[1]){
                real_sources.push_back(sources[1]);
                update_vbo_data = true;
                added[1] =  true;
            }
        } else if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            if (added[1]){
                it = find(real_sources.begin(), real_sources.end(), sources[1]);
                if (it != real_sources.end()) {
                    real_sources.erase(it);
                    update_vbo_data = true;
                    added[1] = false;
                }
            }
        } else if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS){
            if (not added[2]){
                real_sources.push_back(sources[2]);
                update_vbo_data = true;
                added[2] = true;
            }
        } else if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
            if (added[2]){
                it = find(real_sources.begin(), real_sources.end(), sources[2]);
                if (it != real_sources.end()) {
                    real_sources.erase(it);
                    update_vbo_data = true;
                    added[2] = false;
                }
            }
        } else if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS){
            if (not added[3]){
                real_sources.push_back(sources[3]);
                update_vbo_data = true;
                added[3] = true;
            }
        } else if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
            if (added[3]){
                it = find(real_sources.begin(), real_sources.end(), sources[3]);
                if (it != real_sources.end()) {
                    real_sources.erase(it);
                    update_vbo_data = true;
                    added[3] = false;
                }
            }
        } else if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS){
            real_sources.clear();
            update_vbo_data = true;
            for (int i=0; i < 4; ++i)
                added[i] = false;
        }
        if (update_vbo_data) {
            vertices_len = create_vbo_data(real_sources, &vertices);
            glBufferData(GL_ARRAY_BUFFER, vertices_len, vertices, GL_DYNAMIC_DRAW);
            free(vertices);
            update_vbo_data = false;  
        }

        // Clear the screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);   
        
        vbo_offset = 0;
        for (it=real_sources.begin(); it != real_sources.end(); ++it){
            glUniform1i(uniform, (*it)->get_tex_unit());
            lerror = glGetError();
            if (lerror != GL_NO_ERROR)
                cout << "hubo un error seteando la textura" << err2str(lerror) << endl;
            vbo_offset += (*it)->draw(vbo_offset);
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

    glDeleteProgram(shader_program);
    glDeleteShader(fragment_shader);
    glDeleteShader(vertex_shader);

    glDeleteBuffers(1, &vbo);
    for (it=sources.begin(); it != sources.end(); ++it)
        delete *it;

    glDeleteVertexArrays(1, &vao);

    
    glfwTerminate();
    return 0;
}

