#ifndef IMGSUPPORT_H
#define IMGSUPPORT_H

bool LoadImg( IndexedImg& img, RGBx* palette, const char* filename, int* transparent_idx=0 );

void SaveImg( IndexedImg const& img, RGBx const* palette, const char* filename, int transparent_idx=-1 );

#endif

