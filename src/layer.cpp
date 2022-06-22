#include <cassert>

#include "layer.h"
#include "img.h"
#include "exception.h"
#include "util.h"


Layer* FindLayer(BaseNode* n)
{
    Layer* l = n->ToLayer();
    if (l) {
        return l;
    }
    for (auto child : n->mChildren) {
        l = FindLayer(child);
        if (l) {
            return l;
        }
    }
    return nullptr;
}

NodePath CalcPath(BaseNode *n)
{
    std::vector<int> trace;
    while(n->mParent) {
        int i;
        for (i = 0; i < (int)n->mParent->mChildren.size(); ++i) {
            if (n == n->mParent->mChildren[i]) {
                break;
            }
        }
        if (i >= (int)n->mParent->mChildren.size()) {
            // not found! should never get here.
            assert(false);
            return NodePath();
        }
        trace.push_back(i);
        n = n->mParent;
    }
    std::reverse(trace.begin(), trace.end());
    NodePath out;
    out.sel = NodePath::SEL_MAIN;
    out.path = trace;
    return out;
}


Layer::Layer() :
    mFPS(60),
    mRanges(8, 16)
{
}

Layer::~Layer()
{
    Zap();
}

void Layer::Zap()
{
    while( !mFrames.empty() )
    {
        delete mFrames.back();
        mFrames.pop_back();
    }
}

/*
void Layer::Dump() const
{
    printf("Dump---\n");
    unsigned int i;
    for (i=0; i<m_Frames.size(); ++i )
    {
        Img* im = m_Frames[i];
        printf("%d: %p", i, im);
        if(im) {
            Box const& b = im->Bounds();
            printf(" %d %d %d %d\n",b.x,b.y,b.w,b.h);
        } else {
            printf("\n");
        }
    }
}
*/

PixelFormat Layer::Fmt() const
    { return mFrames.front()->mImg->Fmt(); }

void Layer::CalcBounds(Box& bound, int /*first*/, int /*last*/) const
{
    //TODO: constrain to first..last!
    for (auto f : mFrames) {
        bound.Merge(f->mImg->Bounds());
    }
}

int Layer::FrameIndexClipped(uint64_t t) const
{
    assert(!mFrames.empty());
    uint64_t accum = 0;
    int idx = 0;
    for (auto f : mFrames) {
        accum += f->mDuration;
        if (t < accum) {
            break;
        }
        ++idx;
    }
    // clip to last frame.
    if (idx >= (int)mFrames.size()) {
        idx = (int)mFrames.size() - 1;
    }
    return idx;
}

uint64_t Layer::FrameTime(int frame) const
{
    assert(frame < (int)mFrames.size());
    uint64_t t = 0;
    for (int i=0; i<frame; ++i) {
        t += mFrames[i]->mDuration;
    }
    return t;
}

