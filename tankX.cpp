#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glut.h>

#include <iostream>
#include <math.h>
#include <vector>

#include "vec4.h"
#include "mat4.h"
#include "mesh.h"
#include "game_object.h"

#define KEYBOARD_S 115
#define KEYBOARD_ESC 27
#define KEYBOARD_F 102
#define NORMAL_EXIT_GLUT_LOOP "terminating glut"

int windowWidth = 640;
int windowHeight = 480;
bool specialKeys[1000] = {0};

std::vector<mesh> meshes;
std::vector<game_object> characters;

int HERO_ID = 0;
int ENEMY_ID = 1;
int ENVIRONMENT_ID = 2;
GLuint HERO_TEX = 0;
GLuint ENEMY_TEX = 1;
GLuint ENVIRONMENT_TEX = 2;

bool HandleKeyboardInput();
GLvoid InitGL();
GLvoid DrawGLScene();
GLvoid IdleGLScene();
GLvoid ResizeGLScene(int width, int height);
GLvoid GLKeyDown(unsigned char key, int x, int y);
GLvoid SpecialKeys(int key, int x, int y);
GLvoid SpecialKeysUp(int key, int x, int y);
int loadPPM(const char *filename, GLuint textureID);
void DrawObj(mesh& m, mat4& t);

/*
Function taken from www.cs.utexas.edu texture.cpp
*/
int loadPPM(const char *filename, GLuint textureID) {

	//GLuint textureID = 0; 
    FILE *inFile; //File pointer
	char buffer[100]; //Input buffer
    GLubyte *theTexture; //Texture buffer pointer
	unsigned char c; //Input character
	int width, height, maxVal, pixelSize; //Image characteristics from ppm file

	//Try to open the file for reading
	if( (inFile = fopen(filename, "rb")) == NULL) {
		fprintf (stderr, "cannot open %s\n", filename);
		exit(-1);
	}

	//Read file type identifier (magic number)
	fgets(buffer, sizeof(buffer), inFile);
	if ((buffer[0] != 'P') || (buffer[1] != '6')) {
		fprintf (stderr, "not a binary ppm file %s\n", filename);
		exit(-1);
    }

	if(buffer[2] == 'A')
		pixelSize = 4;
	else
		pixelSize = 3;

	//Read image size
	do fgets(buffer, sizeof (buffer), inFile);
	while (buffer[0] == '#');
	sscanf (buffer, "%d %d", &width, &height);

	//Read maximum pixel value (usually 255)
	do fgets (buffer, sizeof (buffer), inFile);
	while (buffer[0] == '#');
	sscanf (buffer, "%d", &maxVal);

	//Allocate RGBA texture buffer
	int memSize = width * height * 4 * sizeof(GLubyte);
	theTexture = (GLubyte *)malloc(memSize);

	// read RGB data and set alpha value
	for (int i = 0; i < memSize; i++) {
		if ((i % 4) < 3 || pixelSize == 4) {
			c = fgetc(inFile);
			theTexture[i]=(GLubyte) c;
        }
		else theTexture[i] = (GLubyte) 255; //Set alpha to opaque
    }
    fclose(inFile);

    glBindTexture(GL_TEXTURE_2D, textureID);

    //Set texture parameters
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	//Ignore surface color
    glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);
    //Define the texture
    glTexImage2D(GL_TEXTURE_2D, 0, 4, (GLuint)width, (GLuint)height, 0, GL_RGBA, GL_UNSIGNED_BYTE, theTexture);

    //Create mipmaps
    gluBuild2DMipmaps(GL_TEXTURE_2D, 4, (GLuint)width, (GLuint)height, GL_RGBA, GL_UNSIGNED_BYTE, theTexture);

    free(theTexture);
    return(textureID);
}

GLvoid InitGL(){
	//All the stuff we need to draw
	std::string hero_geo_file = "geo/car.obj";
	std::string enemy_geo_file = "geo/tire.obj";
	std::string environment_geo_file = "geo/ParkingLot.obj";

	std::string hero_tex_file = "tex/car.ppm";
	std::string enemy_tex_file = "tex/tire.ppm";
	std::string environment_tex_file = "tex/ParkingLot.ppm";

	mesh hero(hero_geo_file);
	mesh enemy(enemy_geo_file);
	mesh environment(environment_geo_file);
	HERO_ID = meshes.size();
	meshes.push_back(hero);
	ENEMY_ID = meshes.size();
	meshes.push_back(enemy);
	ENVIRONMENT_ID = meshes.size();
	meshes.push_back(environment);

	glEnable(GL_TEXTURE_2D);
	glGenTextures(1, &HERO_TEX);
	glGenTextures(1, &ENEMY_TEX);
	glGenTextures(1, &ENVIRONMENT_TEX);
	loadPPM(hero_tex_file.c_str(), HERO_TEX);
	loadPPM(enemy_tex_file.c_str(), ENEMY_TEX);
	loadPPM(environment_tex_file.c_str(), ENVIRONMENT_TEX);

	game_object hero_character(characters.size(), HERO_ID, HERO_TEX);
	characters.push_back(hero_character);
	game_object enemy_character(characters.size(), ENEMY_ID, ENEMY_TEX);
	characters.push_back(enemy_character);
	game_object environment_character(characters.size(), ENVIRONMENT_ID, ENVIRONMENT_TEX);
	characters.push_back(environment_character);

	//GL boilerplate initialization
	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glClearColor(0.5f, 0.5f, 0.5f, 0.5f);				// grey Background
	glClearDepth(1.0f);									// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing				
	
	glEnable(GL_CULL_FACE);
	glShadeModel(GL_SMOOTH);
	glDepthFunc(GL_LEQUAL);								// The Type Of Depth Testing To Do
	glEnable(GL_COLOR_MATERIAL);
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
} 

