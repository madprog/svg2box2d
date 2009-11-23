/*
 *  Rectangle.h
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 12/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__RECTANGLE_H__
#define __THE_ESCAPE__RECTANGLE_H__

#include "maths.h"

class Rectangle {
public:
	Rectangle(void);
	~Rectangle(void) {}
	
	double x0(void) const { return m_set?m_x0:DOUBLE_NAN; }
	double y0(void) const { return m_set?m_y0:DOUBLE_NAN; }
	double x1(void) const { return m_set?m_x1:DOUBLE_NAN; }
	double y1(void) const { return m_set?m_y1:DOUBLE_NAN; }
	
	double width(void) const { return x1() - x0(); }
	double height(void) const { return y1() - y0(); }
	
	void join(double x, double y);
	void reset(void) { m_set = false; }
	void translate(double x, double y);
	
private:
	bool m_set;
	double m_x0;
	double m_y0;
	double m_x1;
	double m_y1;
};

std::ostream &operator<<(std::ostream &o, const Rectangle &r);

#endif//__THE_ESCAPE__RECTANGLE_H__
