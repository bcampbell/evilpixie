#ifndef PROJECTLISTENER_H
#define PROJECTLISTENER_H

struct Colour;
struct Box;
struct ImgID;

// base class for things that want to be informed of
// changes to the Project.
//
class ProjectListener
{
public:
    virtual ~ProjectListener() {}
	virtual void OnDamaged( ImgID const& /*id*/, Box const& /*projdmg*/ ) {}
    virtual void OnPaletteChanged( int /*n*/, Colour const& /*c*/ ) {}
    virtual void OnPaletteReplaced(ImgID const&) {}
    virtual void OnModifiedFlagChanged( bool ) {}
    virtual void OnPenChange() {}
    virtual void OnFramesAdded(int /*first*/, int /*last*/) {}
    virtual void OnFramesRemoved(int /*first*/, int /*last*/) {}
    virtual void OnLayerReplaced() {}

private:

};

#endif // PROJECTLISTENER_H

