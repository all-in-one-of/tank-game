#include "game_object.h"

game_object::game_object(int id, int geo_id, int tex_id, int health)
{
	this->id = id;
	this->geo = geo_id;
	this->tex = tex_id;
}

game_object::~game_object(){}

