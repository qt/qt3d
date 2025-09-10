// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QRENDERPLUGINFACTORY_P_H
#define QT3DRENDER_RENDER_QRENDERPLUGINFACTORY_P_H

//
//  W A R N I N G
//  -------------
//
// This file is not part of the Qt API.  It exists for the convenience
// of other Qt classes.  This header file may change from version to
// version without notice, or even be removed.
//
// We mean it.
//

#include <private/qt3drender_global_p.h>

#include <QtCore/qstringlist.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {

class QRenderPlugin;

class Q_3DRENDERSHARED_PRIVATE_EXPORT QRenderPluginFactory
{
public:
    static QStringList keys();
    static QRenderPlugin *create(const QString &name, const QStringList &args);
};

} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QRENDERPLUGINFACTORY_P_H
