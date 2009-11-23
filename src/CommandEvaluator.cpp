/*
 *  CommandEvaluator.cpp
 *  The Escape
 *
 *  Created by Paul Morelle on 12/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#include "CommandEvaluator.h"
#include "SvgCommand.h"
#include "SvgObject.h"

#define TRANSFORM_X(x0, y0, translation, radians) ((translation).x + (x0) * cos(radians) - (y0) * sin(radians))
#define TRANSFORM_Y(x0, y0, translation, radians) ((translation).y + (x0) * sin(radians) + (y0) * cos(radians))

void CommandEvaluator::evaluate(const SvgCommands &commands, const b2Vec2 &translation, double rotation_radians) {
	double x1, y1, x2, y2, x3, y3;
	SvgCommands::const_iterator it, itend = commands.end();
	for(it = commands.begin(); it != itend; ++ it) {
		switch(it->type) {
			case SVGCOMMAND_MOVE_TO:
				x1 = TRANSFORM_X(it->values[0], it->values[1], translation, rotation_radians);
				y1 = TRANSFORM_Y(it->values[0], it->values[1], translation, rotation_radians);
				move_to(x1, y1);
				break;
				
			case SVGCOMMAND_LINE_TO:
				x1 = TRANSFORM_X(it->values[0], it->values[1], translation, rotation_radians);
				y1 = TRANSFORM_Y(it->values[0], it->values[1], translation, rotation_radians);
				line_to(x1, y1);
				break;
				
			case SVGCOMMAND_CURVE_TO:
				x1 = TRANSFORM_X(it->values[0], it->values[1], translation, rotation_radians);
				y1 = TRANSFORM_Y(it->values[0], it->values[1], translation, rotation_radians);
				x2 = TRANSFORM_X(it->values[2], it->values[3], translation, rotation_radians);
				y2 = TRANSFORM_Y(it->values[2], it->values[3], translation, rotation_radians);
				x3 = TRANSFORM_X(it->values[4], it->values[5], translation, rotation_radians);
				y3 = TRANSFORM_Y(it->values[4], it->values[5], translation, rotation_radians);
				curve_to(x1, y1, x2, y2, x3, y3);
				break;
				
			case SVGCOMMAND_CLOSE_PATH:
				close_path();
				break;
				
			case SVGCOMMAND_FILL:
				// 0,1,2 are for filling, 3,4,5 are for stroking
				fill(it->values[0], it->values[1], it->values[2]);
				break;
				
			case SVGCOMMAND_FILL_AND_STROKE:
				// 0,1,2 are for filling, 3,4,5 are for stroking
				fill_and_stroke(it->values[0], it->values[1], it->values[2], it->values[3], it->values[4], it->values[5]);
				break;
				
			case SVGCOMMAND_STROKE:
				// 0,1,2 are for filling, 3,4,5 are for stroking
				stroke(it->values[3], it->values[4], it->values[5]);
				break;
				
			default:
				break;
		}
	}
}

void CommandEvaluator::evaluate(const SvgObject &object) {
	evaluate(object.commands(), object.position(), object.angle());
}
