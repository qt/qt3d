// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_INPUTSETTINGS_H
#define QT3DINPUT_INPUT_INPUTSETTINGS_H

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
#include <QtCore/QPointer>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

class InputHandler;

class InputSettings : public BackendNode
{
public:
    InputSettings();
    inline QObject *eventSource() const { return m_eventSource; }

    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

private:
    QPointer<QObject> m_eventSource;
};

class InputSettingsFunctor : public Qt3DCore::QBackendNodeMapper
{
public:
    explicit InputSettingsFunctor(InputHandler *handler);

    Qt3DCore::QBackendNode *create(Qt3DCore::QNodeId id) const override;
    Qt3DCore::QBackendNode *get(Qt3DCore::QNodeId id) const override;
    void destroy(Qt3DCore::QNodeId id) const override;

private:
    InputHandler *m_handler;
};

} // Input

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_INPUTSETTINGS_H
