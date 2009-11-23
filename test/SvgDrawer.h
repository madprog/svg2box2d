/*
 *  SvgDrawer.h
 *  The Escape
 *
 *  Created by Paul Morelle on 06/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__SVG_DRAWER_H__
#define __THE_ESCAPE__SVG_DRAWER_H__

#include "CommandEvaluator.h"

#include <cairo.h>

class SvgDrawer: public CommandEvaluator {
public:
	SvgDrawer(cairo_t *cr);
	virtual void move_to(double x, double y);
	virtual void line_to(double x, double y);
	virtual void curve_to(double x1, double y1, double x2, double y2, double x3, double y3);
	virtual void close_path(void);
	virtual void fill(double r, double g, double b);
	virtual void stroke(double r, double g, double b);
	virtual void fill_and_stroke(double r_fill, double g_fill, double b_fill,double r_stroke, double g_stroke, double b_stroke);
	
protected:
	cairo_t *m_cr;
};

#endif//__THE_ESCAPE__SVG_DRAWER_H__
