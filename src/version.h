#ifndef VERSION_H
#define VERSION_H

#include "clientversion.h"

static const int CLIENT_VERSION =
                           1000000 * CLIENT_VERSION_MAJOR
                         +   10000 * CLIENT_VERSION_MINOR
                         +     100 * CLIENT_VERSION_REVISION
                         +       1 * CLIENT_VERSION_BUILD;


extern const QString CLIENT_BUILD;

#define DISPLAY_VERSION_MAJOR    CLIENT_VERSION_MAJOR
#define DISPLAY_VERSION_MINOR    CLIENT_VERSION_MINOR
#define DISPLAY_VERSION_REVISION CLIENT_VERSION_REVISION
#define DISPLAY_VERSION_BUILD    CLIENT_VERSION_BUILD

#endif
