/*
 *  svg.cpp
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 20/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#include "svg.h"

#include <string.h>
#include <stdlib.h>
#include <iostream>

#define INV_255 (0.0039215686274509803)

unsigned char hex2int(char c1, char c2, bool &ok) {
	unsigned char ret = 0x00;
	ok = true;
	
	if(c1 >= '0' && c1 <= '9') {
		ret |= (c1 - '0') << 4;
	} else if(c1 >= 'a' && c1 <= 'f') {
		ret |= (c1 - 'a' + 10) << 4;
	} else if(c1 >= 'A' && c1 <= 'F') {
		ret |= (c1 - 'A' + 10) << 4;
	} else {
		ok = false;
	}
	
	if(c2 >= '0' && c2 <= '9') {
		ret |= c2 - '0';
	} else if(c2 >= 'a' && c2 <= 'f') {
		ret |= c2 - 'a' + 10;
	} else if(c2 >= 'A' && c2 <= 'F') {
		ret |= c2 - 'A' + 10;
	} else {
		ok = false;
	}
	
	return ret;
}

void svg_parse_css_style(const char *svg_style, CSSInfo &info) {
	// Parse the fill: color from the CSS style
	const char *fill_ptr = strstr(svg_style, "fill:");
	info.do_fill = false;
	if(fill_ptr != NULL && strncmp(fill_ptr + 5, "none", 4) != 0) {
		bool ok_red = false, ok_green = false, ok_blue = false;
		info.fill_r = hex2int(fill_ptr[6], fill_ptr[7], ok_red) * INV_255;
		info.fill_g = hex2int(fill_ptr[8], fill_ptr[9], ok_green) * INV_255;
		info.fill_b = hex2int(fill_ptr[10], fill_ptr[11], ok_blue) * INV_255;
		info.do_fill = ok_red && ok_green && ok_blue;
	}
	
	// Parse the stroke: color from the CSS style
	const char *stroke_ptr = strstr(svg_style, "stroke:");
	info.do_stroke = false;
	if(stroke_ptr != NULL && strncmp(stroke_ptr + 7, "none", 4) != 0) {
		bool ok_red = false, ok_green = false, ok_blue = false;
		info.stroke_r = hex2int(stroke_ptr[8], stroke_ptr[9], ok_red) * INV_255;
		info.stroke_g = hex2int(stroke_ptr[10], stroke_ptr[11], ok_green) * INV_255;
		info.stroke_b = hex2int(stroke_ptr[12], stroke_ptr[13], ok_blue) * INV_255;
		info.do_stroke = ok_red && ok_green && ok_blue;
	}
	
	// Parse the stroke-width: length from the CSS style
	info.stroke_width = 1.0;
	const char *stroke_width_ptr = strstr(svg_style, "stroke-width:");
	if(stroke_width_ptr != NULL) {
		info.stroke_width = strtod(stroke_width_ptr + 13, NULL);
	}
}

