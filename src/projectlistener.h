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
    virtual void OnPaletteReplaced(NodePath const&) {}
    virtual void OnModifiedFlagChanged( bool ) {}
    virtual void OnFramesAdded(NodePath const& first, int /*cnt*/) {}
    virtual void OnFramesRemoved(NodePath const& first, int /*last*/) {}
    virtual void OnLayerReplaced() {}

private:

};

#endif // PROJECTLISTENER_H

