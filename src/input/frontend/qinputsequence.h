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
    QVector<QAbstractActionInput *> sequences() const;

public Q_SLOTS:
    void setTimeout(int timeout);
    void setButtonInterval(int buttonInterval);

Q_SIGNALS:
    void timeoutChanged(int timeout);
    void buttonIntervalChanged(int buttonInterval);

private:
    Q_DECLARE_PRIVATE(QInputSequence)
    Qt3DCore::QNodeCreatedChangeBasePtr createNodeCreationChange() const override;
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QT3DINPUT_QINPUTSEQUENCE_H
