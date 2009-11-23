/*
 *  CommandEvaluator.h
 *  The Escape
 *
 *  Created by Paul Morelle on 12/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__COMMAND_EVALUATOR_H__
#define __THE_ESCAPE__COMMAND_EVALUATOR_H__

#include <Box2D.h>
#include <vector>

#include "SvgCommand.h"

class Rectangle;
class SvgObject;
class Transform;

class CommandEvaluator {
public:
	virtual ~CommandEvaluator(void) {}
	
	virtual void move_to(double x, double y) = 0;
	virtual void line_to(double x, double y) = 0;
	virtual void curve_to(double x0, double y0, double x1, double y1, double x2, double y2) = 0;
	virtual void close_path() = 0;
	virtual void fill(double r, double g, double b) = 0;
	virtual void stroke(double r, double g, double b) = 0;
	virtual void fill_and_stroke(double r_fill, double g_fill, double b_fill, double r_stroke, double g_stroke, double b_stroke) = 0;
	
	void evaluate(const SvgCommands &commands, const b2Vec2 &translation=b2Vec2(), double rotation_radians=0.0);
	void evaluate(const SvgObject &object);
};

#endif//__THE_ESCAPE__COMMAND_EVALUATOR_H__
