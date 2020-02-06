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

// remaps indexed image to another palette
Img* ConvertI8toI8(Img const& srcImg, Palette const& srcPalette, Palette const& destPalette);

#endif // IMG_CONVERT_H

