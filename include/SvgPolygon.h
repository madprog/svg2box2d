/*
 *  SvgPolygon.h
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 10/11/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__SVG_POLYGON_H__
#define __THE_ESCAPE__SVG_POLYGON_H__

#include <Box2D.h>
#include <iostream>
#include <map>
#include <vector>

extern const double EPSILON;

class SvgObject;
class SvgCommand;
typedef std::vector<SvgCommand> SvgCommands;

class SvgVertex {
public:
	double x, y;
	
	SvgVertex(void): x(0.0), y(0.0) {}
	SvgVertex(double x, double y): x(x), y(y) {}
	SvgVertex(const SvgVertex &v): x(v.x), y(v.y) {}
	
	bool operator!=(const SvgVertex &v) const { return fabs(x - v.x) > EPSILON || fabs(y - v.y) > EPSILON; }
	bool operator==(const SvgVertex &v) const { return fabs(x - v.x) < EPSILON && fabs(y - v.y) < EPSILON; }
	bool operator<(const SvgVertex &v) const { return x < v.x || (x == v.x && y < v.y); }
	SvgVertex &operator+=(const SvgVertex &v) { x += v.x; y += v.y; return *this; }
	SvgVertex operator+(const SvgVertex &v) const { return SvgVertex(*this) += v; }
	SvgVertex &operator-=(const SvgVertex &v) { x -= v.x; y -= v.y; return *this; }
	SvgVertex operator-(const SvgVertex &v) const { return SvgVertex(*this) -= v; }
	double operator^(const SvgVertex &v) const { return x * v.y - y * v.x; }
	double length2(void) const { return x*x + y*y; }
	double length(void) const { return sqrt(length2()); }
	SvgVertex &normalize(void) { if(length2() != 1) { double l = 1.0 / length(); x *= l; y *= l; } return *this; }
};

typedef std::vector<SvgVertex> SvgVertices;

std::ostream &operator<<(std::ostream &o, const SvgVertex &v);

class SvgSegment {
public:
	SvgVertex p0;
	SvgVertex p1;
};

typedef std::vector<SvgSegment> SvgSegments;

class SvgPolygon {
public:
	SvgPolygon(void) {}
	SvgPolygon(const SvgObject &object);
	SvgPolygon(const SvgCommands &commands, const b2Vec2 &translation, double rotation);
	SvgPolygon(const SvgPolygon &p);
	SvgPolygon &operator=(const SvgPolygon &p);
	
	const SvgVertices &vertices(void) const { return m_vertices; }
	SvgVertices &vertices(void) { return m_vertices; }
	
	void append(const SvgVertex &v) { m_vertices.push_back(v); }
	size_t size(void) const { return m_vertices.size(); }
	
	void make_clockwise(void);
	
private:
	SvgVertices m_vertices;
	
	void reverse(void);
};

typedef std::vector<SvgPolygon> SvgPolygons;

std::ostream &operator<<(std::ostream &o, const SvgPolygon &p);

#endif//__THE_ESCAPE__SVG_POLYGON_H__
