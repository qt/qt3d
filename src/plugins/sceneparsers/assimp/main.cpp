// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial

#include "assimpimporter.h"

#include <Qt3DRender/private/qsceneimportplugin_p.h>

QT_BEGIN_NAMESPACE

class AssimpSceneImportPlugin : public Qt3DRender::QSceneImportPlugin
{
    Q_OBJECT
    Q_PLUGIN_METADATA(IID QSceneImportFactoryInterface_iid FILE "assimp.json")

    Qt3DRender::QSceneImporter *create(const QString &key, const QStringList &paramList) override
    {
        Q_UNUSED(key);
        Q_UNUSED(paramList);
        return new Qt3DRender::AssimpImporter();
    }
};

QT_END_NAMESPACE

#include "main.moc"
