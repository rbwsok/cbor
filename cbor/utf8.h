#ifndef __H_UTF8_H_
#define __H_UTF8_H_

#include <string>

std::string wstrTOutf8(const std::wstring& in);
std::string wstrTOutf8(const wchar_t* in, int sizein);
std::wstring utf8TOwstr(const std::string& in);
std::wstring utf8TOwstr(const char* in, size_t sizein);
std::wstring wstrTOwstr(const unsigned short* in, size_t sizein);

#endif
