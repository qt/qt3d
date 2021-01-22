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

#ifndef QT3DANIMATION_QBLENDEDCLIPANIMATOR_P_H
#define QT3DANIMATION_QBLENDEDCLIPANIMATOR_P_H

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

#include <Qt3DAnimation/private/qabstractclipanimator_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAbstractClipBlendNode;

class QBlendedClipAnimatorPrivate : public Qt3DAnimation::QAbstractClipAnimatorPrivate
{
public:
    QBlendedClipAnimatorPrivate();

    Q_DECLARE_PUBLIC(QBlendedClipAnimator)
    QAbstractClipBlendNode *m_blendTreeRoot;
};

struct QBlendedClipAnimatorData : public QAbstractClipAnimatorData
{
    Qt3DCore::QNodeId blendTreeRootId;
};

} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_QBLENDEDCLIPANIMATOR_P_H
