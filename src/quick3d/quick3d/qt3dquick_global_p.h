// Copyright (C) 2020 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only
#ifndef QT3DQUICK_GLOBAL_P_H
#define QT3DQUICK_GLOBAL_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <Qt3DQuick/qt3dquick_global.h>
#include <QtQml/qqml.h>
#include <private/qglobal_p.h>

#include <QColor>
#include <QMatrix4x4>
#include <QVector2D>
#include <QVector3D>

#define Q_3DQUICKSHARED_PRIVATE_EXPORT Q_3DQUICKSHARED_EXPORT

QT_BEGIN_NAMESPACE

namespace Qt3DCore {
namespace Quick {

Q_3DQUICKSHARED_PRIVATE_EXPORT void Quick3D_initialize();
Q_3DQUICKSHARED_PRIVATE_EXPORT void Quick3D_uninitialize();
Q_3DQUICKSHARED_PRIVATE_EXPORT void Quick3D_registerType(const char *className, const char *quickName, int major, int minor);

template<class T, class E> void registerExtendedType(const char *className, const char *quickName,
                                                     const char *uri, int major, int minor, const char *name)
{
    qmlRegisterExtendedType<T, E>(uri, major, minor, name);
    Quick3D_registerType(className, quickName, major, minor);
}

} // namespace Quick
} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DQUICK_GLOBAL_P_H
