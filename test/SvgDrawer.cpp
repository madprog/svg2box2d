/*
 *  SvgObject.cpp
 *  The Escape
 *
 *  Created by Paul Morelle on 06/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#include "SvgDrawer.h"

/******************************************************************************
 * SvgDrawer - knows how to draw a SVG object polygon limits with a Cairo     *
 *             cursor                                                         *
 ******************************************************************************/
SvgDrawer::SvgDrawer(cairo_t *cr): m_cr(cr) { }
void SvgDrawer::move_to(double x, double y) { cairo_move_to(m_cr, x, y); }
void SvgDrawer::line_to(double x, double y) { cairo_line_to(m_cr, x, y); }
void SvgDrawer::curve_to(double x1, double y1, double x2, double y2, double x3, double y3) { cairo_curve_to(m_cr, x1, y1, x2, y2, x3, y3); }
void SvgDrawer::close_path(void) { cairo_close_path(m_cr); }
void SvgDrawer::stroke(double r, double g, double b) { cairo_set_source_rgb(m_cr, r, g, b); cairo_stroke(m_cr); }
void SvgDrawer::fill(double r, double g, double b) { cairo_set_source_rgb(m_cr, r, g, b); cairo_fill(m_cr); }
void SvgDrawer::fill_and_stroke(double r_fill, double g_fill, double b_fill,double r_stroke, double g_stroke, double b_stroke) {
	cairo_set_source_rgb(m_cr, r_fill, g_fill, b_fill); cairo_fill_preserve(m_cr);
	cairo_set_source_rgb(m_cr, r_stroke, g_stroke, b_stroke); cairo_stroke(m_cr);
}
