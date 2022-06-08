// Copyright (C) 2017 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include <Qt3DRender/private/qgeometryloaderfactory_p.h>

#include "objgeometryloader.h"
#include "plygeometryloader.h"
#include "stlgeometryloader.h"

QT_BEGIN_NAMESPACE

class DefaultGeometryLoaderPlugin : public Qt3DRender::QGeometryLoaderFactory
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QGeometryLoaderFactory_iid FILE "default.json")
public:

    QStringList keys() const override
    {
        return QStringList() << OBJGEOMETRYLOADER_EXT
                             << PLYGEOMETRYLOADER_EXT
                             << STLGEOMETRYLOADER_EXT;
    }

    Qt3DRender::QGeometryLoaderInterface *create(const QString &ext) override
    {
        if (ext.compare(OBJGEOMETRYLOADER_EXT, Qt::CaseInsensitive) == 0)
            return new Qt3DRender::ObjGeometryLoader;
        else if (ext.compare(PLYGEOMETRYLOADER_EXT, Qt::CaseInsensitive) == 0)
            return new Qt3DRender::PlyGeometryLoader;
        else if (ext.compare(STLGEOMETRYLOADER_EXT, Qt::CaseInsensitive) == 0)
            return new Qt3DRender::StlGeometryLoader;
        return nullptr;
    }
};

QT_END_NAMESPACE

#include "main.moc"
