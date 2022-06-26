#include "file_type.h"

#include "util.h"

// A best-guess attempt for picking a format based solely upon name.
// If loading, it's no substitute for sniffing file content.
// Some extensions are ambiguous (eg iff can be ilbm or maya or something
// else entirely)
Filetype FiletypeFromFilename( std::string const& filename)
{
    std::string ext = ToLower(ExtName(filename));
    if (ext == ".png") {
        return FILETYPE_PNG;
    }
    if (ext == ".gif") {
        return FILETYPE_GIF;
    }
    if (ext == ".jpg" || ext == ".jpeg") {
        return FILETYPE_JPEG;
    }
    if (ext == ".bmp") {
        return FILETYPE_BMP;
    }
    if (ext == ".tga") {
        return FILETYPE_TARGA;
    }
    if (ext == ".pcx") {
        return FILETYPE_PCX;
    }
    if (ext == ".iff" || ext == ".ilbm" || ext == ".lbm") {
        return FILETYPE_IFF_ILBM;
    }
    return FILETYPE_UNKNOWN;
}

