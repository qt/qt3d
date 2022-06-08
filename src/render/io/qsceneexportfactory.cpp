// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsceneexportfactory_p.h"
#include "qsceneexportplugin_p.h"
#include "qsceneexporter_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>

#include <private/qfactoryloader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QSceneExportFactoryInterface_iid, QLatin1String("/sceneparsers"), Qt::CaseInsensitive))

QStringList QSceneExportFactory::keys()
{
    return loader()->keyMap().values();
}

QSceneExporter *QSceneExportFactory::create(const QString &name, const QStringList &args)
{
    return qLoadPlugin<QSceneExporter, QSceneExportPlugin>(loader(), name, args);
}

} // namespace Qt3DRender

QT_END_NAMESPACE
