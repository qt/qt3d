// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/private/qgeometryloaderfactory_p.h>

#include "fbxgeometryloader.h"

QT_BEGIN_NAMESPACE

class FBXGeometryLoaderPlugin : public Qt3DRender::QGeometryLoaderFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QGeometryLoaderFactory_iid FILE "fbx.json")
public:

    QStringList keys() const override
    {
        return QStringList() << FBXGEOMETRYLOADER_EXT;
    }

    Qt3DRender::QGeometryLoaderInterface* create(const QString& ext) override
    {
        if ((ext.compare(FBXGEOMETRYLOADER_EXT, Qt::CaseInsensitive) == 0))
            return new Qt3DRender::FbxGeometryLoader;
        return nullptr;
    }
};

QT_END_NAMESPACE

#include "main.moc"
