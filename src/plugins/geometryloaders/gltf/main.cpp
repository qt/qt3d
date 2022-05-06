/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd and/or its subsidiary(-ies).
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
******************************************************************************/

#include <Qt3DRender/private/qgeometryloaderfactory_p.h>

#include "gltfgeometryloader.h"

QT_BEGIN_NAMESPACE

class GLTFGeometryLoaderPlugin : public Qt3DRender::QGeometryLoaderFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QGeometryLoaderFactory_iid FILE "gltf.json")
public:

    QStringList keys() const override
    {
        return QStringList() << GLTFGEOMETRYLOADER_EXT
                             << JSONGEOMETRYLOADER_EXT
                             << QGLTFGEOMETRYLOADER_EXT;
    }

    Qt3DRender::QGeometryLoaderInterface *create(const QString &ext) override
    {
        if ((ext.compare(GLTFGEOMETRYLOADER_EXT, Qt::CaseInsensitive) == 0) ||
            (ext.compare(JSONGEOMETRYLOADER_EXT, Qt::CaseInsensitive) == 0) ||
            (ext.compare(QGLTFGEOMETRYLOADER_EXT, Qt::CaseInsensitive) == 0))
            return new Qt3DRender::GLTFGeometryLoader;
        return nullptr;
    }
};

QT_END_NAMESPACE

#include "main.moc"
