/****************************************************************************
**
** Copyright (C) 2014 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef QT3D_INPUT_QKEYBOARDINPUT_H
#define QT3D_INPUT_QKEYBOARDINPUT_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DInput/q3dkeyevent.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3D {

class QKeyboardController;
class QKeyboardInputPrivate;

class QT3DINPUTSHARED_EXPORT QKeyboardInput : public QComponent
{
    Q_OBJECT
    Q_PROPERTY(Qt3D::QKeyboardController *controller READ controller WRITE setController NOTIFY controllerChanged)
    Q_PROPERTY(bool focus READ focus WRITE setFocus NOTIFY focusChanged)
public:
    explicit QKeyboardInput(QNode *parent = 0);

    void setController(QKeyboardController *controller);
    QKeyboardController *controller() const;
    bool focus() const;
    void setFocus(bool focus);

Q_SIGNALS:
    void controllerChanged();
    void focusChanged();

    void digit0Pressed(Q3DKeyEvent *event);
    void digit1Pressed(Q3DKeyEvent *event);
    void digit2Pressed(Q3DKeyEvent *event);
    void digit3Pressed(Q3DKeyEvent *event);
    void digit4Pressed(Q3DKeyEvent *event);
    void digit5Pressed(Q3DKeyEvent *event);
    void digit6Pressed(Q3DKeyEvent *event);
    void digit7Pressed(Q3DKeyEvent *event);
    void digit8Pressed(Q3DKeyEvent *event);
    void digit9Pressed(Q3DKeyEvent *event);

    void leftPressed(Q3DKeyEvent *event);
    void rightPressed(Q3DKeyEvent *event);
    void upPressed(Q3DKeyEvent *event);
    void downPressed(Q3DKeyEvent *event);
    void tabPressed(Q3DKeyEvent *event);
    void backtabPressed(Q3DKeyEvent *event);

    void asteriskPressed(Q3DKeyEvent *event);
    void numberSignPressed(Q3DKeyEvent *event);
    void escapePressed(Q3DKeyEvent *event);
    void returnPressed(Q3DKeyEvent *event);
    void enterPressed(Q3DKeyEvent *event);
    void deletePressed(Q3DKeyEvent *event);
    void spacePressed(Q3DKeyEvent *event);
    void backPressed(Q3DKeyEvent *event);
    void cancelPressed(Q3DKeyEvent *event);
    void selectPressed(Q3DKeyEvent *event);
    void yesPressed(Q3DKeyEvent *event);
    void noPressed(Q3DKeyEvent *event);
    void context1Pressed(Q3DKeyEvent *event);
    void context2Pressed(Q3DKeyEvent *event);
    void context3Pressed(Q3DKeyEvent *event);
    void context4Pressed(Q3DKeyEvent *event);
    void callPressed(Q3DKeyEvent *event);
    void hangupPressed(Q3DKeyEvent *event);
    void flipPressed(Q3DKeyEvent *event);
    void menuPressed(Q3DKeyEvent *event);
    void volumeUpPressed(Q3DKeyEvent *event);
    void volumeDownPressed(Q3DKeyEvent *event);

    void pressed(Q3DKeyEvent *event);
    void released(Q3DKeyEvent *event);

protected:
    QKeyboardInput(QKeyboardInputPrivate &dd, QNode *parent = 0);
    void copy(const QNode *ref) Q_DECL_OVERRIDE;
    void sceneChangeEvent(const QSceneChangePtr &change) Q_DECL_OVERRIDE;
    void keyEvent(Q3DKeyEvent *event);

private:
    Q_DECLARE_PRIVATE(QKeyboardInput)
    QT3D_CLONEABLE(QKeyboardInput)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3D_INPUT_QKEYBOARDINPUT_H
