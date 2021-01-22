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

#ifndef QT3DCORE_QSTATICPROPERTYUPDATEDCHANGEBASE_H
#define QT3DCORE_QSTATICPROPERTYUPDATEDCHANGEBASE_H

#include <Qt3DCore/qpropertyupdatedchangebase.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QStaticPropertyUpdatedChangeBasePrivate;

class Q_3DCORESHARED_EXPORT QStaticPropertyUpdatedChangeBase : public QPropertyUpdatedChangeBase
{
public:
    ~QStaticPropertyUpdatedChangeBase();

    const char *propertyName() const;
    void setPropertyName(const char *name);

protected:
    Q3D_DECL_DEPRECATED explicit QStaticPropertyUpdatedChangeBase(QNodeId subjectId);
    Q3D_DECL_DEPRECATED explicit QStaticPropertyUpdatedChangeBase(QStaticPropertyUpdatedChangeBasePrivate &dd, QNodeId subjectId);
    Q_DECLARE_PRIVATE(QStaticPropertyUpdatedChangeBase)
};

Q3D_DECL_DEPRECATED typedef QSharedPointer<QStaticPropertyUpdatedChangeBase> QStaticPropertyUpdatedChangeBasePtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QSTATICPROPERTYUPDATEDCHANGEBASE_H
