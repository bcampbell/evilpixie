#ifndef LAYER_H
#define LAYER_H

#include <vector>
#include <algorithm>
#include <string>

#include "box.h"
#include "colours.h"
#include "img.h"
#include "palette.h"
#include "point.h"
#include "ranges.h"
#include "sheet.h"

class Layer;
class Stack;


//
struct NodePath {
    enum {SEL_NULL=0, SEL_MAIN, SEL_SPARE} sel;
    std::vector<int> path;
    //int frame;  // frame within target (if layer)

    NodePath() : sel(SEL_NULL) {}

    bool IsNull() const {
        return sel == SEL_NULL;
    }

    bool operator==(NodePath const& other) const {
        return (sel == other.sel && path == other.path);
    }
    bool operator!=(NodePath const& other) const {
        return (!(*this == other));
    }

    void dump() {
        printf("%d:", sel);
        for( auto i : path) {
            printf("%d/",i);
        }
        printf("\n");
    }
};


// Base class for anything that can be part of the layer stack.
// Think of it as like a DOM node in a web browser - it has
// a parent and child nodes. The root's parent is null.
class BaseNode {
public:
    // The data:
    std::string mName;
    Point mOffset;
    BaseNode* mParent;  // root stack has null parent
    std::vector<BaseNode*> mChildren;
    // opacity, visibility, composite-op?

    BaseNode() : mOffset(0,0), mParent(nullptr) {
    }

    virtual ~BaseNode() {
        assert(!mParent);    // still attached!
        for (auto child : mChildren) {
            child->mParent = nullptr;
            delete child;
        }
    }

    // cheesy rtti
    virtual bool IsLayer() const {return false;}
    virtual Layer* ToLayer() {return nullptr;}
    virtual Layer const* ToLayerConst() const {return nullptr;}
    virtual bool IsStack() const {return false;}
    virtual Stack* ToStack() {return nullptr;}
    virtual Stack const* ToStackConst() const {return nullptr;}

    // transfers ownership - this node now owns n.
    void AddChild(BaseNode* n) {
        assert(!IsLayer());
        assert(!n->mParent);
        n->mParent = this;
        mChildren.push_back(n);
    }

    // detach from mParent
    void Detach() {
        assert(mParent);
        auto it = std::find(mParent->mChildren.begin(), mParent->mChildren.end(), this);
        assert(it != mParent->mChildren.end());
        mParent->mChildren.erase(it);
        mParent = nullptr;
    }

    // Replace this node with another one.
    // This node is detached and the parent takes ownership of other.
    void Replace(BaseNode *other) {
        assert(mParent);
        if(other->mParent) {
            other->Detach();
        }
        auto it = std::find(mParent->mChildren.begin(), mParent->mChildren.end(), this);
        assert(it != mParent->mChildren.end());
        *it = other;
        other->mParent = mParent;
        mParent = nullptr;
    }

    void WalkConst(auto visit) const {
        visit(this);
        for (auto const child: mChildren) {
            child->WalkConst(visit);
        }
    }
};

// Starting at n, find the first layer.
Layer* FindLayer(BaseNode* n);

// Return a path from root to the given node layer.
NodePath CalcPath(BaseNode *n);


// A Stack groups multiple Layers (and/or other Stacks).
class Stack : public BaseNode {
public:
    virtual bool IsStack() const {return true;}
    virtual Stack* ToStack() {return this;}
    virtual Stack const* ToStackConst() const {return this;}
};



class Frame {
public:
    // Frame owns the Img object
    Img* mImg;
    // How long this frame should be displayed, in microsecs.
    int mDuration;
    // Can have per-frame palette
    // Palette mPalette;

    Frame(Img* img, int duration) : mImg(img), mDuration(duration) {}
    Frame() : mImg(nullptr), mDuration(0) {}
    ~Frame() { delete mImg; }
};


// Layers are where all the image data is stashed.
// They should never have child nodes.
class Layer : public BaseNode
{
public:
    Layer();
    ~Layer();

    virtual bool IsLayer() const {return true;}
    virtual Layer* ToLayer() {return this;}
    virtual Layer const* ToLayerConst() const {return this;}

    int NumFrames() const { return mFrames.size(); }
    Img& GetImg(int n) {
        assert(n >= 0 && n < (int)mFrames.size());
        return *mFrames[n]->mImg;
    }
    Img const& GetImgConst(int n) const {
        assert(n >= 0 && n < (int)mFrames.size());
        return *mFrames[n]->mImg;
    }
    // TODO: account for frames...
    Palette& GetPalette() { return mPalette; }
//    void SetPalette(Palette const& pal) { mPalette=pal; }
    Palette const& GetPaletteConst() const { return mPalette; }

    // Kill this.
    void Append(Img* img) {
        Frame* f = new Frame();
        f->mDuration = 1000000/mFPS;
        f->mImg = img;
        mFrames.push_back(f);
    }
    void ZapFrames();

    // transfer frames in range [srcfirst, srclast) to another Layer
    void TransferFrames(int srcfirst, int srclast, Layer& dest, int destfirst);
    // Work out bounds, taking all frames into consideration.
    Box Bounds() const;

    // frame rate control (in fps)
    int FPS() const { return mFPS; }
    void SetFPS(int fps) { mFPS=fps; }

    void Dump() const;

    PixelFormat Fmt() const;


    // Find frame index for time t (clips to last frame).
    int FrameIndexClipped(uint64_t t) const;

    // Calculate start time of frame (in microseconds).
    uint64_t FrameTime(int frame) const;


    // DATA

    std::vector<Frame*> mFrames;

    int mFPS;
    Palette mPalette;
    RangeGrid mRanges;

    std::string mFilename;

    // Grid layout to use when converting this layer to/from a spritesheet
    // use IsZero() to check if set or not.
    SpriteGrid mSpriteSheetGrid;
};

#endif // LAYER_H


