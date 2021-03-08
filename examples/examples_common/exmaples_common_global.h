#pragma once

#include <QtCore/QtGlobal>

#ifndef QT_PROTOBUF_STATIC
    #ifdef EXAMPLES_COMMON_LIB
        #define UNVERSALLISTMODEL_EXPORT Q_DECL_EXPORT
    #else
        #define UNVERSALLISTMODEL_EXPORT Q_DECL_IMPORT
    #endif
#else
    #define UNVERSALLISTMODEL_EXPORT
#endif
