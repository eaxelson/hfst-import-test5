#ifndef _HFSTDLL_H_
#define _HFSTDLL_H_
#ifdef _MSC_VER
#ifdef HFSTEXPORT
#define HFSTDLL  __declspec(dllexport)
#else
#define HFSTDLL __declspec(dllimport)
#endif // HFSTEXPORT
#else
#define HFSTDLL
#endif // _MSC_VER
#endif // _HFSTDLL_H_
