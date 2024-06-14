/* File : MosaifyPy.i */
%module MosaifyPy

%begin %{
#pragma warning(disable:4100 4127 4706)
%}

%{
#include "Mosaify/Mosaify.h"
#include "Image.h"
#include <filesystem>
#include <iostream>
namespace fs = std::__fs::filesystem;
using namespace std;
#include <Board.h>

const char * getMosaicPath(const Mosaify &mosaify){
    static string path = string(fs::temp_directory_path()) + string("/") + to_string(rand()) + string(".png");
    NJLIC::Image *img = new NJLIC::Image();
    mosaify.getMosaicImage(*img);
    ImageFileLoader::write(path, img);
    delete img;

    LibBoard::Board board;
    static string svg_path = string(fs::temp_directory_path()) + string("image.svg");
    LibBoard::Image image(path.c_str(), 0, 0, 200);
    board << image;
    board.saveSVG(svg_path.c_str(), LibBoard::PageSize::BoundingBox, 0.0, LibBoard::Unit::Centimeter);
    return svg_path.c_str();
}

const char * getMosaicPreviewPath(const Mosaify &mosaify){
    static string path = string(fs::temp_directory_path()) + string("/") + to_string(rand()) + string(".png");
    NJLIC::Image *img = new NJLIC::Image();
    mosaify.getMosaicImage(*img);
    ImageFileLoader::write(path, img);

    delete img;
    return path.c_str();
}

const char * getMosaicTilePreviewPath(const Mosaify &mosaify, Mosaify::TileId _id){
    static string path = string(fs::temp_directory_path()) + string("/") + to_string(rand()) + string(".png");
    NJLIC::Image *img = new NJLIC::Image();
    if(!mosaify.getTileImage(_id, *img)) {
       img->generate(mosaify.getTileSize(), mosaify.getTileSize(), 3);
    }
    ImageFileLoader::write(path, img);

    delete img;
    return path.c_str();
}
%}

%typemap(in) uint8* (char* buffer, Py_ssize_t length) {
    PyBytes_AsStringAndSize($input,&buffer,&length);
    $1 = (uint8*)buffer;
}

%include "Mosaify/Mosaify.h"
const char * getMosaicPath(const Mosaify &mosaify);
const char * getMosaicPreviewPath(const Mosaify &mosaify);
const char * getMosaicTilePreviewPath(const Mosaify &mosaify, Mosaify::TileId _id);
