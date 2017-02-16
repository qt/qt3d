/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "quick3dabstractclipblendnode_p.h"

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {
namespace Quick {

Quick3DAbstractClipBlendNode::Quick3DAbstractClipBlendNode(QObject *parent)
    : QObject(parent)
{
}

QQmlListProperty<QAbstractAnimationClip> Quick3DAbstractClipBlendNode::clipList()
{
    return QQmlListProperty<QAbstractAnimationClip>(this, 0,
                                                    &Quick3DAbstractClipBlendNode::appendClip,
                                                    &Quick3DAbstractClipBlendNode::clipCount,
                                                    &Quick3DAbstractClipBlendNode::clipAt,
                                                    &Quick3DAbstractClipBlendNode::clearClips);
}

void Quick3DAbstractClipBlendNode::appendClip(QQmlListProperty<QAbstractAnimationClip> *list, QAbstractAnimationClip *clip)
{
    Quick3DAbstractClipBlendNode *extension = qobject_cast<Quick3DAbstractClipBlendNode *>(list->object);
    extension->parentAbstractClipBlendNode()->addClip(clip);
}

QAbstractAnimationClip *Quick3DAbstractClipBlendNode::clipAt(QQmlListProperty<QAbstractAnimationClip> *list, int index)
{
    Quick3DAbstractClipBlendNode *extension = qobject_cast<Quick3DAbstractClipBlendNode *>(list->object);
    return extension->parentAbstractClipBlendNode()->clips().at(index);
}

int Quick3DAbstractClipBlendNode::clipCount(QQmlListProperty<QAbstractAnimationClip> *list)
{
    Quick3DAbstractClipBlendNode *extension = qobject_cast<Quick3DAbstractClipBlendNode *>(list->object);
    return extension->parentAbstractClipBlendNode()->clips().count();
}

void Quick3DAbstractClipBlendNode::clearClips(QQmlListProperty<QAbstractAnimationClip> *list)
{
    Quick3DAbstractClipBlendNode *extension = qobject_cast<Quick3DAbstractClipBlendNode *>(list->object);
    const QVector<QAbstractAnimationClip *> clips = extension->parentAbstractClipBlendNode()->clips();
    for (QAbstractAnimationClip *clip : clips)
        extension->parentAbstractClipBlendNode()->removeClip(clip);
}

} // Quick
} // Animation
} // Qt3DAnimation

QT_END_NAMESPACE
