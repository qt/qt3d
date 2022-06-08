// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QINPUTSEQUENCE_P_H
#define QT3DINPUT_QINPUTSEQUENCE_P_H

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

#include <Qt3DInput/private/qabstractactioninput_p.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractActionInput;

/*!
    \class Qt3DInput::QInputChordSequence
    \internal
*/
class QInputSequencePrivate : public QAbstractActionInputPrivate
{
public:
    QInputSequencePrivate();

    int m_timeout;
    int m_buttonInterval;
    QList<QAbstractActionInput *> m_sequences;
};

struct QInputSequenceData
{
    Qt3DCore::QNodeIdVector sequenceIds;
    int timeout;
    int buttonInterval;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTSEQUENCE_P_H

