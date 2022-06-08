// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICKINPUT_GLOBAL_H
#define QT3DQUICKINPUT_GLOBAL_H

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  if defined(QT_BUILD_3DQUICKINPUT_LIB)
#    define Q_3DQUICKINPUTSHARED_EXPORT Q_DECL_EXPORT
#  else
#    define Q_3DQUICKINPUTSHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_3DQUICKINPUTSHARED_EXPORT
#endif

QT_END_NAMESPACE

#endif // QT3DQUICKINPUT_GLOBAL_H
