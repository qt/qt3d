/****************************************************************************
**
** Copyright (C) 2021 The Qt Company Ltd.
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
****************************************************************************/

#ifndef QT3DANIMATION_QMORPHTARGET_P_H
#define QT3DANIMATION_QMORPHTARGET_P_H

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

#include <Qt3DAnimation/qmorphtarget.h>

#include <private/qobject_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QMorphTargetPrivate : public QObjectPrivate
{
public:
    QMorphTargetPrivate();

    void updateAttributeNames();

    QStringList m_attributeNames;
    QVector<Qt3DRender::QAttribute *> m_targetAttributes;

    Q_DECLARE_PUBLIC(QMorphTarget)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QMORPHTARGET_P_H
