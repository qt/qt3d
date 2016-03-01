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

#ifndef QT3DINPUT_QMOUSEINPUT_H
#define QT3DINPUT_QMOUSEINPUT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/qmouseevent.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QMouseController;
class QMouseInputPrivate;

class QT3DINPUTSHARED_EXPORT QMouseInput : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QMouseController *controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)

public:
    explicit QMouseInput(QNode *parent = Q_NULLPTR);
    ~QMouseInput();

    QMouseController *controller() const;
    bool containsMouse() const;

public Q_SLOTS:
    void setController(QMouseController *controller);

Q_SIGNALS:
    void controllerChanged(QMouseController *controller);
    void containsMouseChanged(bool containsMouse);

    void clicked(Qt3DInput::QMouseEvent *mouse);
    void doubleClicked(Qt3DInput::QMouseEvent *mouse);

    void entered();
    void exited();

    void pressed(Qt3DInput::QMouseEvent *mouse);
    void released(Qt3DInput::QMouseEvent *mouse);
    void pressAndHold(Qt3DInput::QMouseEvent *mouse);
    void positionChanged(Qt3DInput::QMouseEvent *mouse);

    void wheel(Qt3DInput::QWheelEvent *wheel);

protected:
    QMouseInput(QMouseInputPrivate &dd, Qt3DCore::QNode *parent = Q_NULLPTR);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_OVERRIDE;
    void mouseEvent(Qt3DInput::QMouseEvent *event);
    void setContainsMouse(bool contains);

private:
    Q_DECLARE_PRIVATE(QMouseInput)
    QT3D_CLONEABLE(QMouseInput)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QMOUSEINPUT_H
