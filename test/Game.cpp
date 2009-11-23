/*
 *  Game.cpp
 *  The Escape
 *
 *  Created by Paul Morelle on 06/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#include "Game.h"

#include <libxml/xpath.h>

#include "Exception.h"
#include "SvgDrawer.h"
#include "SvgObject.h"

const double PIXELS_PER_METER = 50.0;
const double METERS_PER_PIXEL = 1.0 / PIXELS_PER_METER;
const double GRAVITY_NORM = 9.81;

#if defined(__APPLE__) && defined(__MACH__)
#  include <CoreFoundation/CoreFoundation.h>

char *getOSXAppBundlePath() {
	static char path[1024];
	CFBundleRef mainBundle = CFBundleGetMainBundle();
	assert(mainBundle);
	
	CFURLRef mainBundleURL = CFBundleCopyBundleURL(mainBundle);
	assert(mainBundleURL);
	
	CFStringRef cfStringRef = CFURLCopyFileSystemPath( mainBundleURL, kCFURLPOSIXPathStyle);
	assert(cfStringRef);
	
	CFStringGetCString(cfStringRef, path, 1024, kCFStringEncodingASCII);
	
	CFRelease(mainBundleURL);
	CFRelease(cfStringRef);
	
	return path;
}

#define RESOURCES_DIR (getOSXAppBundlePath() + std::string("/Contents/Resources"))
#else //defined(__APPLE__) && defined(__MACH__)
#define RESOURCES_DIR ("./resources")
#endif//defined(__APPLE__) && defined(__MACH__)

Game::Game(): m_resources_dir(RESOURCES_DIR) {
	xmlXPathInit();
	
	b2AABB worldAABB;
	worldAABB.lowerBound.Set(-100.0f, -100.0f);
	worldAABB.upperBound.Set(1200.0f, 1200.0f);
	b2Vec2 gravity(0.0f, GRAVITY_NORM);
	
	m_box2d_world = new b2World(worldAABB, gravity, true);
	
	m_objects = SvgObject::load_svg_objects(m_resources_dir + "/level1.svg", *m_box2d_world);
}

Game::~Game(void) {
	SvgObjects::const_iterator it, itend = m_objects.end();
	for(it = m_objects.begin(); it != itend; ++ it) {
		delete *it;
	}
	m_objects.clear();
	
	delete m_box2d_world;
}

void Game::advance(double dt) {
	m_box2d_world->Step(dt, 75);
}

void Game::draw(cairo_t *cr) const {
	// Clear the screen
	cairo_set_source_rgb(cr, 0.0, 0.0, 0.0);
	cairo_paint(cr);
	
	// Draw the objects
	SvgObjects::const_iterator it, itend = m_objects.end();
	for(it = m_objects.begin(); it != itend; ++ it) {
		SvgDrawer(cr).evaluate(**it);
	}
}
