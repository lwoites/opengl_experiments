#include "framesource.h"
#include <cv.h>
#include <highgui.h>
#include <GL/glew.h>
#include <GL/gl.h>
#include <GL/glut.h>

#include "gl_utils.h"

#include <string>

using namespace std;

FrameSource::FrameSource(unsigned int id, string filename, float x, float y, float w, float h) :
    id(id), filename(filename), x(x), y(y), w(w), h(h)
{
    _capturer = new cv::VideoCapture(filename);
    if(!_capturer->isOpened()){
        cout << "cannot open file '" << filename << "'" << endl;
    }
    _texture = _create_tex_object();
}

FrameSource::~FrameSource()
{
    _capturer->release();
    glDeleteTextures(1, &_texture);
}

int FrameSource::draw(int vbo_offset)
{
    cv::Mat current_frame;
    (*_capturer) >> current_frame;
    
//     value = GL_TEXTURE0 + t;
//     glUniform1i(uniform, t);
//     //glActiveTexture(value);  // con o sin el active funciona igual
    
    GLenum lerror;
    glBindTexture(GL_TEXTURE_2D, _texture);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width(), height(), GL_BGR, GL_UNSIGNED_BYTE, current_frame.data);
    lerror = glGetError();
    if (lerror != GL_NO_ERROR)
        cout << "hubo un error seteando la textura" << err2str(lerror) << endl;
    
    glDrawArrays(GL_TRIANGLES, vbo_offset, 6);
    lerror = glGetError();
    if (lerror != GL_NO_ERROR)
        cout << "hubo un error dibujando los triangulos:" << err2str(lerror) << endl;
    
    return 6;
}

vector<t_vertex> FrameSource::get_vertices()
{
    
    vector <t_vertex> result;
    result.push_back(t_vertex(x, y, 0.0, 0.0));
    result.push_back(t_vertex(x + w, y, 1.0, 0.0));
    result.push_back(t_vertex(x + w, y - h, 1.0, 1.0));
    result.push_back(t_vertex(x + w, y - h, 1.0, 1.0));
    result.push_back(t_vertex(x, y - h, 0.0, 1.0));
    result.push_back(t_vertex(x, y, 0.0, 0.0));
    
    return result;
}

GLuint FrameSource::get_tex_unit()
{
    return id;
}


unsigned int FrameSource::width()
{
    return _capturer->get(CV_CAP_PROP_FRAME_WIDTH);
}

unsigned int FrameSource::height()
{
    return _capturer->get(CV_CAP_PROP_FRAME_HEIGHT);
}

GLuint FrameSource::_create_tex_object()
{
    GLuint texture;
    glGenTextures(1, &texture);

    glActiveTexture(GL_TEXTURE0 + id);
    glBindTexture(GL_TEXTURE_2D, texture);
        
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
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width(), height());
    
    return texture;
}


    