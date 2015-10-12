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

#ifndef QT3DINPUT_INPUT_KEYBOARDCONTROLLER_P_H
#define QT3DINPUT_INPUT_KEYBOARDCONTROLLER_P_H

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

#include <Qt3DCore/qbackendnode.h>
#include <Qt3DCore/qnodeid.h>
#include <Qt3DInput/private/handle_types_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {
namespace Input {

class InputHandler;

class KeyboardController : public Qt3DCore::QBackendNode
{
public:
    KeyboardController();
    void updateFromPeer(Qt3DCore::QNode *peer) Q_DECL_OVERRIDE;
    void requestFocusForInput(const Qt3DCore::QNodeId &inputId);
    void setInputHandler(InputHandler *handler);

    void addKeyboardInput(const Qt3DCore::QNodeId &input);
    void removeKeyboardInput(const Qt3DCore::QNodeId &input);

    void setCurrentFocusItem(const Qt3DCore::QNodeId &input);

    inline Qt3DCore::QNodeId currentFocusItem() const { return m_currentFocusItem; }
    inline Qt3DCore::QNodeId lastKeyboardInputRequester() const { return m_lastRequester; }
    inline QVector<Qt3DCore::QNodeId> keyboardInputs() const { return m_keyboardInputs; }
    inline QVector<HKeyboardInput> keyboardInputsHandles() const { return m_keyboardInputHandles; }

protected:
    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &) Q_DECL_OVERRIDE;

private:
    InputHandler *m_inputHandler;
    QVector<Qt3DCore::QNodeId> m_keyboardInputs;
    QVector<HKeyboardInput> m_keyboardInputHandles;
    Qt3DCore::QNodeId m_lastRequester;
    Qt3DCore::QNodeId m_currentFocusItem;
};

class KeyboardControllerFunctor : public Qt3DCore::QBackendNodeFunctor
{
public:
    explicit KeyboardControllerFunctor(InputHandler *handler);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNode *frontend, const Qt3DCore::QBackendNodeFactory *factory) const Q_DECL_OVERRIDE;
    Qt3DCore::QBackendNode *get(const Qt3DCore::QNodeId &id) const Q_DECL_OVERRIDE;
    void destroy(const Qt3DCore::QNodeId &id) const Q_DECL_OVERRIDE;

private:
    InputHandler *m_handler;
};

} // namespace Input
} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_KEYBOARDCONTROLLER_P_H
