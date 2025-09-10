// Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_RENDER_QUICK_QUICK3DTECHNIQUEFILTER_P_H
#define QT3DRENDER_RENDER_QUICK_QUICK3DTECHNIQUEFILTER_P_H

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

#include <Qt3DRender/qfilterkey.h>
#include <Qt3DRender/qtechniquefilter.h>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuick/private/quick3dnode_p.h>
#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DTechniqueFilter : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QFilterKey> matchAll READ matchList)
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QParameter> parameters READ parameterList)

public:
    explicit Quick3DTechniqueFilter(QObject *parent = 0);

    QQmlListProperty<QFilterKey> matchList();
    QQmlListProperty<QParameter> parameterList();

    inline QTechniqueFilter *parentTechniqueFilter() const { return qobject_cast<QTechniqueFilter*>(parent()); }
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3DRENDER_RENDER_QUICK_QUICK3DTECHNIQUEFILTER_P_H
