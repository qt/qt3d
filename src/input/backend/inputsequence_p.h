/****************************************************************************
**
** Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:COMM$
**
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** $QT_END_LICENSE$
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
**
****************************************************************************/

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

    inline QVector<Qt3DCore::QNodeId> sequences() const { return m_sequences; }
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
    QVector<Qt3DCore::QNodeId> m_sequences;
    QVector<Qt3DCore::QNodeId> m_inputsToTrigger;
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
