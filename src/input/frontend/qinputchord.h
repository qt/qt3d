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
******************************************************************************/

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
