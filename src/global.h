#ifndef GLOBAL_H
#define GLOBAL_H

enum Button { NONE, DRAW, ERASE, PAN };

#if not defined(EVILPIXIE_DATA_DIR)
  #define EVILPIXIE_DATA_DIR "./data"
#endif

#endif // GLOBAL_H

