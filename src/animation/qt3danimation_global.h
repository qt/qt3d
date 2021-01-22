/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

#ifndef QT3DANIMATION_GLOBAL_H
#define QT3DANIMATION_GLOBAL_H

#include <Qt3DCore/qt3dcore_global.h>

QT_BEGIN_NAMESPACE

#if defined(QT_SHARED) || !defined(QT_STATIC)
#  if defined(QT_BUILD_3DANIMATION_LIB)
#    define Q_3DANIMATIONSHARED_EXPORT Q_DECL_EXPORT
#  else
#    define Q_3DANIMATIONSHARED_EXPORT Q_DECL_IMPORT
#  endif
#else
#  define Q_3DANIMATIONSHARED_EXPORT
#endif

QT_END_NAMESPACE

#endif // QT3DANIMATION_GLOBAL_H
