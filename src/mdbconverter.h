#ifndef MDBCONVERTER_H
#define MDBCONVERTER_H

#ifdef USE_MDBTOOLS
    #include "mdbconverter_unix.h"
#else
    #include "mdbconverter_win.h"
#endif

#endif // MDBCONVERTER_H
