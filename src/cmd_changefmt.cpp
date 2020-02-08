#include "cmd_changefmt.h"
#include "img_convert.h"
#include "project.h"
#include "quantise.h"

Cmd_ChangeFmt::Cmd_ChangeFmt(Project& proj, int layerNum, PixelFormat newFmt, int nColours) :
    Cmd(proj,NOT_DONE),
    m_LayerNum(layerNum)
{
    Layer const& srcLayer = Proj().GetLayerConst(m_LayerNum);
    Palette const& srcPalette = Proj().PaletteConst();

    // Calculate a palette if we need one.
    if (nColours > 0 && srcLayer.NumFrames() > 0) {
        // TODO: for global palette, need to take all frames into consideration!!!
        std::vector<Colour> quantised;
        CalculatePalette(srcLayer.GetFrameConst(0), quantised, nColours, &srcPalette);
        Palette newPalette(nColours);
        for (int i=0; i<(int)quantised.size(); ++i) {
            newPalette.SetColour(i, quantised[i]);
        }
        m_FrameSwap.SetPalette(newPalette);
    }

    // populate frameswap with converted frames
    int n;
    for (n = 0; n < srcLayer.NumFrames(); ++n)
    {
        Img const& srcImg = srcLayer.GetFrameConst(n);
        Img* destImg = nullptr;
        switch (srcImg.Fmt()) {
            case FMT_I8:
                if (newFmt == FMT_RGBX8) {
                    destImg = ConvertI8toRGBX8(srcImg, srcPalette);
                } else if (newFmt == FMT_RGBA8) {
                    destImg = ConvertI8toRGBA8(srcImg, srcPalette);
                } else if (newFmt == FMT_I8) {
                    destImg = ConvertI8toI8(srcImg, srcPalette, m_FrameSwap.GetPaletteConst());
                }
                break;
            case FMT_RGBX8:
                if (newFmt == FMT_RGBA8) {
                    destImg = ConvertRGBX8toRGBA8(srcImg);
                } else if(newFmt == FMT_I8) {
                    destImg = ConvertRGBX8toI8(srcImg, m_FrameSwap.GetPaletteConst());
                }
                break;
            case FMT_RGBA8:
                if (newFmt == FMT_RGBX8) {
                    destImg = ConvertRGBA8toRGBX8(srcImg);
                } else if(newFmt == FMT_I8) {
                    destImg = ConvertRGBA8toI8(srcImg, m_FrameSwap.GetPaletteConst());
                }
                break;
        }
        assert(destImg);

        m_FrameSwap.Append(destImg);
    }
}


Cmd_ChangeFmt::~Cmd_ChangeFmt()
{
}


void Cmd_ChangeFmt::Swap()
{
    Layer& layer = Proj().GetLayer(m_LayerNum);
    Layer tmp;

    // swap the palettes
    tmp.SetPalette(layer.GetPaletteConst());
    layer.SetPalette(m_FrameSwap.GetPaletteConst());
    m_FrameSwap.SetPalette(tmp.GetPaletteConst());

    // swap the frames
    layer.TransferFrames(0, layer.NumFrames(), tmp, 0);
    m_FrameSwap.TransferFrames(0, m_FrameSwap.NumFrames(), layer, 0);
    assert(m_FrameSwap.NumFrames() == 0);
    tmp.TransferFrames(0, tmp.NumFrames(), m_FrameSwap, 0);

    // notify damage.
    Proj().NotifyLayerReplaced();
}

void Cmd_ChangeFmt::Do()
{
    Swap();
    SetState( DONE );
}

void Cmd_ChangeFmt::Undo()
{
    Swap();
    SetState( NOT_DONE );
}

