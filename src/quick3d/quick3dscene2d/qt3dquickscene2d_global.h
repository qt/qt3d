// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICKSCENE2D_GLOBAL_H
#define QT3DQUICKSCENE2D_GLOBAL_H

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  if defined(QT_BUILD_3DQUICKSCENE2D_LIB)
#    define Q_3DQUICKSCENE2DSHARED_EXPORT Q_DECL_EXPORT
#  else
#    define Q_3DQUICKSCENE2DSHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_3DQUICKSCENE2DSHARED_EXPORT
#endif

QT_END_NAMESPACE

#endif // QT3DQUICKSCENE2D_GLOBAL_H
