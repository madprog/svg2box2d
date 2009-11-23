/*
 *  Rectangle.cpp
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 12/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#include "Rectangle.h"

Rectangle::Rectangle(void): m_set(false), m_x0(0.0), m_y0(0.0), m_x1(0.0), m_y1(0.0) {
}

void Rectangle::join(double x, double y) {
	if(m_set) {
		if(x < m_x0) m_x0 = x;
		else if(x > m_x1) m_x1 = x;
		if(y < m_y0) m_y0 = y;
		else if(y > m_y1) m_y1 = y;
	} else {
		m_x0 = m_x1 = x;
		m_y0 = m_y1 = y;
		m_set = true;
	}
}

void Rectangle::translate(double x, double y) {
	m_x0 += x; m_x1 += x;
	m_y0 += y; m_y1 += y;
}

std::ostream &operator<<(std::ostream &o, const Rectangle &r) {
	return o << "Rectangle(" << r.x0() << ", " << r.y0() << ", " << r.x1() << ", " << r.y1() << ")";
}

