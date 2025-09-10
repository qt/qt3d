// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_ACTION_H
#define QT3DINPUT_INPUT_ACTION_H

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

#include <Qt3DInput/private/backendnode_p.h>
#include <Qt3DCore/qnodeid.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

class Q_AUTOTEST_EXPORT Action : public BackendNode
{
public:
    Action();
    void cleanup();
    inline QList<Qt3DCore::QNodeId> inputs() const { return m_inputs; }
    inline bool actionTriggered() const { return m_actionTriggered; }
    void setActionTriggered(bool actionTriggered);
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QList<Qt3DCore::QNodeId> m_inputs;
    bool m_actionTriggered;
};

} // Input

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_ACTION_H
