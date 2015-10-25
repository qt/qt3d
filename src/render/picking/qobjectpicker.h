/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DRENDER_QOBJECTPICKER_H
#define QT3DRENDER_QOBJECTPICKER_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QAttribute;
class QObjectPickerPrivate;
class QPickEvent;

class QT3DRENDERSHARED_EXPORT QObjectPicker : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DRender::QAttribute *pickAttribute READ pickAttribute WRITE setPickAttribute NOTIFY pickAttributeChanged)
    Q_PROPERTY(bool hoverEnabled READ hoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(bool pressed READ isPressed NOTIFY pressedChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)

public:
    explicit QObjectPicker(QNode *parent = Q_NULLPTR);
    ~QObjectPicker();

    void setPickAttribute(QAttribute *pickAttribute);
    QAttribute *pickAttribute() const;

    void setHoverEnabled(bool hoverEnabled);
    bool hoverEnabled() const;

    bool containsMouse() const;
    bool isPressed() const;

Q_SIGNALS:
    void pressed(QPickEvent *event);
    void released(QPickEvent *event);
    void clicked(QPickEvent *event);
    void entered();
    void exited();
    void pickAttributeChanged();
    void hoverEnabledChanged();
    void pressedChanged();
    void containsMouseChanged();

protected:
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_OVERRIDE;

private:
    void setPressed(bool pressed);
    void setContainsMouse(bool containsMouse);

    QT3D_CLONEABLE(QObjectPicker)
    Q_DECLARE_PRIVATE(QObjectPicker)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DRENDER_QOBJECTPICKER_H
