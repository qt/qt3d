// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_GLOBAL_H
#define QT3DRENDER_GLOBAL_H

#include <Qt3DCore/qt3dcore_global.h>
#include <Qt3DRender/qt3drender-config.h>

QT_BEGIN_NAMESPACE

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  if defined(QT_BUILD_3DRENDER_LIB)
#    define Q_3DRENDERSHARED_EXPORT Q_DECL_EXPORT
#  else
#    define Q_3DRENDERSHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_3DRENDERSHARED_EXPORT
#endif

QT_END_NAMESPACE

#endif // QT3DRENDER_GLOBAL_H
