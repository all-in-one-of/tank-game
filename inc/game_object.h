#ifndef __GAME_OBJECT_H__
#define __GAME_OBJECT_H__

#ifdef __APPLE__
#include <GLUT/glut.h>
#include <OpenGL/glext.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glx.h>
#include <GL/glut.h>
#endif

#include <iostream>
#include <string>
#include <vector>
#include <stddef.h>
#include "vec4.h"
#include "mesh.h"
#include "mat4.h"

class game_object{
public:
	game_object(int id, int geo_id, int tex_id, int health=1);
	~game_object();
	void parent_to(game_object &o);
	GLdouble* get_transform();
	// add_child(game_object &o);
	// remove_child(int child_id);
	int health;
	int id;
	int geo;
	int tex;

	mat4 transform;
	game_object *parent;
	// std::vector<game_object*> children;

private:
	GLdouble transform_gl[16];

};

#endif

