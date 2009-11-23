/*
 *  svg.h
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 20/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__SVG_H__
#define __THE_ESCAPE__SVG_H__

struct CSSInfo {
	double fill_r;
	double fill_g;
	double fill_b;
	bool do_fill;
	
	double stroke_r;
	double stroke_g;
	double stroke_b;
	bool do_stroke;
	
	double stroke_width;
};

void svg_parse_css_style(const char *svg_style, CSSInfo &info);

#endif//__THE_ESCAPE__SVG_H__
