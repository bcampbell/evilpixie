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
	virtual void OnDamaged(NodePath const& target, Box const& dmg) = 0;
    virtual void OnPaletteChanged(NodePath const& owner, int index, Colour const& c) = 0;
    virtual void OnPaletteReplaced(NodePath const&) = 0;
    virtual void OnModifiedFlagChanged(bool) = 0;
    virtual void OnFramesAdded(NodePath const& first, int count) = 0;
    virtual void OnFramesRemoved(NodePath const& first, int count) = 0;
    virtual void OnFramesBlatted(NodePath const& first, int count) = 0;
};

#endif // PROJECTLISTENER_H

