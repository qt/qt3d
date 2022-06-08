// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

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
    QList<Qt3DCore::QAttribute *> m_targetAttributes;

    Q_DECLARE_PUBLIC(QMorphTarget)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QMORPHTARGET_P_H
