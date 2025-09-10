// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DMATERIAL_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DMATERIAL_P_H

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

#include <Qt3DRender/qmaterial.h>
#include <Qt3DRender/qparameter.h>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

// FIXME - write a custom QML parser and stop mis-using Tag
// Tags could be replaced by Parameters directly

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DMaterial : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QParameter> parameters READ qmlParameters)

public:
    explicit Quick3DMaterial(QObject *parent = nullptr);

    // TO DO : replace by QAbstractMaterial later on
    inline QMaterial *parentMaterial() const { return qobject_cast<QMaterial*>(parent()); }

    QQmlListProperty<QParameter> qmlParameters();
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DMATERIAL_P_H
