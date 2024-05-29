/* File: example.i */
%module example

%{
#define SWIG_FILE_WITH_INIT
#include "Mosaify/example.h"
%}

int fact(int n);