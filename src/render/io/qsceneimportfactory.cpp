// Copyright (C) 2016 The Qt Company Ltd and/or its subsidiary(-ies).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#include "qsceneimportfactory_p.h"
#include "qsceneimportplugin_p.h"
#include "qsceneimporter_p.h"

#include <QtCore/private/qfactoryloader_p.h>
#include <QtCore/QCoreApplication>
#include <QtCore/QDir>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QSceneImportFactoryInterface_iid, QLatin1String("/sceneparsers"), Qt::CaseInsensitive))

QStringList QSceneImportFactory::keys()
{
    return loader->keyMap().values();
}

QSceneImporter *QSceneImportFactory::create(const QString &name, const QStringList &args)
{
    return qLoadPlugin<QSceneImporter, QSceneImportPlugin>(loader(), name, args);
}

} // namespace Qt3DRender

QT_END_NAMESPACE
