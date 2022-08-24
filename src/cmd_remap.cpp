#include "cmd_remap.h"
#include "img_convert.h"
#include "project.h"
//#include "quantise.h"

Cmd_Remap::Cmd_Remap(Project& proj, NodePath const& target, PixelFormat newFmt, Palette const& destPalette) :
    Cmd(proj,NOT_DONE),
    m_Target(target),
    m_Other(nullptr)
{
    Layer& srcLayer = proj.ResolveLayer(m_Target);

    // create a new layer, holding the converted data.
    m_Other = new Layer();
    m_Other->mFPS = srcLayer.mFPS;
    // TODO: handle palette policies.
    m_Other->mPalette = destPalette;
    m_Other->mRanges = srcLayer.mRanges;
    m_Other->mRanges.Remap(m_Other->mPalette);

    // populate frameswap with the converted frames
    // TODO: handle palette policies.
    Palette const& srcPalette = srcLayer.mPalette;
    for (auto srcFrame : srcLayer.mFrames) {
        Frame* destFrame = ConvertFrame(srcFrame, newFmt, srcPalette, destPalette);
        m_Other->mFrames.push_back(destFrame);
    }
    // May also have SPARE_FRAME.
    if (srcLayer.mSpare) {
        m_Other->mSpare = ConvertFrame(srcLayer.mSpare, newFmt, srcPalette, destPalette);
    }
}


Cmd_Remap::~Cmd_Remap()
{
    delete m_Other;
}


void Cmd_Remap::Swap()
{
    Layer& l = Proj().ResolveLayer(m_Target);
    l.Replace(m_Other);
    m_Other = &l;

    // TODO: palette change doesn't register? current pen stays same.
    Proj().NotifyFramesBlatted(m_Target, 0, (int)l.mFrames.size());
}

void Cmd_Remap::Do()
{
    Swap();
    SetState( DONE );
}

void Cmd_Remap::Undo()
{
    Swap();
    SetState( NOT_DONE );
}


Frame* Cmd_Remap::ConvertFrame(Frame const* srcFrame, PixelFormat newFmt,
    Palette const& srcPalette, Palette const& destPalette) const
{
    Img const& srcImg = *srcFrame->mImg;
    Img* destImg = nullptr;
    switch (srcImg.Fmt()) {
    case FMT_I8:
        if (newFmt == FMT_I8) {
            destImg = new Img(srcImg);
            RemapI8(*destImg, srcPalette, destPalette);
        } else if (newFmt == FMT_RGBX8) {
            destImg = ConvertI8toRGBX8(srcImg, srcPalette);
        } else if (newFmt == FMT_RGBA8) {
            destImg = ConvertI8toRGBA8(srcImg, srcPalette);
        }
        break;
    case FMT_RGBX8:
        if(newFmt == FMT_I8) {
            destImg = ConvertRGBX8toI8(srcImg, destPalette);
        } else if (newFmt == FMT_RGBX8) {
            destImg = new Img(srcImg);
            RemapRGBX8(*destImg, destPalette);
        } else if (newFmt == FMT_RGBA8) {
            destImg = ConvertRGBX8toRGBA8(srcImg);
        }
        break;
    case FMT_RGBA8:
        if(newFmt == FMT_I8) {
            destImg = ConvertRGBA8toI8(srcImg, destPalette);
        } else if (newFmt == FMT_RGBX8) {
            destImg = ConvertRGBA8toRGBX8(srcImg);
        } else if (newFmt == FMT_RGBA8) {
            destImg = new Img(srcImg);
            RemapRGBA8(*destImg, destPalette);
        }
        break;
    }
    assert(destImg);

    Frame* destFrame = new Frame();
    destFrame->mDuration = srcFrame->mDuration;
    destFrame->mImg = destImg;
    return destFrame;
}
