/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
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

#ifndef QT3DANIMATION_QANIMATIONCONTROLLER_H
#define QT3DANIMATION_QANIMATIONCONTROLLER_H

#include <Qt3DAnimation/qkeyframeanimation.h>
#include <Qt3DAnimation/qanimationgroup.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DAnimation/qt3danimation_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DAnimation {

class QAnimationControllerPrivate;

class Q_3DANIMATIONSHARED_EXPORT QAnimationController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(int activeAnimationGroup READ activeAnimationGroup WRITE setActiveAnimationGroup NOTIFY activeAnimationGroupChanged)
    Q_PROPERTY(float position READ position WRITE setPosition NOTIFY positionChanged)
    Q_PROPERTY(float positionScale READ positionScale WRITE setPositionScale NOTIFY positionScaleChanged)
    Q_PROPERTY(float positionOffset READ positionOffset WRITE setPositionOffset NOTIFY positionOffsetChanged)
    Q_PROPERTY(Qt3DCore::QEntity *entity READ entity WRITE setEntity NOTIFY entityChanged)
    Q_PROPERTY(bool recursive READ recursive WRITE setRecursive NOTIFY recursiveChanged)

public:
    QAnimationController(QObject *parent = nullptr);

    QList<Qt3DAnimation::QAnimationGroup *> animationGroupList();

    int activeAnimationGroup() const;
    float position() const;
    float positionScale() const;
    float positionOffset() const;
    Qt3DCore::QEntity *entity() const;
    bool recursive() const;

    void setAnimationGroups(const QList<Qt3DAnimation::QAnimationGroup *> &animationGroups);
    void addAnimationGroup(Qt3DAnimation::QAnimationGroup *animationGroups);
    void removeAnimationGroup(Qt3DAnimation::QAnimationGroup *animationGroups);

    Q_INVOKABLE int getAnimationIndex(const QString &name) const;
    Q_INVOKABLE Qt3DAnimation::QAnimationGroup *getGroup(int index) const;

public Q_SLOTS:
    void setActiveAnimationGroup(int index);
    void setPosition(float position);
    void setPositionScale(float scale);
    void setPositionOffset(float offset);
    void setEntity(Qt3DCore::QEntity *entity);
    void setRecursive(bool recursive);

Q_SIGNALS:
    void activeAnimationGroupChanged(int index);
    void positionChanged(float position);
    void positionScaleChanged(float scale);
    void positionOffsetChanged(float offset);
    void entityChanged(Qt3DCore::QEntity *entity);
    void recursiveChanged(bool recursive);

private:
    Q_DECLARE_PRIVATE(QAnimationController)
};

} // Qt3DAnimation

QT_END_NAMESPACE

#endif // QT3DANIMATION_QANIMATIONCONTROLLER_H
