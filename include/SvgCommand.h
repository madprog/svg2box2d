/*
 *  SvgCommand.h
 *  The Escape
 *
 *  Created by madprog on 27/10/09.
 *  Copyright 2009 __MyCompanyName__. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__SVG_COMMAND_H_
#define __THE_ESCAPE__SVG_COMMAND_H_

#include <vector>

enum SvgCommandType {
	SVGCOMMAND_NONE,
	SVGCOMMAND_MOVE_TO,
	SVGCOMMAND_LINE_TO,
	SVGCOMMAND_CURVE_TO,
	SVGCOMMAND_CLOSE_PATH,
	SVGCOMMAND_STROKE,
	SVGCOMMAND_FILL,
	SVGCOMMAND_FILL_AND_STROKE,
};

struct SvgCommand {
	SvgCommandType type;
	double values[6];
};

typedef std::vector<SvgCommand> SvgCommands;

#endif//__THE_ESCAPE__SVG_COMMAND_H_
