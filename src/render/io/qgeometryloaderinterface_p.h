// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QGEOMETRYLOADERINTERFACE_P_H
#define QT3DRENDER_QGEOMETRYLOADERINTERFACE_P_H

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

#include <QtCore/QObject>

#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

class QIODevice;

namespace Qt3DCore {
class QGeometry;
}

namespace Qt3DRender {

class Q_3DRENDERSHARED_PRIVATE_EXPORT QGeometryLoaderInterface : public QObject
{
public:
    virtual Qt3DCore::QGeometry *geometry() const = 0;

    virtual bool load(QIODevice *ioDev, const QString &subMesh = QString()) = 0;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QGEOMETRYLOADERINTERFACE_P_H
