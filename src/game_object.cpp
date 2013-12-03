#include "game_object.h"

game_object::game_object(int id, int geo_id, int tex_id, int health)
{
	this->id = id;
	this->geo = geo_id;
	this->tex = tex_id;
	parent = NULL;
}

game_object::~game_object(){}

void game_object::parent_to(game_object &o)
{
	parent = &o;
}

GLdouble* game_object::get_transform()
{
	mat4 total_transform;// = this->transform;
	game_object *cur_parent = parent;
	std::vector<game_object*> parents;
	while(cur_parent!=NULL)
	{
		// total_transform *= cur_parent->transform;
		// total_transform = cur_parent->transform * total_transform;
		parents.push_back(cur_parent);
		cur_parent = cur_parent->parent;
	}
	for(int p=0; p<parents.size(); p++)
	{
		total_transform *= parents[p]->transform;
	}
	total_transform *= this->transform;
	for(int i=0; i<16; i++)
	{
		int x = i/4;
		int y = i%4;
		transform_gl[i] = total_transform(y,x);
	}
	return transform_gl;
}

// game_object::add_child(game_object &o)
// {
// 	children.push_back(&o);
// }

// game_object::remove_child(int child_id)
// {
// 	for(int i=0; i<children.size(); i++)
// 	{
// 		if(children[i].id==child_id)
// 		{
// 			children.erase(children.begin()+i);
// 		}
// 	}
// }