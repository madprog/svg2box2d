/*
 *  SvgObject.cpp
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 06/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#include "SvgObject.h"

#include <algorithm>
#include <cstdlib>
#include <cstring>
#include <math.h>
#include <libxml/xpathInternals.h>

#include "Exception.h"
#include "SvgPolygon.h"
#include "Rectangle.h"
#include "svg.h"

SvgObjects SvgObject::load_svg_objects(const std::string &svg_file_name, b2World &world) {
	SvgObjects ret;
	
	// Read the SVG picture
	xmlDocPtr document = xmlParseFile(svg_file_name.c_str());
	if(document == NULL) {
		throw Exception("Could not load SVG level '" + svg_file_name + "': xmlParseFile error");
	}
	
	// Create a XPath context from the XML document
	xmlXPathContextPtr context = xmlXPathNewContext(document);
	if(context == NULL) {
		throw Exception("Could not load SVG level '" + svg_file_name + "': xmlXPathNewContext error");
	}
	
	// Declare the SVG XML namespace
	xmlXPathRegisterNs(context, BAD_CAST "inkscape", BAD_CAST "http://www.inkscape.org/namespaces/inkscape");
	xmlXPathRegisterNs(context, BAD_CAST "svg", BAD_CAST "http://www.w3.org/2000/svg");

	// Get the root element
	xmlNodePtr root = xmlDocGetRootElement(document);

	// Get the units by meter parameter
	const char *units_by_meter_str = (const char *)xmlGetProp(root, BAD_CAST "units_by_meter");
	if(units_by_meter_str == NULL) {
		throw Exception("Could not load SVG object: Could not find attribute 'units_by_meter' in the root element");
	}
	double units_by_meter = std::strtod(units_by_meter_str, NULL);
	
	int nb_nodes;
	xmlNodePtr *nodeTab;
	xmlXPathObjectPtr xpath_result;
	
	///////////////////////////////////////////////////
	// Query the XPath context for <svg:rect> and <svg:path> elements
	xpath_result = xmlXPathEvalExpression(BAD_CAST "//svg:rect|//svg:path", context);
	if(xpath_result == NULL) {
		throw Exception("Could not load SVG level '" + svg_file_name + "': xmlXPathEvalExpression error");
	}
	
	// Create the commands from the <rect>s
	nb_nodes = xpath_result->nodesetval->nodeNr;
	nodeTab = xpath_result->nodesetval->nodeTab;
	for(int i = 0; i < nb_nodes; ++ i) {
		ret.push_back(new SvgObject(nodeTab[i], world, units_by_meter));
	}
	
	xmlXPathFreeObject(xpath_result);
	xmlXPathFreeContext(context);
	xmlFreeDoc(document);
	
	return ret;
}

void SvgObject::get_object_properties(const xmlNodePtr node) {
	std::string node_name((const char *)node->name);
	
	// Get the id of this path
	const char *svg_id_str = (const char *)xmlGetProp(node, BAD_CAST "id");
	if(svg_id_str == NULL) {
		throw Exception("Could not load SVG object: Could not find attribute 'id' in a <" + node_name + "> element");
	}
	m_id = svg_id_str;
	
	/////////////////////////////////////
	// Get the CSS style of this object
	const char *svg_style = (const char *)xmlGetProp(node, BAD_CAST "style");
	if(svg_style == NULL) {
		throw Exception("Could not load SVG object: Could not find attribute 'style' in <" + node_name + "> element #" + id());
	}
	svg_parse_css_style(svg_style, m_css_infos);
}

void SvgObject::get_rect_commands(const xmlNodePtr node, Rectangle &boundaries) {
	SvgCommand command;
	
	const char *svg_x = (const char *)xmlGetProp(node, BAD_CAST "x");
	if(svg_x == NULL) {
		throw Exception("Could not load SVG object: Could not find attribute 'x' in <rect> element #" + id());
	}
	const char *svg_y = (const char *)xmlGetProp(node, BAD_CAST "y");
	if(svg_y == NULL) {
		throw Exception("Could not load SVG object: Could not find attribute 'y' in <rect> element #" + id());
	}
	const char *svg_width = (const char *)xmlGetProp(node, BAD_CAST "width");
	if(svg_width == NULL) {
		throw Exception("Could not load SVG object: Could not find attribute 'width' in <rect> element #" + id());
	}
	const char *svg_height = (const char *)xmlGetProp(node, BAD_CAST "height");
	if(svg_height == NULL) {
		throw Exception("Could not load SVG object: Could not find attribute 'height' in <rect> element #" + id());
	}
	
	double x = 0, y = 0, width = 0, height = 0;
	
	x = strtod(svg_x, NULL);
	y = strtod(svg_y, NULL);
	width = strtod(svg_width, NULL);
	height = strtod(svg_height, NULL);
	
	boundaries.join(x, y);
	boundaries.join(x + width, y + height);
	
	command.type = SVGCOMMAND_MOVE_TO;
	command.values[0] = x;
	command.values[1] = y;
	m_commands.push_back(command);
	
	command.type = SVGCOMMAND_LINE_TO;
	command.values[0] = x + width;
	m_commands.push_back(command);
	
	command.values[1] = y + height;
	m_commands.push_back(command);
	
	command.values[0] = x;
	m_commands.push_back(command);
	
	command.type = SVGCOMMAND_CLOSE_PATH;
	m_commands.push_back(command);
}

// Try to read a double on the stream pointed by ptr
// Returns true or false whether it succeeded or not
// Put the read value in value and updates ptr to the new position
bool read_next_double(const char *&ptr, double &value) {
	double ret;
	char *ptr_end = NULL;

	while(*ptr == ' ' || *ptr == ',') ++ ptr;

	ret = std::strtod(ptr, &ptr_end);

	if(ptr_end != ptr) {
		ptr = ptr_end;
		value = ret;
		return true;
	}

	return false;
}

void SvgObject::get_path_commands(const xmlNodePtr node, Rectangle &boundaries) {
	SvgCommand command;
	double current_x = 0.0, current_y = 0.0;
	
	const char *svg_commands = (const char *)xmlGetProp(node, BAD_CAST "d");
	if(svg_commands == NULL) {
		throw Exception("Could not load SVG object: Could not find attribute 'd' in <path> element #" + id());
	}
	
	// Generate the commands
	char *ptr_end;
	const char *ptr = svg_commands;
	
	// Skip the spaces
	for(; *ptr == ' '; ++ ptr);
	
	for(; *ptr != '\0';) {
		// Get the SVG command (TODO: complete the command set if Inkscape generates other commands)
		switch(*ptr) {
			case 'M':
				command.type = SVGCOMMAND_MOVE_TO;
				++ ptr; // Skip the command character
				if(read_next_double(ptr, command.values[0])) {
					if(read_next_double(ptr, command.values[1])) {
						m_commands.push_back(command);

						boundaries.join(command.values[0], command.values[1]);
						current_x = command.values[0];
						current_y = command.values[1];
					} else {
						throw Exception("Could not load SVG object: bad number of arguments to command M in attribute 'd' of <path> element #" + id());
					}
				}
				
			case 'L':
				command.type = SVGCOMMAND_LINE_TO;
				++ ptr; // Skip the command character
				while(read_next_double(ptr, command.values[0])) {
					if(read_next_double(ptr, command.values[1])) {
						m_commands.push_back(command);

						boundaries.join(command.values[0], command.values[1]);
						current_x = command.values[0];
						current_y = command.values[1];
					} else {
						throw Exception("Could not load SVG object: bad number of arguments to command L in attribute 'd' of <path> element #" + id());
					}
				}
				break;
				
			case 'm':
				command.type = SVGCOMMAND_MOVE_TO;
				++ ptr; // Skip the command character
				if(read_next_double(ptr, command.values[0])) {
					if(read_next_double(ptr, command.values[1])) {
						m_commands.push_back(command);

						boundaries.join(command.values[0], command.values[1]);
						current_x = command.values[0];
						current_y = command.values[1];
					} else {
						throw Exception("Could not load SVG object: bad number of arguments to command m in attribute 'd' of <path> element #" + id());
					}
				}

			case 'l':
				command.type = SVGCOMMAND_LINE_TO;
				++ ptr; // Skip the command character
				while(read_next_double(ptr, command.values[0])) {
					if(read_next_double(ptr, command.values[1])) {
						// Relative!
						command.values[0] += current_x;
						command.values[1] += current_y;
						m_commands.push_back(command);

						boundaries.join(command.values[0], command.values[1]);
						current_x = command.values[0];
						current_y = command.values[1];
					} else {
						throw Exception("Could not load SVG object: bad number of arguments to command l in attribute 'd' of <path> element #" + id());
					}
				}
				break;
				
			case 'C':
				command.type = SVGCOMMAND_CURVE_TO;
				++ ptr; // Skip the command character
				while(read_next_double(ptr, command.values[0])) {
					if(read_next_double(ptr, command.values[1])
							&& read_next_double(ptr, command.values[2])
							&& read_next_double(ptr, command.values[3])
							&& read_next_double(ptr, command.values[4])
							&& read_next_double(ptr, command.values[5])) {
						m_commands.push_back(command);

						boundaries.join(command.values[0], command.values[1]);
						boundaries.join(command.values[2], command.values[3]);
						boundaries.join(command.values[4], command.values[5]);
						current_x = command.values[4];
						current_y = command.values[5];
					} else {
						throw Exception("Could not load SVG object: bad number of arguments to command C in attribute 'd' of <path> element #" + id());
					}
				}
				break;
				
			case 'c':
				command.type = SVGCOMMAND_CURVE_TO;
				++ ptr; // Skip the command character
				while(read_next_double(ptr, command.values[0])) {
					if(read_next_double(ptr, command.values[1])
							&& read_next_double(ptr, command.values[2])
							&& read_next_double(ptr, command.values[3])
							&& read_next_double(ptr, command.values[4])
							&& read_next_double(ptr, command.values[5])) {
						command.values[0] += current_x;
						command.values[1] += current_y;
						command.values[2] += current_x;
						command.values[3] += current_y;
						command.values[4] += current_x;
						command.values[5] += current_y;
						m_commands.push_back(command);

						boundaries.join(command.values[0], command.values[1]);
						boundaries.join(command.values[2], command.values[3]);
						boundaries.join(command.values[4], command.values[5]);
						current_x = command.values[4];
						current_y = command.values[5];
					} else {
						throw Exception("Could not load SVG object: bad number of arguments to command c in attribute 'd' of <path> element #" + id());
					}
				}
				break;
				
				// Hard to do. For the moment, use Bezier curves!
				// (Path > Object to Path, in Inkscape)
				//case 'A':
				//	break;
				
			case 'z':
				command.type = SVGCOMMAND_CLOSE_PATH;
				m_commands.push_back(command);
				++ ptr;
				break;
				
			default:
				throw Exception(std::string("Unknown SVG command '") + *ptr + "' in attribute 'd' of <path> element #" + id());
		}
		
		// Skip the spaces
		for(; *ptr == ' '; ++ ptr);
	}
}

void SvgObject::get_css_info(void) {
	SvgCommand command;
	
	// Only fill and/or stroke if the CSS style has a color for it
	if(m_css_infos.do_stroke && m_css_infos.do_fill) command.type = SVGCOMMAND_FILL_AND_STROKE;
	else if(m_css_infos.do_stroke) command.type = SVGCOMMAND_STROKE;
	else if(m_css_infos.do_fill) command.type = SVGCOMMAND_FILL;
	
	// Set the color values. (fill|stroke)_[rgb] are ints on [0-255], so convert them to doubles on [0-1].
	// 0,1,2 are for filling, 3,4,5 are for stroking
	if(m_css_infos.do_stroke || m_css_infos.do_fill) {
		command.values[0] = m_css_infos.fill_r;
		command.values[1] = m_css_infos.fill_g;
		command.values[2] = m_css_infos.fill_b;
		command.values[3] = m_css_infos.stroke_r;
		command.values[4] = m_css_infos.stroke_g;
		command.values[5] = m_css_infos.stroke_b;
		m_commands.push_back(command);
	}
}

void SvgObject::create_physics(const xmlNodePtr node, double center_x, double center_y) {
	///////////////////////////////////
	// Read physics info initialization
	
	// Get the position and rotation of this object
	b2BodyDef body_def;
	const char *svg_transform = (const char *)xmlGetProp(node, BAD_CAST "transform");
	if(svg_transform != NULL) {
		if(strncmp(svg_transform, "matrix(", 7) == 0) {
			const char *ptr = svg_transform + 7;
			char *ptr_end;
			double a = std::strtod(ptr, &ptr_end); ptr = ptr_end;
			for(++ ptr; *ptr == ' ' || *ptr == ','; ++ ptr);
			double b = std::strtod(ptr, &ptr_end); ptr = ptr_end;
			for(++ ptr; *ptr == ' ' || *ptr == ','; ++ ptr);
			/*double c =*/ std::strtod(ptr, &ptr_end); ptr = ptr_end;
			for(++ ptr; *ptr == ' ' || *ptr == ','; ++ ptr);
			/*double d =*/ std::strtod(ptr, &ptr_end); ptr = ptr_end;
			for(++ ptr; *ptr == ' ' || *ptr == ','; ++ ptr);
			double e = std::strtod(ptr, &ptr_end); ptr = ptr_end;
			for(++ ptr; *ptr == ' ' || *ptr == ','; ++ ptr);
			double f = std::strtod(ptr, &ptr_end); ptr = ptr_end;
			
			body_def.position.Set((center_x + e) * m_meters_by_unit, (center_y + f) * m_meters_by_unit);
			body_def.angle = atan2(b, a);
		} else if(strncmp(svg_transform, "translate(", 10) == 0) {
			const char *ptr = svg_transform + 10;
			char *ptr_end;
			double dx = std::strtod(ptr, &ptr_end); ptr = ptr_end;
			for(++ ptr; *ptr == ' ' || *ptr == ','; ++ ptr);
			double dy = std::strtod(ptr, &ptr_end); ptr = ptr_end;
			
			body_def.position.Set((center_x + dx) * m_meters_by_unit, (center_y + dy) * m_meters_by_unit);
			body_def.angle = 0.0;
		//} else if(strncmp(svg_transform, "scale(", 6) == 0) {
		//} else if(strncmp(svg_transform, "rotate(", 7) == 0) {
		//} else if(strncmp(svg_transform, "skewX(", 6) == 0) {
		//} else if(strncmp(svg_transform, "skewY(", 6) == 0) {
		} else {
			throw Exception("Could not load SVG object: Could not parse attribute 'transform' in <" + std::string((const char *)node->name) + "> element #" + id());
		}
	} else {
		body_def.position.Set(center_x * m_meters_by_unit, center_y * m_meters_by_unit);
		body_def.angle = 0.0;
	}
	
	m_body = m_world.CreateBody(&body_def);
	
	// Get the density of this object
	const char *density_str = (const char *)xmlGetProp(node, BAD_CAST "density");
	if(density_str != NULL) {
		m_density = strtod(density_str, NULL);
	} else {
		m_density = 0.0;
	}

	assert(m_body != NULL);
	
	// Create a polygon from the path
	SvgPolygon polygon(m_commands, b2Vec2(0.0, 0.0), 0.0);
	//SvgPolygons convex_polygons;
	split_polygon_to_convex_set(polygon, m_convex_polygons);
	
	SvgPolygons::const_iterator it_polygon, it_polygonend = m_convex_polygons.end();
	for(it_polygon = m_convex_polygons.begin(); it_polygon != it_polygonend; ++ it_polygon) {
		b2PolygonDef polygon_def;
		polygon_def.vertexCount = it_polygon->vertices().size();
		assert(polygon_def.vertexCount <= b2_maxPolygonVertices);
		for(int i = 0; i < polygon_def.vertexCount; ++ i) {
			polygon_def.vertices[i].Set(it_polygon->vertices()[i].x * m_meters_by_unit, it_polygon->vertices()[i].y * m_meters_by_unit);
		}
		
		polygon_def.density = m_density;
		polygon_def.friction = 0.3;
		
		m_shapes.push_back(m_body->CreateShape(&polygon_def));
	}
	
	m_body->SetMassFromShapes();
}

