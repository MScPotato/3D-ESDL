#pragma once
#include <comdef.h>

#define WIDE2(x) L##x
#define DEFTOWIDE(x) WIDE2(x)

#define CHECK_HR(hr) if (FAILED(hr) ) \
{ \
_com_error error(hr); \
std::wstring errmsg = L"Failed HRESULT in file: \n"; \
errmsg += DEFTOWIDE(__FILE__); \
errmsg += L"\nIn function: \n"; \
errmsg += DEFTOWIDE(__FUNCTION__); \
errmsg += L"\nAt line: "; \
errmsg += std::to_wstring(__LINE__); \
errmsg += L"\nHRESULT: \n"; \
errmsg += error.ErrorMessage(); \
MessageBox(NULL, errmsg.c_str(), L"ERROR: BAD HRESULT", MB_ICONERROR); \
}