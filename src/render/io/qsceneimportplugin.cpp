// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsceneimportplugin_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QSceneImportPlugin::QSceneImportPlugin(QObject *parent) : QObject(parent)
{

}

QSceneImportPlugin::~QSceneImportPlugin()
{

}

QSceneImporter *QSceneImportPlugin::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(key);
    Q_UNUSED(paramList);
    return nullptr;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qsceneimportplugin_p.cpp"

