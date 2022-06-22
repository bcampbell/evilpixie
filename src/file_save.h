#ifndef FILE_SAVE_H
#define FILE_SAVE_H

#include <string>

class Layer;

void SaveLayer(Layer const& layer, std::string const& filename);

#endif // FILE_SAVE_H
