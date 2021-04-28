#ifndef IMG_CONVERT_H
#define IMG_CONVERT_H

class Img;
class Palette;

// Helper functions to convert images into different formats.

// these two potentially lossy (remaps to destPalette);
Img* ConvertRGBA8toI8(Img const& srcImg, Palette const& destPalette);
Img* ConvertRGBX8toI8(Img const& srcImg, Palette const& destPalette);

// these two non-lossy
Img* ConvertI8toRGBX8(Img const& srcImg, Palette const& srcPalette);
Img* ConvertI8toRGBA8(Img const& srcImg, Palette const& srcPalette);

// this one adds alpha channel (set all alpha to 255)
Img* ConvertRGBX8toRGBA8(Img const& srcImg);

// this one trashes the alpha channel
Img* ConvertRGBA8toRGBX8(Img const& srcImg);

// Remap I8 to destPalette.
void RemapI8(Img& img, Palette const& srcPalette, Palette const& destPalette);

// Remap an RGBX8 to destPalette (picks the closest colours in destPalette).
void RemapRGBX8(Img& img, Palette const& destPalette);

// Remap an RGBA8 to destPalette (picks the closest colours in destPalette).
void RemapRGBA8(Img& img, Palette const& destPalette);

#endif // IMG_CONVERT_H

