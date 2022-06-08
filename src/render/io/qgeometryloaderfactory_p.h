// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QGEOMETRYLOADERFACTORY_P_H
#define QT3DRENDER_QGEOMETRYLOADERFACTORY_P_H

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
#include <QtCore/QtPlugin>
#include <QtCore/QFactoryInterface>

#include <Qt3DRender/private/qt3drender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

#define QGeometryLoaderFactory_iid "org.qt-project.Qt3DRender.QGeometryLoaderFactory 5.8"

class QGeometryLoaderInterface;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QGeometryLoaderFactory : public QObject, public QFactoryInterface
{
    Q_OBJECT
    Q_INTERFACES(QFactoryInterface)
public:
    explicit QGeometryLoaderFactory(QObject *parent = nullptr);
    virtual ~QGeometryLoaderFactory();

    virtual QGeometryLoaderInterface *create(const QString &ext) = 0;
};

} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_QGEOMETRYLOADERFACTORY_P_H
