#ifndef FILE_LOAD_H
#define FILE_LOAD_H

#include <string>

class Layer;
struct ProjSettings;

Layer* LoadLayer(std::string const& filename, ProjSettings& projSettings);

#endif // FILE_LOAD_H
