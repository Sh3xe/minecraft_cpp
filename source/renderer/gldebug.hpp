#pragma once

void _log_gl_errors(int line, const char* file);

#ifndef NDEBUG
#define log_gl_errors() _log_gl_errors(__LINE__, __FILE__)
#else
#define log_gl_errors() 
#endif