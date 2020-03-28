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

#if 0
    // Layer access - 0 is bottom layer.

    Layer& GetLayer(int i) { return *m_Layers[i]; }
    Layer const& GetLayerConst(int i) const { return *m_Layers[i]; }
    int NumLayers() const { return (int)m_Layers.size(); }
    // Project takes ownership of layer.
    void InsertLayer(Layer* layer, int pos);
    Layer* DetachLayer(int pos);
#endif
    // TODO: figure this out
//    int NumFrames() const { return 1;};

    Layer& ResolveLayer(NodePath const& target) const {
        assert(target.sel == NodePath::SEL_MAIN);
        BaseNode *n = root;
        for (auto i : target.path) {
            n = n->children[i];
        }
        Layer* l = n->ToLayer();
        assert(l);  // must be layer!
        return *l;
    }

    // shortcuts. Maybe kill these?
    Img& GetImg(NodePath const& targ) const {
        return ResolveLayer(targ).GetImg(targ.frame);
    }

    Img const& GetImgConst(NodePath const& targ) const {
        return ResolveLayer(targ).GetImgConst(targ.frame);
    }

    Palette const& PaletteConst(NodePath const& targ) const {
        return ResolveLayer(targ).GetPaletteConst();
    }
    Palette& GetPalette(NodePath const& targ) const {
        return ResolveLayer(targ).GetPalette();
    }

#if 0
    Colour GetColour(int n) const { return PaletteConst().GetColour(n);}
    // TODO: KILL KILL KILL
    PixelFormat Fmt() const { return m_Layers[0]->GetImgConst(0).Fmt(); }
    // TODO: KILL KILL KILL
    int NumFrames() const { return m_Layers[0]->NumFrames(); }
#endif

    PenColour PickUpPen(NodePath const& target, Point const& pt) const;

    // TODO: KILL THESE
//    Palette const& PaletteConst() const { return m_Layers[0]->GetPaletteConst(); }
//    Colour GetColour(int n) const { return PaletteConst().GetColour(n);}

    // return current filename of project (empty string if no name)
    std::string const& Filename() const { return m_Filename; }

    // --------------------------------------
    // Notifcation fns. To be called when project is fiddled with.
    // --------------------------------------
	void NotifyDamage(NodePath const& target, Box const& b);

    // notify operations on frames
    void NotifyFramesAdded(NodePath const& first, int count);
    void NotifyFramesRemoved(NodePath const& first, int count);
    void NotifyFramesBlatted(NodePath const& first, int count);

    // notify palette modified
    void NotifyPaletteChange(NodePath const& owner, int first, int count);
    void NotifyPaletteReplaced(NodePath const& owner);

    void SetModifiedFlag( bool newmodifiedflag );


    // Return true if both paths share the same palette
    bool IsSamePalette(NodePath const& a, NodePath const& b) const;

    // --------------------------------------
    // DATA
    // --------------------------------------

    // The root stack of the project.
    Stack* root;

private:
    Project( Project const& );  // disallowed

	std::set< ProjectListener* > m_Listeners;

    ProjSettings m_Settings;

    bool m_Expendable;

    // has project been modified?
    bool m_Modified;

    // last known filename of project (empty string for none)
    std::string m_Filename;
};


#endif // PROJECT_H

