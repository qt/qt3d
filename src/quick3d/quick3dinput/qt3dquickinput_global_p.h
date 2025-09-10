// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DQUICKINPUT_GLOBAL_P_H
#define QT3DQUICKINPUT_GLOBAL_P_H

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

#include <Qt3DQuickInput/qt3dquickinput_global.h>
#include <QtQml/qqml.h>
#include <private/qglobal_p.h>

#define Q_3DQUICKINPUTSHARED_PRIVATE_EXPORT Q_3DQUICKINPUTSHARED_EXPORT

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Quick {

Q_3DQUICKINPUTSHARED_PRIVATE_EXPORT void Quick3DInput_initialize();
Q_3DQUICKINPUTSHARED_PRIVATE_EXPORT void Quick3DInput_registerType(const char *className, const char *quickName, int major, int minor);

template<class T, class E> void registerExtendedType(const char *className, const char *quickName,
                                                     const char *uri, int major, int minor, const char *name)
{
    qmlRegisterExtendedType<T, E>(uri, major, minor, name);
    Quick3DInput_registerType(className, quickName, major, minor);
}

template<class T> void registerType(const char *className, const char *quickName,
                                    const char *uri, int major, int minor, const char *name)
{
    qmlRegisterType<T>(uri, major, minor, name);
    Quick3DInput_registerType(className, quickName, major, minor);
}

} // Quick

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DQUICKINPUT_GLOBAL_P_H
