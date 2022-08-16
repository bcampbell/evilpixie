#ifndef FILE_SAVE_H
#define FILE_SAVE_H

#include <string>
#include "file_type.h"

class Box;
class Layer;
class Stack;

struct SaveRequirements
{
    bool cantSave;      // File format doesn't support saving.
    bool flatten;       // Flattening required.
    bool quantise;      // Quantise to palette required.
    bool noAnim;        // Need to deal with animation.
    //
    int sheetColumns;   // number of columns for spritesheet when converting
};

// Work out what operations are required to save the stack in the
// given file format.
SaveRequirements CheckSave(Stack const& stack, Filetype ft);
void SaveLayer(Layer const& layer, std::string const& filename, Box const& grid);

#endif // FILE_SAVE_H
