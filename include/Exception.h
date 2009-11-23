/*
 *  Exception.h
 *  The Escape
 *
 *  Created by Paul Morelle on 06/10/09.
 *  Copyright 2009 UTC. All rights reserved.
 *
 */

#ifndef __THE_ESCAPE__EXCEPTION_H__
#define __THE_ESCAPE__EXCEPTION_H__

#include <exception>

class Exception: public std::exception {
public:
	Exception(const std::string &message) throw(): m_message(message) {}
	virtual ~Exception() throw() {}
	const char *what(void) const throw() { return m_message.c_str(); }
	
private:
	const std::string m_message;
};

#endif//__THE_ESCAPE__EXCEPTION_H__
