// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QINPUTSEQUENCE_H
#define QT3DINPUT_QINPUTSEQUENCE_H

#include <Qt3DInput/qabstractactioninput.h>
#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qnode.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QAbstractPhysicalDevice;
class QInputSequencePrivate;

class Q_3DINPUTSHARED_EXPORT QInputSequence : public Qt3DInput::QAbstractActionInput
{
    Q_OBJECT
    Q_PROPERTY(int timeout READ timeout WRITE setTimeout NOTIFY timeoutChanged)
    Q_PROPERTY(int buttonInterval READ buttonInterval WRITE setButtonInterval NOTIFY buttonIntervalChanged)

public:
    explicit QInputSequence(Qt3DCore::QNode *parent = nullptr);
    ~QInputSequence();

    int timeout() const;
    int buttonInterval() const;

    void addSequence(QAbstractActionInput *input);
    void removeSequence(QAbstractActionInput *input);
    QList<QAbstractActionInput *> sequences() const;

public Q_SLOTS:
    void setTimeout(int timeout);
    void setButtonInterval(int buttonInterval);

Q_SIGNALS:
    void timeoutChanged(int timeout);
    void buttonIntervalChanged(int buttonInterval);

private:
    Q_DECLARE_PRIVATE(QInputSequence)
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTSEQUENCE_H
