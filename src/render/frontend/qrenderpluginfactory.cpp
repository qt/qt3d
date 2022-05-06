/****************************************************************************
**
** Copyright (C) 2022 The Qt Company Ltd.
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

#include "qrenderpluginfactory_p.h"
#include "qrenderpluginfactoryif_p.h"
#include "qrenderplugin_p.h"

#include <QtCore/qcoreapplication.h>
#include <QtCore/qdir.h>

#include <private/qfactoryloader_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, loader, (QRenderPluginFactoryInterface_iid, QLatin1String("/renderplugins"), Qt::CaseInsensitive))

QStringList QRenderPluginFactory::keys()
{
    return loader()->keyMap().values();
}

QRenderPlugin *QRenderPluginFactory::create(const QString &name, const QStringList &args)
{
    return qLoadPlugin<QRenderPlugin, QRenderPluginFactoryIf>(loader(), name, args);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
