// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QINPUTCHORD_H
#define QT3DINPUT_QINPUTCHORD_H

#include <Qt3DInput/qabstractactioninput.h>
#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDevice;
class QInputChordPrivate;

class Q_3DINPUTSHARED_EXPORT QInputChord : public Qt3DInput::QAbstractActionInput
{
    Q_OBJECT
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)

public:
    explicit QInputChord(Qt3DCore::QNode *parent = nullptr);
    ~QInputChord();

    int timeout() const;

    void addChord(QAbstractActionInput *input);
    void removeChord(QAbstractActionInput *input);
    QList<QAbstractActionInput *> chords() const;

public Q_SLOTS:
    void setTimeout(int timeout);

Q_SIGNALS:
    void timeoutChanged(int timeout);

private:
    Q_DECLARE_PRIVATE(QInputChord)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTCHORD_H