void DrawObj(mesh& m)
{
	glBegin(GL_TRIANGLES);
    for(int i=0; i<m.faces.size(); i++)
    {
    	vec4 vert = m.verts[m.faces[i].pnts[0]];
    	vec2 text = m.texts[m.faces[i].txts[0]];
    	glTexCoord2f(text.u, text.v);
    	glVertex3f(vert.x,vert.y,vert.z);
    	vert = m.verts[m.faces[i].pnts[1]];
    	text = m.texts[m.faces[i].txts[1]];
    	glTexCoord2f(text.u, text.v);
    	glVertex3f(vert.x,vert.y,vert.z);
    	vert = m.verts[m.faces[i].pnts[2]];
    	text = m.texts[m.faces[i].txts[2]];
    	glTexCoord2f(text.u, text.v);
    	glVertex3f(vert.x,vert.y,vert.z);
    }
	glEnd();	
}

GLvoid DrawGLScene(){

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glLoadIdentity();
    
    glViewport(0, 0, windowWidth, windowHeight);
    glTranslatef(0.0f, 0.0f, -5.0f);

    for(int i=0; i<characters.size(); i++)
    {
    	glBindTexture(GL_TEXTURE_2D, characters[i].tex);
    	DrawObj(meshes[characters[i].geo]);
    }

    glFlush();
	glutSwapBuffers();
}

GLvoid IdleGLScene(){
	if(HandleKeyboardInput())
	{	
		glutPostRedisplay();
	}
}

GLvoid ResizeGLScene(int width, int height){
	if(height == 0)
		height = 1;
	glViewport(0,0,width,height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();

	gluPerspective(45.0f, (GLfloat)width/(GLfloat)height, 0.1f, 2000.0f);

	windowWidth = width;
	windowHeight = height;
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glutPostRedisplay();
}

/*
 * This function handles all normal key presses on the keyboard. If you need
 * to capture special keys like, ctrl, shift, F1, F2, F..., or arrow keys use
 * the special keys function
 */
GLvoid GLKeyDown(unsigned char key, int x, int y){
	if(key=='q')
	{
		
	}
	if(key=='e')
	{
		
	}
	if(key=='w')
	{
		
	}
	if(key=='s')
	{
		
	}
	if(key=='a')
	{
		
	}
	if(key=='d')
	{
		
	}
}

/*
 * These are the special keys as is set apart by glut
 */
GLvoid SpecialKeys(int key, int x, int y){
	switch(key){
		case GLUT_KEY_LEFT:
			specialKeys[GLUT_KEY_LEFT] = 1;
			break;
		case GLUT_KEY_RIGHT:
			specialKeys[GLUT_KEY_RIGHT] = 1;
			break;
		case GLUT_KEY_UP:
			specialKeys[GLUT_KEY_UP] = 1;
			break;
		case GLUT_KEY_DOWN:
			specialKeys[GLUT_KEY_DOWN] = 1;
			break;
		default:
			break;
	}
}

GLvoid SpecialKeysUp(int key, int x, int y){
	switch(key){
		case GLUT_KEY_LEFT:
			specialKeys[GLUT_KEY_LEFT] = 0;
			break;
		case GLUT_KEY_UP:
			specialKeys[GLUT_KEY_UP] = 0;
			break;
		case GLUT_KEY_RIGHT:
			specialKeys[GLUT_KEY_RIGHT] = 0;
			break;
		case GLUT_KEY_DOWN:
			specialKeys[GLUT_KEY_DOWN] = 0;
			break;
		default:
			break;
	}
}

bool HandleKeyboardInput(){
	if(specialKeys[GLUT_KEY_LEFT]){
		
		return true;
	}
	if(specialKeys[GLUT_KEY_RIGHT]){
		
		return true;
	}
	if(specialKeys[GLUT_KEY_UP]){

		return true;
	}
	if(specialKeys[GLUT_KEY_DOWN]){

		return true;
	}
	return false;
}

int main(int argc, char* argv[])
{
	char windowName[] = "TANK X";
	int windowWidth = 640;
	int windowHeight = 480;
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowSize(windowWidth, windowHeight);
	glutInitWindowPosition(100, 100);
	glutCreateWindow(windowName);

	InitGL();
	glutDisplayFunc(DrawGLScene);
	glutReshapeFunc(ResizeGLScene);
	glutIdleFunc(IdleGLScene);
	glutKeyboardFunc(GLKeyDown);
	glutSpecialFunc(SpecialKeys);
	glutSpecialUpFunc(SpecialKeysUp);
	glutMainLoop();
}