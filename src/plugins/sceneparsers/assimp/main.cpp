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
