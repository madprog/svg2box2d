/*
 *  Game.h
 *  The Escape
 *
 *  Created by Paul Morelle on 06/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__GAME_H__
#define __THE_ESCAPE__GAME_H__

#include <Box2D.h>
#include <cairo.h>
#include <list>
#include <string>

#include "SvgObject.h"

extern const double GRAVITY_NORM;
extern const double PIXELS_PER_METER;
extern const double METERS_PER_PIXEL;

class Game {
public:
	Game();
	~Game();
	
	void advance(double dt);
	void draw(cairo_t *cr) const;

private:
	b2World *m_box2d_world;
	const std::string m_resources_dir;
	SvgObjects m_objects;
};

#endif//__THE_ESCAPE__GAME_H__
