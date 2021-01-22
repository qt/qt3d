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

#ifndef QT3DCORE_QDYNAMICPROPERTYUPDATEDCHANGE_H
#define QT3DCORE_QDYNAMICPROPERTYUPDATEDCHANGE_H

#include <Qt3DCore/qpropertyupdatedchangebase.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QDynamicPropertyUpdatedChangePrivate;

class Q_3DCORESHARED_EXPORT QDynamicPropertyUpdatedChange : public QPropertyUpdatedChangeBase
{
public:
    Q3D_DECL_DEPRECATED explicit QDynamicPropertyUpdatedChange(QNodeId subjectId);
    ~QDynamicPropertyUpdatedChange();

    QByteArray propertyName() const;
    void setPropertyName(const QByteArray &name);

    QVariant value() const;
    void setValue(const QVariant &value);

protected:
    Q_DECLARE_PRIVATE(QDynamicPropertyUpdatedChange)
    QDynamicPropertyUpdatedChange(QDynamicPropertyUpdatedChangePrivate &dd, QNodeId subjectId);
};

Q3D_DECL_DEPRECATED typedef QSharedPointer<QDynamicPropertyUpdatedChange> QDynamicPropertyUpdatedChangePtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QNODEDYNAMICPROPERTYUPDATEDCHANGE_H
