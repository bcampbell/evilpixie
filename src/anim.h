#ifndef ANIM_H
#define ANIM_H

#include <vector>
#include "palette.h"
#include "colours.h"

class Img;
class Box;

class Anim
{
public:
    Anim();
    ~Anim();

    int NumFrames() const { return m_Frames.size(); }
    Img& GetFrame(int n) { return *m_Frames[n]; }
    Img const& GetFrameConst(int n) const { return *m_Frames[n]; }
    Palette& GetPalette() { return m_Palette; }
    void SetPalette( Palette const& pal ) { m_Palette=pal; }
    Palette const& GetPaletteConst() const { return m_Palette; }

    void Load(const char* filename);
    void Save(const char* filename);
    void Append(Img* img) { m_Frames.push_back(img); }
    void Zap();

    // transfer frames in range [srcfirst, srclast) to another animation
    void TransferFrames(int srcfirst, int srclast, Anim& dest, int destfirst);
    // work out bounds of selection of anim (ie union of frames)
    void CalcBounds(Box& bound, int first, int last) const;

    // frame rate control (in fps)
    int FPS() const { return m_FPS; }
    void SetFPS(int fps) { m_FPS=fps; }

    void Dump() const;

    PixelFormat Fmt() const;
private:
    std::vector< Img* > m_Frames;
    int m_FPS;
    Palette m_Palette;
};

#endif // ANIM_H

