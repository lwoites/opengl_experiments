#ifndef FRAMESOURCE_H
#define FRAMESOURCE_H

#include <string>
#include <vector>
#include <highgui.h>

#include <GL/glew.h>
#include <GL/gl.h>

class t_vertex
{
public:
    t_vertex(float x, float y, float tex_x, float tex_y) :
        x(x), y(y), tex_x(tex_x), tex_y(tex_y){};
public:
    float x;
    float y;
    float tex_x;
    float tex_y;
};

class FrameSource
{
public:
    FrameSource(unsigned int id, std::string filename, float x, float y, float w, float h);
    ~FrameSource();
    int draw(int vbo_offset);
    unsigned int width();
    unsigned int height();
    GLuint get_tex_unit();
    std::vector<t_vertex> get_vertices();
    
public:
    unsigned int id;
    std::string filename;
    float x;
    float y;
    float w;
    float h;

private:
    GLuint _create_tex_object();
    
    cv::VideoCapture *_capturer;
    GLuint _texture;
    GLuint _tex_unit;
};

#endif // FRAMESOURCE_H
