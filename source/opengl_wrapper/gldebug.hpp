#ifndef GLDEBUG_INCLUDED_
#define GLDEBUG_INCLUDED_

void _logGlErrors(int line, const char* file);

#ifndef NDEBUG
#define logGlErrors() _logGlErrors(__LINE__, __FILE__)
#else
#define logGlErrors() 
#endif

#endif