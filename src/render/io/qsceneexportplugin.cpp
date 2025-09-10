// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsceneexportplugin_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

QSceneExportPlugin::QSceneExportPlugin(QObject *parent) : QObject(parent)
{

}

QSceneExportPlugin::~QSceneExportPlugin()
{

}

QSceneExporter *QSceneExportPlugin::create(const QString &key, const QStringList &paramList)
{
    Q_UNUSED(key);
    Q_UNUSED(paramList);
    return nullptr;
}

} // namespace Qt3DRender

QT_END_NAMESPACE

#include "moc_qsceneexportplugin_p.cpp"

