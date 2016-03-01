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
#include <Qt3DInput/qkeyevent.h>
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
    explicit QKeyboardInput(QNode *parent = Q_NULLPTR);
    ~QKeyboardInput();

    QKeyboardController *controller() const;
    bool focus() const;

public Q_SLOTS:
    void setController(QKeyboardController *controller);
    void setFocus(bool focus);

Q_SIGNALS:
    void controllerChanged(QKeyboardController *controller);
    void focusChanged(bool focus);

    void digit0Pressed(Qt3DInput::QKeyEvent *event);
    void digit1Pressed(Qt3DInput::QKeyEvent *event);
    void digit2Pressed(Qt3DInput::QKeyEvent *event);
    void digit3Pressed(Qt3DInput::QKeyEvent *event);
    void digit4Pressed(Qt3DInput::QKeyEvent *event);
    void digit5Pressed(Qt3DInput::QKeyEvent *event);
    void digit6Pressed(Qt3DInput::QKeyEvent *event);
    void digit7Pressed(Qt3DInput::QKeyEvent *event);
    void digit8Pressed(Qt3DInput::QKeyEvent *event);
    void digit9Pressed(Qt3DInput::QKeyEvent *event);

    void leftPressed(Qt3DInput::QKeyEvent *event);
    void rightPressed(Qt3DInput::QKeyEvent *event);
    void upPressed(Qt3DInput::QKeyEvent *event);
    void downPressed(Qt3DInput::QKeyEvent *event);
    void tabPressed(Qt3DInput::QKeyEvent *event);
    void backtabPressed(Qt3DInput::QKeyEvent *event);

    void asteriskPressed(Qt3DInput::QKeyEvent *event);
    void numberSignPressed(Qt3DInput::QKeyEvent *event);
    void escapePressed(Qt3DInput::QKeyEvent *event);
    void returnPressed(Qt3DInput::QKeyEvent *event);
    void enterPressed(Qt3DInput::QKeyEvent *event);
    void deletePressed(Qt3DInput::QKeyEvent *event);
    void spacePressed(Qt3DInput::QKeyEvent *event);
    void backPressed(Qt3DInput::QKeyEvent *event);
    void cancelPressed(Qt3DInput::QKeyEvent *event);
    void selectPressed(Qt3DInput::QKeyEvent *event);
    void yesPressed(Qt3DInput::QKeyEvent *event);
    void noPressed(Qt3DInput::QKeyEvent *event);
    void context1Pressed(Qt3DInput::QKeyEvent *event);
    void context2Pressed(Qt3DInput::QKeyEvent *event);
    void context3Pressed(Qt3DInput::QKeyEvent *event);
    void context4Pressed(Qt3DInput::QKeyEvent *event);
    void callPressed(Qt3DInput::QKeyEvent *event);
    void hangupPressed(Qt3DInput::QKeyEvent *event);
    void flipPressed(Qt3DInput::QKeyEvent *event);
    void menuPressed(Qt3DInput::QKeyEvent *event);
    void volumeUpPressed(Qt3DInput::QKeyEvent *event);
    void volumeDownPressed(Qt3DInput::QKeyEvent *event);

    void pressed(Qt3DInput::QKeyEvent *event);
    void released(Qt3DInput::QKeyEvent *event);

protected:
    QKeyboardInput(QKeyboardInputPrivate &dd, QNode *parent = Q_NULLPTR);
    void copy(const Qt3DCore::QNode *ref) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_OVERRIDE;
    void keyEvent(QKeyEvent *event);

private:
    Q_DECLARE_PRIVATE(QKeyboardInput)
    QT3D_CLONEABLE(QKeyboardInput)
};

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_QKEYBOARDINPUT_H
