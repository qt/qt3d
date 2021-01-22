/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DCORE_QSCENECHANGE_P_H
#define QT3DCORE_QSCENECHANGE_P_H

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

#include <Qt3DCore/qscenechange.h>
#include <QtCore/QtGlobal>

#include <Qt3DCore/private/qt3dcore_global_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DCore {

class QSceneChange;
class QNodeId;

QT_WARNING_PUSH
QT_WARNING_DISABLE_DEPRECATED
struct NodeRelationshipChange {
    QNode *node;
    QNode *subNode;
    ChangeFlag change;
    const char *property;
};
QT_WARNING_POP

class Q_3DCORE_PRIVATE_EXPORT QSceneChangePrivate
{
public :
    QSceneChangePrivate();
    virtual ~QSceneChangePrivate();

    Q_DECLARE_PUBLIC(QSceneChange)

    QSceneChange *q_ptr;

    QNodeId m_subjectId;
    QSceneChange::DeliveryFlags m_deliveryFlags;
    QT_WARNING_PUSH
    QT_WARNING_DISABLE_DEPRECATED
    ChangeFlag m_type;
    QT_WARNING_POP
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DCORE_QSCENECHANGE_P_H
