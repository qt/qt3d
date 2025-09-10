// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_GLOBAL_H
#define QT3DINPUT_GLOBAL_H

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  if defined(QT_BUILD_3DINPUT_LIB)
#    define Q_3DINPUTSHARED_EXPORT Q_DECL_EXPORT
#  else
#    define Q_3DINPUTSHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_3DINPUTSHARED_EXPORT
#endif

QT_END_NAMESPACE

#endif // QT3DINPUT_GLOBAL_H
