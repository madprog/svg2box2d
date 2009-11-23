/*
 *  maths.cpp
 *  The Escape
 *
 *  Created by Paul Morelle (madprog) <paul.morelle@gmail.com> on 07/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#include "maths.h"

#include <Box2D.h>
#include <iomanip>

#include "Exception.h"
#include "SvgPolygon.h"

const double EPSILON = 1e-4;
const double PI = 3.1415926535897931;
const double TWOPI = 6.2831853071795862;

const double MAX_BEZIER_POLYGON_THICKNESS = 0.25;

// Recursively transform a cubic Bezier curve (controlled by 4 anchors) into a polygon
void bezier_to_segments(const double x0, const double y0,
						const double x1, const double y1,
						const double x2, const double y2,
						const double x3, const double y3,
						SvgSegments &segments) {
	double thickness = 1.0e5;
	
	// Avoid infinite recursion if the four points are the same
	if(fabs(x0 - x1) < EPSILON
	   && fabs(x1 - x2) < EPSILON
	   && fabs(x2 - x3) < EPSILON
	   && fabs(y0 - y1) < EPSILON
	   && fabs(y1 - y2) < EPSILON
	   && fabs(y2 - y3) < EPSILON) {
		return; // without creating any segment
	}
	
	// Algorithm:
	// Project the bezier curve on the straight line passing by point A and perpendicular to [AD]
	// See if the thickness of the projected line is larger than a value
	// If yes, then split the curve in two and call the function recursively on each sub-curve
	// Else, you can approximate the curve to a right segment
	
	// d is a vector perpendicular to [AD]. We will project the four points on it.
	double dx = y3 - y0;
	double dy = x0 - x3;
	
	if(fabs(dx) > EPSILON || fabs(dy) > EPSILON) {
		// Normalize d
		double inv_d_length = 1.0 / sqrt(dy * dy + dx * dx);
		dx *= inv_d_length;
		dy *= inv_d_length;
		
		// k0, k1, k2 and k3 are the projections of A, B, C, and D on the right line (A, d), perpendicular to [AD] going through A.
		// A' = A + k0 * d
		// B' = B + k1 * d
		// C' = C + k2 * d
		// D' = D + k3 * d
		// Formulae were found with Maxima
		double divisor = dy * dy + dx * dx;
		// double k0 = 0.0; by construction
		double k1 = (dx * (x1 - x0) + dy * (y1 - y0)) / divisor;
		double k2 = (dx * (x2 - x0) + dy * (y2 - y0)) / divisor;
		// double k3 = 0.0; by construction
		
		// A Bezier curve on one dimension still has the same equation:
		// y(t) = A (1-t)^3 + 3 B (1-t)^2 t + 3 C (1-t) t^2 + D t^3, t in [0,1]
		// As we are looking for the maximum and minimum values, we can solve y'(t) = 0
		// The equation is:
		// y'(t) = (3 D - 9 C + 9 B - 3 A) t^2 + (6 C - 12 B + 6 A) + 3 B - 3 A = 0
		// Knowing that A and D are 0:
		// y'(t) = 9 (C + B) t^2 + 6 (C - 2 B ) + 3 B = 0
		// Let's solve:
		// a t^2 + b t + c = 0
		// with:
		double a = 9.0 * (k1 - k2), b = 6.0 * (k2 - 2.0 * k1), c = 3.0 * k1;
		double determinant = b*b - 4*a*c;
		
		if(fabs(determinant) < EPSILON) { // determinant = 0
			// One solution in this case
			double s = fabs(-0.5 * b / a);
			
			// Extremities of the curve (A and D) are both at 0.0,
			// so we know that this point is on the image
			double min_x = 0.0, max_x = 0.0;
			
			// If s in in the right bounds (t must be in [0,1])
			if(0.0 < s && s < 1.0) {
				// Compute the image of s by the Bezier equation
				double val = k1 * (1 - s) * (1 - s) * s + k2 * (1 - s) * s * s;
				
				// Now we know the bounds of the image of the curve.
				min_x = std::min(min_x, val);
				max_x = std::max(max_x, val);
			}
			
			// And by the way, the thickness
			thickness = max_x - min_x;
			
		} else if(determinant > 0) {
			// Two solutions in this case
			double s1 =     0.5 * (-b - sqrt(determinant)) / a;
			double s2 = 0.5 * (-b + sqrt(determinant)) / a;
			
			// Extremities of the curve (A and D) are both at 0.0,
			// so we know that this point is on the image
			double min_x = 0.0, max_x = 0.0;
			
			// If s1 in in the right bounds (t must be in [0,1])
			if(0.0 < s1 && s1 < 1.0) {
				// Compute the image of s1 by the Bezier equation
				double val = k1 * (1 - s1) * (1 - s1) * s1 + k2 * (1 - s1) * s1 * s1;
				
				// Now we know the bounds of the image of the curve.
				min_x = std::min(min_x, val);
				max_x = std::max(max_x, val);
			}
			
			// If s1 in in the right bounds (t must be in [0,1])
			if(0.0 < s2 && s2 < 1.0) {
				// Compute the image of s2 by the Bezier equation
				double val = k1 * (1 - s2) * (1 - s2) * s2 + k2 * (1 - s2) * s2 * s2;
				
				// Now we know the bounds of the image of the curve.
				min_x = std::min(min_x, val);
				max_x = std::max(max_x, val);
			}
			
			// And by the way, the thickness
			thickness = max_x - min_x;
		} else {
			// No real solution in this case.
			// I assume the curve is straight
			// TODO check this ?
			thickness = 0.0;
		}
	}
	
	if(thickness > MAX_BEZIER_POLYGON_THICKNESS) {
		// This curve is not straight enough.
		// Let's cut the curve in two
		
		bezier_to_segments(         x0,                              y0,
						   0.5 *   (x0 +   x1),             0.5 * (  y0 +   y1),
						   0.25 *  (x0 + 2*x1 +   x2),      0.25 * ( y0 + 2*y1 +   y2),
						   0.125 * (x0 + 3*x1 + 3*x2 + x3), 0.125 * (y0 + 3*y1 + 3*y2 + y3),
						   segments);
		
		bezier_to_segments(0.125 * (x0 + 3*x1 + 3*x2 + x3), 0.125 * (y0 + 3*y1 + 3*y2 + y3),
						   0.25 *         (x1 + 2*x2 + x3), 0.25 *         (y1 + 2*y2 + y3),
						   0.5 *                 (x2 + x3), 0.5 *                 (y2 + y3),
						                               x3,                              y3,
						   segments);
	} else {
		// This curve is straight enough.
		// Let's approximate it to the [AD] segment.
		
		SvgSegment segment;
		segment.p0.x = x0;
		segment.p0.y = y0;
		segment.p1.x = x3;
		segment.p1.y = y3;
		segments.push_back(segment);
	}
}

// Tells whether there is an intersection between the half straight line formed by origin and vector, and the ab segment.
// If there is such an intersection, then set the intersection SvgVertex to its position.
bool find_intersection(const SvgVertex &origin, const SvgVertex &vector, const SvgVertex &a, const SvgVertex &b, SvgVertex &intersection) {
	// It will return false if the vector is null, and if a == b.
	double divisor = (b.x - a.x) * vector.y + (a.y - b.y) * vector.x;
	if(fabs(divisor) > EPSILON) { 
		// Projection formulae found with Maxima
		// They solve the equation system:
		// origin.x + k vector.x = a.x + l (b.x - a.x)
		// origin.y + k vector.y = a.y + l (b.y - a.y)
		double k = -((b.x - a.x) * origin.y + (a.y - b.y) * origin.x + a.x * b.y - a.y * b.x) / divisor;
		double l = ((origin.x - a.x) * vector.y + (a.y - origin.y) * vector.x) / divisor;
		
		// If k is positive, then the intersection is on the right half of the straight line.
		// If l is between 0 and 1, then the intersection is on the segment (EPSILON is here to include a and b themselves).
		if(k > EPSILON && l >= -EPSILON && l <= 1 + EPSILON) {
			// Just compute the position with one of the two equations
			intersection.x = a.x + l * (b.x - a.x);
			intersection.y = a.y + l * (b.y - a.y);
			return true;
		}
	}
	
	return false;
}

void split_polygon_to_convex_set(const SvgPolygon &polygon, SvgPolygons &convex_polygons) {
	// Some constant values to simplify the writing
	const SvgVertices &vertices = polygon.vertices();
	const size_t NB_VERTICES = vertices.size();
	
	// We do not want to test straight lines
	if(NB_VERTICES < 3) {
		return;
	}
	
	// We assume that the polygon is given in clockwise order.
	// Please call SvgPolygon::make_clockwise(void)
	
	// Find a notch, i.e. a vertex whose interior angle is larger than π
	// For such a vertex, the cross product will be negative
	// (it would be positive if we were reading the vertices in counter-clockwise order)
	// (that's why it is so important to know)
	size_t notch_index;
	for(notch_index = 0; notch_index < NB_VERTICES; ++ notch_index) {
		const SvgVertex &a = vertices[(notch_index + NB_VERTICES - 1) % NB_VERTICES];
		const SvgVertex &b = vertices[notch_index];
		const SvgVertex &c = vertices[(notch_index + 1) % NB_VERTICES];
		
		const SvgVertex u(b - a);
		const SvgVertex v(c - b);
		
		if((u ^ v) < -EPSILON) {
			// We found a notch, stop the loop
			break;
		}
	}
	
	// If we have found a notch
	if(notch_index < NB_VERTICES) {
		bool found_one_intersection = false;
		SvgVertex closest_intersection;
		size_t vertex_index;
		size_t closest_intersection_prev_index;
		
		// Then get it back, and find its neighbors
		const SvgVertex &prev_notch = vertices[(notch_index + NB_VERTICES - 1) % NB_VERTICES];
		const SvgVertex &notch = vertices[notch_index];
		const SvgVertex &next_notch = vertices[(notch_index + 1) % NB_VERTICES];
		
		// vector is such that it cuts the angle in two (don't remember the name in English, the French is 'bissectrice')
		const SvgVertex vector = (notch - prev_notch).normalize() - (next_notch - notch).normalize();
		
		// Now let's find the closest intersection between this vector and a segment of the polygon
		for(vertex_index = 0; vertex_index < NB_VERTICES; ++ vertex_index) {
			
			// Obviously, we don't want to detect the intersection with the notch itself
			if(vertex_index != (notch_index + NB_VERTICES - 1) % NB_VERTICES) {
				// Get the vertices of the segment
				const SvgVertex &vertex1 = vertices[vertex_index];
				const SvgVertex &vertex2 = vertices[(vertex_index + 1) % NB_VERTICES];
				
				// This is to get the intersection
				SvgVertex intersection;
				
				// Call the intersection utility function
				if(find_intersection(notch, vector, vertex1, vertex2, intersection)) {
					
					// Test the proximity of the intersection (it could be farther than an other already found)
					if(!found_one_intersection || (intersection - notch).length() < (closest_intersection - notch).length()) {
						closest_intersection = intersection;
						found_one_intersection = true;
						closest_intersection_prev_index = vertex_index;
					}
				}
			}
		}
		
		// OMG! Not any intersection? WTF is this polygon?
		if(!found_one_intersection) {
			throw Exception("Could not split the polygon to convex set!");
		}
		
		// Let's split the polygon in two
		SvgPolygon polygon1, polygon2;
		size_t tmp_vertex_index;
		
		// The first child polygon will be from the intersection to the notch vertices, still in clockwise order
		polygon1.append(closest_intersection);
		for(tmp_vertex_index = (closest_intersection_prev_index + 1) % NB_VERTICES; tmp_vertex_index != notch_index; tmp_vertex_index = (tmp_vertex_index + 1) % NB_VERTICES) {
			polygon1.append(vertices[tmp_vertex_index]);
		}
		polygon1.append(notch);
		
		// The second child polygon will be from the notch to the intersection vertices, still in clockwise order
		for(; tmp_vertex_index != (closest_intersection_prev_index + 1) % NB_VERTICES; tmp_vertex_index = (tmp_vertex_index + 1) % NB_VERTICES) {
			polygon2.append(vertices[tmp_vertex_index]);
		}
		polygon2.append(closest_intersection);
		
		// Ensure that the new polygons are in clockwise order
		// TODO Is it really useful?
		polygon1.make_clockwise();
		polygon2.make_clockwise();
		
		// Try to split them into convex pieces
		split_polygon_to_convex_set(polygon1, convex_polygons);
		split_polygon_to_convex_set(polygon2, convex_polygons);
	} else {
		// This polygon appears to be convex.
		// Box2D wants up to 'b2_maxPolygonVertices' vertices in a polygon.
		// So just split this one into one or more pieces of this number of vertices
		// For new polygons, we start with the first point (which will be common to all sub-polygons)
		// and the last point of the previous polygons. This way, we ensure that all polygons will have 3 points at minimum.
		SvgVertices::const_iterator prev_vertex_it = polygon.vertices().begin();
		SvgVertices::const_iterator vertex_it = prev_vertex_it; ++ vertex_it;
		SvgVertices::const_iterator vertex_itend = polygon.vertices().end();
		const SvgVertex &first_vertex = *prev_vertex_it;
		
		// Scan all the vertices
		while(vertex_it != vertex_itend) {
			SvgPolygon sub_polygon;
			
			// Always add the first vertex: it is common to all the sub-polygons, as the polygon is convex
			sub_polygon.append(first_vertex);
			
			// Skip the eventual vertices being at the same position as prev_vertex_it
			while(vertex_it != vertex_itend && *vertex_it == first_vertex) {
				++ vertex_it;
				++ prev_vertex_it;
			}
			
			if(vertex_it != vertex_itend) {
				sub_polygon.append(*vertex_it);
				
				while(sub_polygon.size() < static_cast<size_t>(b2_maxPolygonVertices) && vertex_it != vertex_itend) {
					// Advance by one vertex, and skip the eventual vertices being at the same position as prev_vertex_it
					do {
						++ vertex_it;
						++ prev_vertex_it;
					} while(vertex_it != vertex_itend && *vertex_it == *prev_vertex_it);
					
					if(vertex_it != vertex_itend) {
						sub_polygon.append(*vertex_it);
					}
				}
				
				// If the sub-polygon has enough points, add it
				// Else, we only have first_vertex, and maybe the last point of the previous polygon
				// This may happen when some points were at the same position
				if(sub_polygon.size() >= 3) {
					convex_polygons.push_back(sub_polygon);
				}
			}
		}
	}
}
