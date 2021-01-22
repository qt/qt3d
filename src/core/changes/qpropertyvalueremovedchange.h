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

#ifndef QT3DCORE_QPROPERTYVALUEREMOVEDCHANGE_H
#define QT3DCORE_QPROPERTYVALUEREMOVEDCHANGE_H

#include <Qt3DCore/qstaticpropertyvalueremovedchangebase.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QPropertyValueRemovedChangePrivate;

class Q_3DCORESHARED_EXPORT QPropertyValueRemovedChange : public QStaticPropertyValueRemovedChangeBase
{
public:
    Q3D_DECL_DEPRECATED explicit QPropertyValueRemovedChange(QNodeId subjectId);
    ~QPropertyValueRemovedChange();

    void setRemovedValue(const QVariant &value);
    QVariant removedValue() const;

private:
    Q_DECLARE_PRIVATE(QPropertyValueRemovedChange)
};

Q3D_DECL_DEPRECATED typedef QSharedPointer<QPropertyValueRemovedChange> QPropertyValueRemovedChangePtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QPROPERTYVALUEREMOVEDCHANGE_H
