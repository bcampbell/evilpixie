#ifndef PROJECT_H
#define PROJECT_H

#include <stdint.h>
#include <list>
#include <set>
#include <vector>
#include <string>

#include "anim.h"
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

    void Save( std::string const& filename, bool savetransparency );

    // pixel access
    Anim& GetAnim() { return m_Anim; }
    Anim const& GetAnimConst() const { return m_Anim; }
    // shortcuts
    //Img& Img(int frame) { return m_Anim.GetFrame(frame); }
    Img const& ImgConst(int frame) const { return m_Anim.GetFrameConst(frame); }


    PixelFormat Fmt() const { return m_Anim.GetFrameConst(0).Fmt(); }
    int NumFrames() const { return m_Anim.NumFrames(); }

    PenColour PickUpPen(Point const& pt, int frame) const;

    // kill!
    // palette manipulation
	Colour const& GetColour(int n ) const { return GetAnimConst().GetPaletteConst().GetColour(n); }
	void SetColour( int n, Colour const& c ) { GetAnim().GetPalette().SetColour(n,c); }
//    Colour const* GetPaletteConst() const { return GetAnimConst().GetPaletteConst().rawconst(); }

    Palette const& PaletteConst() const { return m_Anim.GetPaletteConst(); }

    // return current filename of project (empty string if no name)
    std::string const& Filename() const { return m_Filename; }

    //--------------------------------------
    // Interface for Cmd to use
    //
    // tell project it's been modified
    // (within draw operation, use Draw_Damage() instead!)
	void Damage( int frame, Box const& b );


    // notify operations on frames in range [first,last)
    void Damage_FramesAdded(int first, int last);
    void Damage_FramesRemoved(int first, int last);


    // TODO: move these out of project into their own class (like DrawTransaction)
    void PaletteChange_Begin();
    void PaletteChange_Alter( int n, Colour const& c );
    void PaletteChange_Replace( Palette const& p );
    void PaletteChange_Commit();
    void PaletteChange_Rollback();


    void SetModifiedFlag( bool newmodifiedflag );
private:
    Project( Project const& );  // disallowed

	std::set< ProjectListener* > m_Listeners;

    ProjSettings m_Settings;

    bool m_Expendable;


    // anim stores all the image data (even if it's a single frame anim :-)
    Anim m_Anim;


    // has project been modified?
    bool m_Modified;

    // last known filename of project (empty string for none)
    std::string m_Filename;
};


#endif // PROJECT_H

