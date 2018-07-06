#include "stdafx.h"
#include "Convertions.h"

using namespace std;

wchar_t* getWChar(const char* str) {
	size_t outSize;
	size_t strSize = strlen_mb(str) + 1;
	wchar_t* wc = (wchar_t*)malloc(strSize * sizeof(wchar_t));
	mbstowcs_s(&outSize, wc, strSize, str, strSize - 1);

	return wc;
}

// the number of characters in a multibyte string is the sum of mblen()'s
// note: the simpler approach is std::mbstowcs(NULL, s.c_str(), s.size())
size_t strlen_mb(const string s) {
	size_t result = 0;
	const char* ptr = s.data();
	const char* end = ptr + s.size();
	mblen(NULL, 0); // reset the conversion state
	while (ptr < end) {
		int next = mblen(ptr, end - ptr);
		if (next == -1) {
			std::cout << "strlen_mb(): conversion error" << std::endl;
		}
		ptr += next;
		++result;
	}
	return result;
}