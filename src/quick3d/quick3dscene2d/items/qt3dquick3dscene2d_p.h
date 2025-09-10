// Copyright (C) 2017 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DSCENE2D_QUICK_QUICK3DSCENE2D_P_H
#define QT3DSCENE2D_QUICK_QUICK3DSCENE2D_P_H

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

#include <QtQml/qqmllist.h>

#include <Qt3DQuickScene2D/qscene2d.h>

#include <private/qt3dquickscene2d_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class Q_3DQUICKSCENE2DSHARED_EXPORT QQuick3DScene2D : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DCore::QEntity> entities READ entities)

public:
    QQuick3DScene2D(QObject *parent = nullptr);

    inline Qt3DRender::Quick::QScene2D *parentScene2D() const
    {
        return qobject_cast<Qt3DRender::Quick::QScene2D *>(parent());
    }

    QQmlListProperty<Qt3DCore::QEntity> entities();
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DANIMATION_QUICK_QUICK3DMORPHINGANIMATION_P_H

