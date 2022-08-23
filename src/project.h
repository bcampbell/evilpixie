#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>
#include <list>
#include <set>
#include <vector>
#include <string>

#include "box.h"
#include "colours.h"
#include "img.h"
#include "layer.h"
#include "palette.h"
#include "point.h"
#include "ranges.h"

class Tool;
class ProjectListener;


// General project settings
struct ProjSettings
{
    std::string Filename;

    //
    Box Grid {0,0,8,8};

    // Grid layout to use when converting this layer to/from a spritesheet
    // use IsZero() to check if set or not.
    SpriteGrid SpriteSheetGrid;

    // pixel ratio. Usually 1:1 but might be 2:1 (C64 multicolour)
    // or 1:2 (Amiga hires) say...
    // TODO: should be in LayerSettings!
    int PixW {1};
    int PixH {1};
};




// Project holds all the frames and image data.
// Listeners can register with a project to hear whenever changes are made.

class Project
{
public:
    Project();

    // palette ownership taken by project
	Project(PixelFormat fmt, int w, int h, Palette* palette=0, int num_frames=1);
    Project(std::string const& filename);
    // Project takes ownership of layer.
    Project(Layer* layer);
	virtual ~Project();


    ProjSettings const& Settings() { return mSettings; }

	void AddListener( ProjectListener* l )
		{ m_Listeners.insert( l ); }
	void RemoveListener( ProjectListener* l )
		{ m_Listeners.erase( l ); }


    bool ModifiedFlag() const { return m_Modified; }

    // expendable is set if project is default and unmodified, and can be
    // deleted without care (eg if user loads another project)
    bool Expendable() const { return m_Expendable; }

    Layer& ResolveLayer(NodePath const& target) const {
        assert(!target.IsEmpty());
        BaseNode *n = mRoot;
        for (auto i : target.path) {
            n = n->mChildren[i];
        }
        Layer* l = n->ToLayer();
        assert(l);  // must be layer!
        return *l;
    }

    // shortcuts. Maybe kill these?
    Img& GetImg(NodePath const& target, int frame) const {
        return ResolveLayer(target).GetImg(frame);
    }

    Img const& GetImgConst(NodePath const& target, int frame) const {
        return ResolveLayer(target).GetImgConst(frame);
    }

    Palette const& PaletteConst(NodePath const& target, int /*frame*/) const {
        // TODO: implement shared-palette policy
        return ResolveLayer(target).GetPaletteConst();
    }
    Palette& GetPalette(NodePath const& target, int /*frame*/) const {
        // TODO: implement shared-palette policy
        return ResolveLayer(target).GetPalette();
    }

    RangeGrid& Ranges(NodePath const& target, int /*frame*/) const {
        // TODO: implement shared-palette policy
        return ResolveLayer(target).mRanges;
    }

    PenColour PickUpPen(NodePath const& target, int frame, Point const& pt) const;


    // return current filename of project (empty string if no name)
    std::string const& Filename() const { return mFilename; }

    // --------------------------------------
    // Notifcation fns. To be called when project is fiddled with.
    // --------------------------------------
	void NotifyDamage(NodePath const& target, int frame, Box const& b);

    // notify operations on frames
    void NotifyFramesAdded(NodePath const& target, int first, int count);
    void NotifyFramesRemoved(NodePath const& target, int first, int count);
    void NotifyFramesBlatted(NodePath const& target, int first, int count);

    // notify palette modified
    void NotifyPaletteChange(NodePath const& target, int frame, int index, int count);
    void NotifyPaletteReplaced(NodePath const& target, int frame);

    void NotifyRangesBlatted(NodePath const& target, int frame);
 
    void SetModifiedFlag( bool newmodifiedflag );

    // Return true if both paths share the same palette
    bool SharesPalette(NodePath const& a, int frameA, NodePath const& b, int frameB) const;

    // --------------------------------------
    // DATA
    // --------------------------------------

    // The root stack of the project.
    Stack* mRoot;

    // last known filename of project (empty string for none)
    std::string mFilename;

    ProjSettings mSettings;
private:
    Project( Project const& );  // disallowed

	std::set< ProjectListener* > m_Listeners;

    bool m_Expendable;

    // has project been modified?
    bool m_Modified;

};


#endif // PROJECT_H

