#pragma once

#ifdef MOREFUNCS_EXPORTS
#define MOREFUNCS_API __declspec(dllexport)
#else
#define MOREFUNCS_API __declspec(dllimport)
#endif

