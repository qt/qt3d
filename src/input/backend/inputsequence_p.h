// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_INPUT_INPUTSEQUENCE_H
#define QT3DINPUT_INPUT_INPUTSEQUENCE_H

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

#include <Qt3DCore/qnodeid.h>

#include <Qt3DInput/private/abstractactioninput_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

namespace Input {

class Q_AUTOTEST_EXPORT InputSequence : public AbstractActionInput
{
public:
    InputSequence();
    void cleanup();

    inline QList<Qt3DCore::QNodeId> sequences() const { return m_sequences; }
    inline qint64 timeout() const { return m_timeout; }
    inline qint64 buttonInterval() const { return m_buttonInterval; }
    inline qint64 startTime() const { return m_startTime; }
    void setStartTime(qint64 time);
    bool sequenceTriggered() const;
    void reset();
    bool actionTriggered(Qt3DCore::QNodeId input, const qint64 currentTime);
    void syncFromFrontEnd(const Qt3DCore::QNode *frontEnd, bool firstTime) override;

    bool process(InputHandler *inputHandler, qint64 currentTime) override;

private:
    QList<Qt3DCore::QNodeId> m_sequences;
    QList<Qt3DCore::QNodeId> m_inputsToTrigger;
    qint64 m_timeout;
    qint64 m_buttonInterval;
    qint64 m_startTime;
    qint64 m_lastInputTime;
    Qt3DCore::QNodeId m_lastInputId;
};

} // namespace Input

} // namespace Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_INPUT_INPUTSEQUENCE_H
