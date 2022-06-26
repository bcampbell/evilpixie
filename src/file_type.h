#ifndef FILE_TYPES_H
#define FILE_TYPES_H

#include <string>

enum Filetype {
    FILETYPE_UNKNOWN,
    FILETYPE_PNG,
    FILETYPE_GIF,
    FILETYPE_BMP,
    FILETYPE_JPEG,
    FILETYPE_TARGA,
    FILETYPE_PCX,
    FILETYPE_IFF_ILBM
};


Filetype FiletypeFromFilename( std::string const& filename);

#endif // FILE_TYPES_H
