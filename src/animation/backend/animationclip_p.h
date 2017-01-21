/****************************************************************************
**
** Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3DANIMATION_ANIMATION_ANIMATIONCLIP_P_H
#define QT3DANIMATION_ANIMATION_ANIMATIONCLIP_P_H

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

#include <Qt3DAnimation/private/backendnode_p.h>
#include <Qt3DAnimation/private/fcurve_p.h>
#include <QtCore/qurl.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {
namespace Animation {

class Handler;

class Q_AUTOTEST_EXPORT AnimationClip : public BackendNode
{
public:
    AnimationClip();

    void cleanup();
    void setSource(const QUrl &source) { m_source = source; }
    QUrl source() const { return m_source; }
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE;

    QString name() const { return m_name; }
    QString objectName() const { return m_objectName; }
    QVector<ChannelGroup> channelGroups() const { return m_channelGroups; }

    // Called from jobs
    void loadAnimation();

private:
    void initializeFromPeer(const Qt3DCore::QNodeCreatedChangeBasePtr &change) Q_DECL_FINAL;
    void clearData();

    QUrl m_source;

    QString m_name;
    QString m_objectName;
    QVector<ChannelGroup> m_channelGroups;
};

#ifndef QT_NO_DEBUG_STREAM
inline QDebug operator<<(QDebug dbg, const AnimationClip &animationClip)
{
    QDebugStateSaver saver(dbg);
    dbg << "QNodeId =" << animationClip.peerId() << endl
        << "Name =" << animationClip.name() << endl
        << "Object Name =" << animationClip.objectName() << endl
        << "Channel Groups:" << endl;

    const QVector<ChannelGroup> channelGroups = animationClip.channelGroups();
    for (const auto channelGroup : channelGroups) {
        dbg << channelGroup;
    }

    return dbg;
}
#endif

} // namespace Animation
} // namespace Qt3DAnimation


QT_END_NAMESPACE

#endif // QT3DANIMATION_ANIMATION_ANIMATIONCLIP_P_H
