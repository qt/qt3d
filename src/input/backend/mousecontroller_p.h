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

#ifndef QT3DINPUT_INPUT_MOUSECONTROLLER_H
#define QT3DINPUT_INPUT_MOUSECONTROLLER_H

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

#include <Qt3DInput/qabstractphysicaldevicebackendnode.h>
#include <QMouseEvent>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QInputAspect;

namespace Input {

class InputHandler;

class MouseController : public Qt3DInput::QAbstractPhysicalDeviceBackendNode
{
public:
    MouseController();
    ~MouseController();

    void updateFromPeer(Qt3DCore::QNode *peer) Q_DECL_OVERRIDE;
    void setInputHandler(InputHandler *handler);

    void addMouseInput(const Qt3DCore::QNodeId &input);
    void removeMouseInput(const Qt3DCore::QNodeId &input);

    float axisValue(int axisIdentifier) const Q_DECL_OVERRIDE;
    bool isButtonPressed(int buttonIdentifier) const Q_DECL_OVERRIDE;

    QVector<Qt3DCore::QNodeId> mouseInputs() const;

    void updateMouseEvents(const QList<QT_PREPEND_NAMESPACE(QMouseEvent)> &events);

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &e) Q_DECL_OVERRIDE;

private:
    QVector<Qt3DCore::QNodeId> m_mouseInputs;
    InputHandler *m_inputHandler;

    struct MouseState {

        MouseState()
            : xAxis(0.0f)
            , yAxis(0.0f)
            , leftPressed(false)
            , rightPressed(false)
            , centerPressed(false)
        {}

        float xAxis;
        float yAxis;
        bool leftPressed;
        bool rightPressed;
        bool centerPressed;
    };

    MouseState m_mouseState;
    QPointF m_previousPos;
    float m_sensitivity;
};

class MouseControllerFunctor : public Qt3DCore::QBackendNodeFunctor
{
public:
    explicit MouseControllerFunctor(Qt3DInput::QInputAspect *inputAspect, InputHandler *handler);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    Qt3DCore::QBackendNode *get(const Qt3DCore::QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const Qt3DCore::QNodeId &id) const Q_DECL_OVERRIDE;

private:
    QInputAspect *m_inputAspect;
    InputHandler *m_handler;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // MOUSECONTROLLER_H
