/*
 *  maths.h
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 07/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__MATHS_H__
#define __THE_ESCAPE__MATHS_H__

#include <Box2D.h>
#include <iostream>
#include <limits>
#include <vector>

#include "SvgPolygon.h"

#define DOUBLE_NAN std::numeric_limits<double>::quiet_NaN()

extern const double PI;
extern const double TWOPI;
extern const double EPSILON;

void bezier_to_segments(const double x0, const double y0,
						const double x1, const double y1,
						const double x2, const double y2,
						const double x3, const double y3,
						SvgSegments &segments);

void split_polygon_to_convex_set(const SvgPolygon &polygon, SvgPolygons &m_convex_polygons);

#endif//__THE_ESCAPE__MATHS_H__
