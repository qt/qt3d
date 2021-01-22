/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
****************************************************************************/

#ifndef QT3D_QUICK3DGEOMETRY_P_H
#define QT3D_QUICK3DGEOMETRY_P_H

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

#include <Qt3DRender/QGeometry>
#include <QtQml/QQmlListProperty>

#include <Qt3DQuickRender/private/qt3dquickrender_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {
namespace Render {
namespace Quick {

class Q_3DQUICKRENDERSHARED_PRIVATE_EXPORT Quick3DGeometry : public QObject
{
    Q_OBJECT
    Q_PROPERTY(QQmlListProperty<Qt3DRender::QAttribute> attributes READ attributeList)
    Q_CLASSINFO("DefaultProperty", "attributes")

public:
    explicit Quick3DGeometry(QObject *parent = 0);
    inline QGeometry *parentGeometry() const { return qobject_cast<QGeometry *>(parent()); }

    QQmlListProperty<Qt3DRender::QAttribute> attributeList();

private:
    static void appendAttribute(QQmlListProperty<Qt3DRender::QAttribute> *list, Qt3DRender::QAttribute *provider);
    static Qt3DRender::QAttribute *attributeAt(QQmlListProperty<Qt3DRender::QAttribute> *list, int index);
    static int attributesCount(QQmlListProperty<Qt3DRender::QAttribute> *list);
    static void clearAttributes(QQmlListProperty<Qt3DRender::QAttribute> *list);

    QVector<Qt3DRender::QAttribute *> m_managedAttributes;
};

} // namespace Quick
} // namespace Render
} // namespace Qt3DRender

QT_END_NAMESPACE

#endif // QT3D_QUICK3DGEOMETRY_P_H
