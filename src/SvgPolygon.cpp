/*
 *  SvgPolygon.cpp
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 10/11/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#include "SvgPolygon.h"

#include <iomanip>

#include "CommandEvaluator.h"
#include "maths.h"

class LineGetter: public CommandEvaluator {
public:
	LineGetter(void);
	
	void move_to(double x, double y);
	void line_to(double x, double y);
	void curve_to(double x0, double y0, double x1, double y1, double x2, double y2);
	void close_path();
	void fill(double r, double g, double b) {}
	void stroke(double r, double g, double b) {}
	void fill_and_stroke(double r_fill, double g_fill, double b_fill, double r_stroke, double g_stroke, double b_stroke) {}
	
	const SvgSegments &segments() const { return m_segments; }
	
private:
	bool m_started;
	double m_start_x, m_start_y;
	double m_current_x, m_current_y;
	SvgSegments m_segments;
};

LineGetter::LineGetter(void): m_started(false) {
}

void LineGetter::move_to(double x, double y) {
	if(m_started) close_path();
	m_start_x = m_current_x = x;
	m_start_y = m_current_y = y;
	
	m_started = true;
}

void LineGetter::line_to(double x, double y) {
	SvgSegment segment;
	if(m_started && (fabs(m_current_x - x) > EPSILON || fabs(m_current_y - y) > EPSILON)) {
		segment.p0.x = m_current_x;
		segment.p0.y = m_current_y;
		segment.p1.x = x;
		segment.p1.y = y;
		m_segments.push_back(segment);
		m_current_x = x;
		m_current_y = y;
	}
}

void LineGetter::curve_to(double x0, double y0, double x1, double y1, double x2, double y2) {
	SvgSegments segments;
	bezier_to_segments(m_current_x, m_current_y, x0, y0, x1, y1, x2, y2, segments);
	m_segments.insert(m_segments.end(), segments.begin(), segments.end());
	
	m_current_x = x2;
	m_current_y = y2;
	
	m_started = true;
}

void LineGetter::close_path() {
	if(fabs(m_current_x - m_start_x) > EPSILON || fabs(m_current_y - m_start_y) > EPSILON) {
		line_to(m_start_x, m_start_y);
	}
	
	m_started = false;
}

SvgPolygon::SvgPolygon(const SvgObject &object) {
	LineGetter line_getter;
	line_getter.evaluate(object);
	SvgSegments::const_iterator it = line_getter.segments().begin(), itend = line_getter.segments().end();
	if(fabs(line_getter.segments().back().p1.x - it->p0.x) > EPSILON || fabs(line_getter.segments().back().p1.y - it->p0.y) > EPSILON) {
		m_vertices.push_back(it->p0);
	}
	for(; it != itend; ++ it) {
		m_vertices.push_back(it->p1);
	}
	
	make_clockwise();
}

SvgPolygon::SvgPolygon(const SvgCommands &commands, const b2Vec2 &translation, double rotation) {
	LineGetter line_getter;
	line_getter.evaluate(commands, translation, rotation);
	
	if(line_getter.segments().size() > 0) {
		SvgSegments::const_iterator it = line_getter.segments().begin(), itend = line_getter.segments().end();
		if(fabs(line_getter.segments().back().p1.x - it->p0.x) > EPSILON || fabs(line_getter.segments().back().p1.y - it->p0.y) > EPSILON) {
			m_vertices.push_back(it->p0);
		}
		for(; it != itend; ++ it) {
			m_vertices.push_back(it->p1);
		}
	}
	
	make_clockwise();
}

SvgPolygon::SvgPolygon(const SvgPolygon &p): m_vertices(p.m_vertices.size()) {
	*this = p;
}

SvgPolygon &SvgPolygon::operator=(const SvgPolygon &p) {
	m_vertices.clear();
	
	SvgVertices::const_iterator it_vertices, it_verticesend = p.m_vertices.end();
	for(it_vertices = p.m_vertices.begin(); it_vertices != it_verticesend; ++ it_vertices) {
		m_vertices.push_back(*it_vertices);
	}
	
	make_clockwise();
	
	return *this;
}

void SvgPolygon::reverse(void) {
	SvgVertices vertices;
	vertices.insert(vertices.end(), m_vertices.rbegin(), m_vertices.rend());
	m_vertices = vertices;
}

void SvgPolygon::make_clockwise(void) {
	double cross_products_sum = 0.0;
	
	SvgVertices::const_iterator it, itend = m_vertices.end();
	for(it = m_vertices.begin(); it != itend; ++ it) {
		SvgVertices::const_iterator it_prev(it);
		
		if(it_prev == m_vertices.begin()) {
			it_prev = itend;
		}
		-- it_prev;
		
		const SvgVertex &a = *it_prev;
		const SvgVertex &b = *it;
		
		cross_products_sum += a.x * b.y - a.y * b.x;
	}
	
	if(cross_products_sum < 0) {
		reverse();
	}
}

std::ostream &operator<<(std::ostream &o, const SvgVertex &v) {
	return o << "(" << v.x << "," << v.y << ")";
}

std::ostream &operator<<(std::ostream &o, const SvgPolygon &p) {
	SvgVertices::const_iterator it = p.vertices().begin(), itend = p.vertices().end();
	o << "Polygone[" << *it;
	for(++ it; it != itend; ++ it) {
		o << ", " << *it;
	}
	return o << "]";
}
