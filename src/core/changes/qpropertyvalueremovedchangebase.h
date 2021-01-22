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

#ifndef QT3DCORE_QPROPERTYVALUEREMOVEDCHANGEBASE_H
#define QT3DCORE_QPROPERTYVALUEREMOVEDCHANGEBASE_H

#include <Qt3DCore/qscenechange.h>
#include <Qt3DCore/qt3dcore_global.h>
#include <QtCore/qsharedpointer.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QPropertyValueRemovedChangeBasePrivate;

class Q_3DCORESHARED_EXPORT QPropertyValueRemovedChangeBase : public QSceneChange
{
public:
    ~QPropertyValueRemovedChangeBase();

protected:
    Q_DECLARE_PRIVATE(QPropertyValueRemovedChangeBase)
    Q3D_DECL_DEPRECATED explicit QPropertyValueRemovedChangeBase(QNodeId subjectId);
    Q3D_DECL_DEPRECATED explicit QPropertyValueRemovedChangeBase(QPropertyValueRemovedChangeBasePrivate &dd, QNodeId subjectId);
};

Q3D_DECL_DEPRECATED typedef QSharedPointer<QPropertyValueRemovedChangeBase> QPropertyValueRemovedChangeBasePtr;

} // namespace Qt3DCore

QT_END_NAMESPACE

#endif // QT3DCORE_QPROPERTYVALUEREMOVEDCHANGEBASE_H
