/*
 *  SvgObject.h
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 06/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__SVG_LEVEL_H__
#define __THE_ESCAPE__SVG_LEVEL_H__

#include <Box2D.h>
#include <libxml/xpath.h>
#include <string>

#include "Rectangle.h"
#include "SvgCommand.h"
#include "maths.h"
#include "svg.h"

class SvgObject;
typedef std::vector<SvgObject *> SvgObjects;

class SvgObject {
public:
	static SvgObjects load_svg_objects(const std::string &svg_file_name, b2World &world);
	
	SvgObject(const xmlNodePtr node, b2World &world, double units_by_meter);
	~SvgObject(void);
	
	const std::string &id() const { return m_id; }
	const SvgCommands &commands(void) const { return m_commands; }
	b2Vec2 position(void) const { return m_units_by_meter * m_body->GetPosition(); }
	//b2Vec2 position(void) const { return m_body->GetPosition(); }
	double angle(void) const { return m_body->GetAngle(); }
	
private:
	const double m_units_by_meter;
	const double m_meters_by_unit;

	CSSInfo m_css_infos;
	std::string m_id;

	b2World &m_world;
	b2Body *m_body;
	std::vector<b2Shape *> m_shapes;
	double m_density;
	
	SvgCommands m_commands;
	SvgPolygons m_convex_polygons;
	
	void get_object_properties(const xmlNodePtr node);
	void get_rect_commands(const xmlNodePtr node, Rectangle &boundaries);
	void get_path_commands(const xmlNodePtr node, Rectangle &boundaries);
	void get_css_info(void);
	void create_physics(const xmlNodePtr node, double center_x, double center_y);
	void translate_commands(double dx, double dy);
};

#endif//__THE_ESCAPE__SVG_LEVEL_H__
