// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QSCENEEXPORTFACTORY_P_H
#define QT3DRENDER_QSCENEEXPORTFACTORY_P_H

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

#include <private/qt3drender_global_p.h>

#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QSceneExporter;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QSceneExportFactory
{
public:
    static QStringList keys();
    static QSceneExporter *create(const QString &name, const QStringList &args);
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QSCENEEXPORTFACTORY_P_H
