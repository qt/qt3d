/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

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
#if QT_CONFIG(library)
Q_GLOBAL_STATIC_WITH_ARGS(QFactoryLoader, directLoader, (QRenderPluginFactoryInterface_iid, QLatin1String(""), Qt::CaseInsensitive))
#endif

QStringList QRenderPluginFactory::keys(const QString &pluginPath)
{
#if QT_CONFIG(library)
    QStringList list;
    if (!pluginPath.isEmpty()) {
        QCoreApplication::addLibraryPath(pluginPath);
        list = directLoader()->keyMap().values();
        if (!list.isEmpty()) {
            const QString postFix = QLatin1String(" (from ")
                    + QDir::toNativeSeparators(pluginPath)
                    + QLatin1Char(')');
            const QStringList::iterator end = list.end();
            for (QStringList::iterator it = list.begin(); it != end; ++it)
                (*it).append(postFix);
        }
    }
    list.append(loader()->keyMap().values());
    return list;
#else
    return loader()->keyMap().values();
#endif
}

QRenderPlugin *QRenderPluginFactory::create(const QString &name, const QStringList &args,
                                            const QString &pluginPath)
{
#if QT_CONFIG(library)
    if (!pluginPath.isEmpty()) {
        QCoreApplication::addLibraryPath(pluginPath);
        if (QRenderPlugin *ret
                = qLoadPlugin<QRenderPlugin, QRenderPluginFactoryIf>(directLoader(), name, args)) {
            return ret;
        }
    }
#endif
    return qLoadPlugin<QRenderPlugin, QRenderPluginFactoryIf>(loader(), name, args);
}

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE
