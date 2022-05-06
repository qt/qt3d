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
