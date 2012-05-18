/**

	File:		StackMap.h

	Project:	DCPU-16 Tools
	Component:	LibDCPU-ci-lang

	Authors:	James Rhodes

	Description:	Declares the StackMap typedef.

**/

#ifndef __DCPU_LIBDCC_STACKMAP_H
#define __DCPU_LIBDCC_STACKMAP_H

#include <string>
#include <vector>
#include <exception>
#include <stdexcept>

class IType;

// Work-around for GNU C++; can't just typedef here as we need to provide
// an assignment operator so that we can use it inside std::vector.
class StackPair
{
	public:
		std::string first;
		IType* second;

		StackPair(std::string key, IType* value) : first(key), second(value) { };
		inline StackPair& operator=(const StackPair& other)
		{
			throw new std::runtime_error("Attempt to use assignment operator on partial-const class IType (internal error)!");
		}
};
typedef std::vector<StackPair> StackMap;

#endif
