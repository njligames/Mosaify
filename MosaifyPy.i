/* File : MosaifyPy.i */
%module MosaifyPy

%begin %{
#pragma warning(disable:4100 4127 4706)
%}

%{
#include "Mosaify/Mosaify.h"
%}

%typemap(in) uint8* (char* buffer, Py_ssize_t length) {
    PyBytes_AsStringAndSize($input,&buffer,&length);
    $1 = (uint8*)buffer;
}

%include "Mosaify/Mosaify.h"
