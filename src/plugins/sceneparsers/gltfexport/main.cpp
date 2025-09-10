// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "gltfexporter.h"

#include <private/qsceneexportplugin_p.h>

QT_BEGIN_NAMESPACE

class GLTFSceneExportPlugin : public Qt3DRender::QSceneExportPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QSceneExportFactoryInterface_iid FILE "gltfexport.json")

    Qt3DRender::QSceneExporter *create(const QString &key, const QStringList &paramList) override
    {
        Q_UNUSED(key);
        Q_UNUSED(paramList);
        return new Qt3DRender::GLTFExporter();
    }
};

QT_END_NAMESPACE

#include "main.moc"
