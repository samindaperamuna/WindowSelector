/*
* File:   Conversions.h
* Author: Saminda
*
* Created on 04 July 2018, 15:09
*/

#ifndef CONVERSIONS_H
#define CONVERSIONS_H

#ifdef __cplusplus
extern "C" {
#endif

	wchar_t* getWChar(const char*);
	std::size_t strlen_mb(const std::string);

#ifdef __cplusplus
}
#endif

#endif // !CONVERSIONS_H