void SvgObject::translate_commands(double dx, double dy) {
	SvgCommands::iterator it, itend = m_commands.end();
	for(it = m_commands.begin(); it != itend; ++ it) {
		SvgCommand &command = *it;
		command.values[0] += dx;
		command.values[1] += dy;
		command.values[2] += dx;
		command.values[3] += dy;
		command.values[4] += dx;
		command.values[5] += dy;
	}
}

SvgObject::SvgObject(const xmlNodePtr node, b2World &world, double units_by_meter): m_world(world), m_units_by_meter(units_by_meter), m_meters_by_unit(1.0/units_by_meter) {
	if(node != NULL) {
		Rectangle boundaries;
		
		get_object_properties(node);
		
		/////////////////////////////////////
		// Get the SVG commands of this object
		if(strncmp((const char *)node->name, "rect", 5) == 0) {
			get_rect_commands(node, boundaries);
		} else if(strncmp((const char *)node->name, "path", 5) == 0) {
			get_path_commands(node, boundaries);
		}
		
		// The middle of the bounding box
		double center_x = (boundaries.x0() + boundaries.x1()) / 2.0;
		double center_y = (boundaries.y0() + boundaries.y1()) / 2.0;
		
		// Get the transform center of this object
		const char *inkscape_transform_center_x = (const char *)xmlGetProp(node, BAD_CAST "transform-center-x");
		const char *inkscape_transform_center_y = (const char *)xmlGetProp(node, BAD_CAST "transform-center-y");
		if(inkscape_transform_center_x != NULL) {
			center_x += strtod(inkscape_transform_center_x, NULL);
		}
		if(inkscape_transform_center_y != NULL) {
			center_y += strtod(inkscape_transform_center_y, NULL);
		}
		
		// Re-center the object points on its local origin
		translate_commands(-center_x, -center_y);
		
		// Initialize drawing information
		get_css_info();
		
		// Initialize Box2D objects
		create_physics(node, center_x, center_y);
	} else {
		throw Exception("Cannot create a SvgObject from a NULL node");
	}
}

SvgObject::~SvgObject(void) {
}

