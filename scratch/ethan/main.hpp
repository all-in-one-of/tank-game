#if !defined(MAIN_HPP)
#define MAIN_HPP

/* Include C++ standard headers */
#include <stdexcept>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <vector>
#include <array>
#include <locale>

/* Include C standard headers */
#include <cstdio>
#include <cstring>
//#include <unistd.h>
//#include <GL/glut.h>
//#include <glut.h>
//#include "texture.h"

//#include <unistd.h>
//#include <X11/Xlib.h>
//#include <X11/Xutil.h>

#include <GL/glut.h>
//#include <GL/gl.h>
//#include <GL/glu.h>


/* Include my headers */

#include "utils/exceptions.hpp"
#include "cg/objparser.hpp"
#include "cg/image.hpp"
#include "utils/system.hpp"

/* Global vars */


namespace ede
{
const unsigned int BUFFER_MODE = GLUT_DOUBLE;
const int XCONST = 768, YCONST = 768;

typedef std::vector< std::shared_ptr<cg::trimesh> > mesh_ptr_vec;
mesh_ptr_vec meshvec;

std::vector<cg::trimesh> meshlist;

/* report GL errors, if any, to stderr */
//void checkError(const char *functionName)
void checkGlError(const std::string &functionName)
{
    GLenum error;
    while (( error = glGetError() ) != GL_NO_ERROR) {
        std::cerr << "Run function: " << functionName << "\n" ;
        std::cerr << "Detected GL error 0x";
        std::cerr << std::hex << error << std::dec << "\n";
        //fprintf (stderr, "GL error 0x%X detected in %s\n", error, functionName);
    }
    std::cerr << std::flush;
}

std::string load_file_as_str(const std::string &fname)
{
        std::ifstream file;
        file.exceptions(std::ifstream::failbit);

        try
        {
            file.open(fname.c_str());
        }
        catch(std::ios_base::failure &e)
        {
            std::cerr << "When opening file \"" << fname;
            std::cerr << "\" the following exception occured: ";
            std::cerr << e.what() << std::endl;

            throw e;
        }
 
        std::stringstream fileData;
        fileData << file.rdbuf();
        file.close();
 
        return fileData.str();
}


GLuint loadGLTexture(const char *filename)
{
    cg::image::bitmap image = cg::image::loadPPM(filename);

    // Bind to gl state machine
    GLuint textureID = 0; //OpenGL texture ID to be returned

    glGenTextures(1, &textureID);

    std::cerr << "Generated texture ID: " << textureID << std::endl;

    glBindTexture(GL_TEXTURE_2D, textureID);

    //Set texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	//Ignore surface color
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

    //Define the texture
    glTexImage2D(GL_TEXTURE_2D, 0, 4, image.width, image.height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image.raw());
    //Create mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, image.width, image.height, GL_RGBA, GL_UNSIGNED_BYTE, image.raw());

    // PPMs come in upside down, so correct this.
    glMatrixMode(GL_TEXTURE);
    glLoadIdentity();
    glScaled(1.0,-1.0,1.0);
    glTranslated(0.0,1.0,0.0);

    return textureID;
}

void draw_meshes(const mesh_ptr_vec &mv)
{
    for(auto iter = mv.cbegin(); iter != mv.cend(); ++iter)
    {
        auto iteri = *iter;

        glBegin(GL_TRIANGLES);
        for(auto iterj = iteri->tris.cbegin(); iterj != iteri->tris.cend(); ++iterj)
        {
            // first texture coordinate position
            glTexCoord2f( iteri->uvs.at(iterj->tex[0]).x,
                          iteri->uvs.at(iterj->tex[0]).y);
            // first vertex position
            glVertex3f( iteri->pts.at(iterj->pos[0]).x,
                        iteri->pts.at(iterj->pos[0]).y,
                        iteri->pts.at(iterj->pos[0]).z );

            // second texture coordinate position
            glTexCoord2f( iteri->uvs.at(iterj->tex[1]).x,
                          iteri->uvs.at(iterj->tex[1]).y);
            // second vertex position
            glVertex3f( iteri->pts.at(iterj->pos[1]).x,
                        iteri->pts.at(iterj->pos[1]).y,
                        iteri->pts.at(iterj->pos[1]).z );

            // third texture coordinate position
            glTexCoord2f( iteri->uvs.at(iterj->tex[2]).x,
                          iteri->uvs.at(iterj->tex[2]).y);
            // third vertex position
            glVertex3f( iteri->pts.at(iterj->pos[2]).x,
                        iteri->pts.at(iterj->pos[2]).y,
                        iteri->pts.at(iterj->pos[2]).z );
        }
        glEnd();
    }
}

void draw()
{
    //glMatrixMode(GL_PROJECTION);
    //glLoadIdentity();


    //glTranslated(0.0,-0.0,1.0);
    //glMatrixMode(GL_MODELVIEW);
    //glLoadIdentity();
    //glScaled(-3.0,3.0,1.0);
    draw_meshes(meshvec);

    std::unique_ptr<double[]> d(new double[4]);

    glGetClipPlane(GL_CLIP_PLANE0, d.get());

    for(int i = 0; i < 4; ++i)
    {
        std::clog << d[i] << " ";
    }
    std::clog << std::endl;
}

void display(void)
{
    glClear (GL_COLOR_BUFFER_BIT);

    //glClearColor ( 1, 0.5, 0.0, 1 );
    glClearColor ( 0.0, 0.0, 0.0, 1 );
    glClear ( GL_COLOR_BUFFER_BIT );
    draw ();
    glFlush ();

    if(BUFFER_MODE == GLUT_DOUBLE)
        glutSwapBuffers ();

    checkGlError ("display");
}

void idle(void)
{
    sleep(1); // SLEEP!
    checkGlError ("idle");
}

void reshape(int x, int y)
{
    glutReshapeWindow(XCONST, YCONST);
    checkGlError ("reshape");
}

void load_meshes(void)
{
    std::array<std::string, 2> fnames = {{"./geo/xformed_crayon.obj", "./xformed_box.obj"}};

    for(int i = 0; i < 1; ++i)
    {
        auto tm_uptr = cg::objparser::parse_file(fnames[i]);
        std::shared_ptr<cg::trimesh> tm_shared(tm_uptr.release());

        meshvec.push_back(tm_shared);
    }
}

void load_textures(void)
{
}

void initGL(void)
{
    /*
    GLuint texid;

    glGenTextures(1, &texid);

    glBindTexture(GL_TEXTURE_2D, texid);

    GLubyte test[] = {255, 0, 0};

    glTexImage2D(   GL_TEXTURE_2D, //target
                    0, //MIPMAP level
                    GL_RGB, //internal Format
                    1, //width
                    1, //height
                    0, //border width
                    GL_RGB, //format
                    GL_UNSIGNED_BYTE, //type
                    test); //data

    // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    */

    //GLuint tmp = loadPPM("./test.ppm");

    glShadeModel(GL_SMOOTH);
    glEnable(GL_COLOR_MATERIAL);
    glColorMaterial(GL_FRONT, GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_TEXTURE_2D);
    //glClearColor(0.0, 0.0, 0.0, 0.5);
    //glClearDepth(1.0);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);
    glMatrixMode(GL_MODELVIEW);
    //glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);

    load_meshes();
    loadGLTexture("./textures/ParkingLot.ppm");
    //glRotated(180.0, 0.0,0.0,1.0);
   // glTranslated(1.0,1.0,0.0);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(-1.0,1.0,1.0,-1.0,0.01,100000);
    checkGlError("initGL");
}

void keyboard(unsigned char key, int x, int y)
{
    switch ( key )
    {
        case 27:                    // if ASCII escape is hit
            std::cerr << "Escape key pressed. Exiting program." << std::endl;
            throw err::system_exit();    // then exit the program
            break;
        default:                    // Otherwise do nothing
            break;
    }
}

void special(int key, int x, int y)
{
    switch ( key )
    {
        case GLUT_KEY_UP:
            std::cerr << "Up key pressed." << std::endl;
            break;
        case GLUT_KEY_DOWN:
            std::cerr << "Down key pressed." << std::endl;
            break;
        case GLUT_KEY_LEFT:
            std::cerr << "Left key pressed." << std::endl;
            break;
        case GLUT_KEY_RIGHT:
            std::cerr << "Right key pressed." << std::endl;
            break;
        default:                    // Otherwise do nothing
            break;
    }
}

// Since we are inside a namespace the compiler/linker will not complain
// about the function being named main.
int main(int argc, char **argv)
{
    // Set locale US English, UTF-8.
    std::locale::global(std::locale("en_US.utf8"));
    // Force the only pre-existing iostream objects to follow suit. All new 
    // iostream objects will simply inherit the global locale at instantiation.
    std::cout.imbue(std::locale());
    std::cerr.imbue(std::locale());
    std::clog.imbue(std::locale());
    std::cin.imbue(std::locale());

    glutInit(&argc, argv);
    glutInitDisplayMode (BUFFER_MODE | GLUT_RGB);

    //glutInitContextVersion (3, 1);
    //glutInitContextFlags (GLUT_COMPATIBILITY_PROFILE | GLUT_DEBUG);
    //glutInitContextFlags (GLUT_FORWARD_COMPATIBLE | GLUT_CORE_PROFILE | GLUT_DEBUG);

    glutInitWindowSize (XCONST, YCONST); 
    glutInitWindowPosition (0, 0);
    glutCreateWindow ("OpenGL 2.1 context via FreeGLUT");
    //int main_window_id = glutCreateWindow ("OpenGL 2.1 context via FreeGLUT");

    //std::cerr << "Main window ID: " << main_window_id << std::endl;

    // test obj file parser
    // test_obj_parser();

    //dumpInfo ();
    initGL();

    // Register GLUT functions
    glutDisplayFunc(display); 
    glutIdleFunc(idle); 
    glutReshapeFunc(reshape);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(special);

    // Start GLUT main loop
    glutMainLoop();
    return 0;
}

}

#endif // end include guard
