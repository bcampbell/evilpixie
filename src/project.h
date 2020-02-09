#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>
#include <list>
#include <set>
#include <vector>
#include <string>

#include "layer.h"
#include "point.h"
#include "colours.h"
#include "palette.h"
#include "box.h"
#include "img.h"

class Tool;
class ProjectListener;

// id to get at images within the project.
struct ImgID
{
    ImgID(int layerIdx = -1, int frameIdx = -1) :
        layer(layerIdx),
        frame(frameIdx)
    {}

    int layer;  // -1 = global: all layers and frames
    int frame;  // -1 = all frames in layer
};

inline bool operator==(ImgID const& a, ImgID const&b) {
    return a.layer == b.layer && a.frame == b.frame;
}

inline bool operator!=(ImgID const& a, ImgID const&b) {
    return a.layer != b.layer || a.frame != b.frame;
}


struct ProjSettings
{
    ProjSettings() :
        PixW(1),
        PixH(1)
    {}

    // pixel ratio. Usually 1:1 but might be 2:1 (C64 multicolour)
    // or 1:2 (Amiga hires) say...
    int PixW;
    int PixH;
};




// Project holds all the frames and image data.
// Listeners can register with a project to hear whenever changes are made.

class Project
{
public:
    Project();
	Project( PixelFormat fmt, int w, int h, Palette* palette=0, int num_frames=1 );
    Project(std::string const& filename);
	virtual ~Project();


    ProjSettings const& Settings() { return m_Settings; }

	void AddListener( ProjectListener* l )
		{ m_Listeners.insert( l ); }
	void RemoveListener( ProjectListener* l )
		{ m_Listeners.erase( l ); }


    bool ModifiedFlag() const { return m_Modified; }

    // expendable is set if project is default and unmodified, and can be
    // deleted without care (eg if user loads another project)
    bool Expendable() const { return m_Expendable; }

    // replace palette, informing listeners. ownership is passed to proj.
    void ReplacePalette(Palette* newpalette);

    void Save( std::string const& filename);

    // pixel access
    Layer& GetLayer(int i) { return *m_Layers[i]; }
    Layer const& GetLayerConst(int i) const { return *m_Layers[i]; }
    int NumLayers() const { return (int)m_Layers.size(); }

    // shortcuts
    Img& GetImg(ImgID const& id) {
        assert(id.layer >= 0 && id.layer < (int)m_Layers.size());
        Layer* l = m_Layers[id.layer];
        return l->GetFrame(id.frame);
    }
    Img const& GetImgConst(ImgID const& id) const {
        assert(id.layer >= 0 && id.layer < (int)m_Layers.size());
        Layer const* l = m_Layers[id.layer];
        Img const& img = l->GetFrameConst(id.frame);
        assert(img.Bounds().x==0);
        return img;
    }


    PixelFormat Fmt() const { return m_Layer.GetFrameConst(0).Fmt(); }
    int NumFrames() const { return m_Layer.NumFrames(); }

    PenColour PickUpPen(ImgID const& id, Point const& pt) const;

    // TODO: KILL THESE
    Palette const& PaletteConst() const { return m_Layer.GetPaletteConst(); }
    Colour GetColour(int n) const { return PaletteConst().GetColour(n);}

    // return current filename of project (empty string if no name)
    std::string const& Filename() const { return m_Filename; }

    // --------------------------------------
    // Notifcation fns. To be called when project is fiddled with.
    // --------------------------------------
	void NotifyDamage(ImgID const& target, Box const& b);

    // notify operations on frames in range [first,last)
    void NotifyFramesAdded(int first, int last);
    void NotifyFramesRemoved(int first, int last);

    // Layer completely changed.
    void NotifyLayerReplaced();

    // notify palette modified
    void NotifyPaletteChange(int first, int cnt);
    void NotifyPaletteReplaced(ImgID const& id);

    void SetModifiedFlag( bool newmodifiedflag );
private:
    Project( Project const& );  // disallowed

	std::set< ProjectListener* > m_Listeners;

    ProjSettings m_Settings;

    bool m_Expendable;

    std::vector<Layer*> m_Layers;
    Layer m_Layer;  // KILL KILL KILL!


    // has project been modified?
    bool m_Modified;

    // last known filename of project (empty string for none)
    std::string m_Filename;
};


#endif // PROJECT_H

