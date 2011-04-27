#ifndef ANIM_H
#define ANIM_H

#include <vector>

class IndexedImg;
class Palette;

class Anim
{
public:
    Anim();
    ~Anim();

    int NumFrames() const { return m_Frames.size(); }
    IndexedImg& GetFrame(int n) { return *m_Frames[n]; }
    IndexedImg const& GetFrameConst(int n) const { return *m_Frames[n]; }
    Palette& GetPalette(int n) { return *m_Palettes[n]; }
    Palette const& GetPaletteConst(int n) const { return *m_Palettes[n]; }

    void Load(const char* filename);
    void Append(IndexedImg* img, Palette* palette ) { m_Frames.push_back(img); m_Palettes.push_back(palette); }
    void Zap();
private:
    std::vector< IndexedImg* > m_Frames;
    std::vector< Palette* > m_Palettes;
};

#endif // ANIM_H

