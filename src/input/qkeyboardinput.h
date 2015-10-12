/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
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

#ifndef QT3DINPUT_INPUT_QKEYBOARDINPUT_H
#define QT3DINPUT_INPUT_QKEYBOARDINPUT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/q3dkeyevent.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QKeyboardController;
class QKeyboardInputPrivate;

class QT3DINPUTSHARED_EXPORT QKeyboardInput : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3DInput::QKeyboardController *controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(bool focus READ focus WRITE setFocus NOTIFY focusChanged)
public:
    explicit QKeyboardInput(QNode *parent = 0);
    ~QKeyboardInput();

    void setController(QKeyboardController *controller);
    QKeyboardController *controller() const;
    bool focus() const;
    void setFocus(bool focus);

Q_SIGNALS:
    void controllerChanged();
    void focusChanged();

    void digit0Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit1Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit2Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit3Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit4Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit5Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit6Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit7Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit8Pressed(Qt3DInput::Q3DKeyEvent *event);
    void digit9Pressed(Qt3DInput::Q3DKeyEvent *event);

    void leftPressed(Qt3DInput::Q3DKeyEvent *event);
    void rightPressed(Qt3DInput::Q3DKeyEvent *event);
    void upPressed(Qt3DInput::Q3DKeyEvent *event);
    void downPressed(Qt3DInput::Q3DKeyEvent *event);
    void tabPressed(Qt3DInput::Q3DKeyEvent *event);
    void backtabPressed(Qt3DInput::Q3DKeyEvent *event);

    void asteriskPressed(Qt3DInput::Q3DKeyEvent *event);
    void numberSignPressed(Qt3DInput::Q3DKeyEvent *event);
    void escapePressed(Qt3DInput::Q3DKeyEvent *event);
    void returnPressed(Qt3DInput::Q3DKeyEvent *event);
    void enterPressed(Qt3DInput::Q3DKeyEvent *event);
    void deletePressed(Qt3DInput::Q3DKeyEvent *event);
    void spacePressed(Qt3DInput::Q3DKeyEvent *event);
    void backPressed(Qt3DInput::Q3DKeyEvent *event);
    void cancelPressed(Qt3DInput::Q3DKeyEvent *event);
    void selectPressed(Qt3DInput::Q3DKeyEvent *event);
    void yesPressed(Qt3DInput::Q3DKeyEvent *event);
    void noPressed(Qt3DInput::Q3DKeyEvent *event);
    void context1Pressed(Qt3DInput::Q3DKeyEvent *event);
    void context2Pressed(Qt3DInput::Q3DKeyEvent *event);
    void context3Pressed(Qt3DInput::Q3DKeyEvent *event);
    void context4Pressed(Qt3DInput::Q3DKeyEvent *event);
    void callPressed(Qt3DInput::Q3DKeyEvent *event);
    void hangupPressed(Qt3DInput::Q3DKeyEvent *event);
    void flipPressed(Qt3DInput::Q3DKeyEvent *event);
    void menuPressed(Qt3DInput::Q3DKeyEvent *event);
    void volumeUpPressed(Qt3DInput::Q3DKeyEvent *event);
    void volumeDownPressed(Qt3DInput::Q3DKeyEvent *event);

    void pressed(Qt3DInput::Q3DKeyEvent *event);
    void released(Qt3DInput::Q3DKeyEvent *event);

protected:
    QKeyboardInput(QKeyboardInputPrivate &dd, QNode *parent = 0);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_OVERRIDE;
    void keyEvent(Q3DKeyEvent *event);

private:
    Q_DECLARE_PRIVATE(QKeyboardInput)
    QT3D_CLONEABLE(QKeyboardInput)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_QKEYBOARDINPUT_H
