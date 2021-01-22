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

#ifndef QT3DCORE_QPROPERTYVALUEADDEDCHANGE_H
#define QT3DCORE_QPROPERTYVALUEADDEDCHANGE_H

#include <Qt3DCore/qstaticpropertyvalueaddedchangebase.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QPropertyValueAddedChangePrivate;

class Q_3DCORESHARED_EXPORT QPropertyValueAddedChange : public QStaticPropertyValueAddedChangeBase
{
public:
    Q3D_DECL_DEPRECATED explicit QPropertyValueAddedChange(QNodeId subjectId);
    ~QPropertyValueAddedChange();

    void setAddedValue(const QVariant &value);
    QVariant addedValue() const;

private:
    Q_DECLARE_PRIVATE(QPropertyValueAddedChange)
};

Q3D_DECL_DEPRECATED typedef QSharedPointer<QPropertyValueAddedChange> QPropertyValueAddedChangePtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QPROPERTYVALUEADDEDCHANGE_H
