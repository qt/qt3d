/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DCORE_QCOMPONENTADDEDCHANGE_H
#define QT3DCORE_QCOMPONENTADDEDCHANGE_H

#include <Qt3DCore/qscenechange.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QComponent;
class QEntity;
class QComponentAddedChangePrivate;

class Q_3DCORESHARED_EXPORT QComponentAddedChange : public QSceneChange
{
public:
    Q3D_DECL_DEPRECATED explicit QComponentAddedChange(const QEntity *entity,
                                                       const QComponent *component);
    Q3D_DECL_DEPRECATED explicit QComponentAddedChange(const QComponent *component,
                                                       const QEntity *entity);
    ~QComponentAddedChange();

    QNodeId entityId() const Q_DECL_NOTHROW;
    QNodeId componentId() const Q_DECL_NOTHROW;
    const QMetaObject *componentMetaObject() const Q_DECL_NOTHROW;

private:
    Q_DECLARE_PRIVATE(QComponentAddedChange)
};

Q3D_DECL_DEPRECATED typedef QSharedPointer<QComponentAddedChange> QComponentAddedChangePtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QCOMPONENTADDEDCHANGE_H
