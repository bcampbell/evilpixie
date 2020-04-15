#ifndef PROJECTLISTENER_H
#define PROJECTLISTENER_H

struct Colour;
struct Box;
struct NodePath;

// base class for things that want to be informed of
// changes to the Project.
//
class ProjectListener
{
public:
    virtual ~ProjectListener() {}
	virtual void OnDamaged(NodePath const& /* target */, int /*frame*/, Box const& /*dmg*/) {}
    virtual void OnPaletteChanged(NodePath const& /*target*/, int /*frame*/, int /*index*/, Colour const& /*c*/) {}
    virtual void OnPaletteReplaced(NodePath const& /*target*/, int /*frame*/) {}
    virtual void OnRangesBlatted(NodePath const& /*target*/, int /*frame*/) {}
    virtual void OnModifiedFlagChanged(bool) {}
    virtual void OnFramesAdded(NodePath const& /*target*/, int /*first*/, int /*count*/) {}
    virtual void OnFramesRemoved(NodePath const& /*target*/, int /*first*/, int /*count*/) {}
    virtual void OnFramesBlatted(NodePath const& /*target*/, int /*first*/, int /*count*/) {}
};

#endif // PROJECTLISTENER_H

