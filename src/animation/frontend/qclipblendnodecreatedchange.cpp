/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
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

#include "qclipblendnodecreatedchange.h"
#include "qclipblendnodecreatedchange_p.h"
#include <Qt3DAnimation/qabstractclipblendnode.h>
#include <Qt3DAnimation/qabstractanimationclip.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

QClipBlendNodeCreatedChangeBasePrivate::QClipBlendNodeCreatedChangeBasePrivate(const QAbstractClipBlendNode *node)
    : Qt3DCore::QNodeCreatedChangeBasePrivate(node)
{
}

/*!
   \class Qt3DAnimation::QClipBlendNodeCreatedChangeBase
   \inherits Qt3DCore::QNodeCreatedChangeBase
   \inmodule Qt3DAnimation
   \brief Base class for changes in QClipBlendNode.
*/
QClipBlendNodeCreatedChangeBase::QClipBlendNodeCreatedChangeBase(const QAbstractClipBlendNode *node)
    : Qt3DCore::QNodeCreatedChangeBase(*new QClipBlendNodeCreatedChangeBasePrivate(node), node)
{
}

QClipBlendNodeCreatedChangeBase::~QClipBlendNodeCreatedChangeBase()
{
}
/*!
    \typedef Qt3DAnimation::QClipBlendNodeCreatedChangeBasePtr

    A shared pointer for QClipBlendNodeCreatedChangeBase.
*/

} // Qt3DAnimation

QT_END_NAMESPACE
