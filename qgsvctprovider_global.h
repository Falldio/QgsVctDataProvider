#pragma once

#include <QtCore/qglobal.h>

#ifndef BUILD_STATIC
# if defined(QGSVCTPROVIDER_LIB)
#  define QGSVCTPROVIDER_EXPORT Q_DECL_EXPORT
# else
#  define QGSVCTPROVIDER_EXPORT Q_DECL_IMPORT
# endif
#else
# define QGSVCTPROVIDER_EXPORT
#endif
