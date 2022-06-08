// Copyright (C) 2016 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DINPUT_QINPUTSETTINGS_H
#define QT3DINPUT_QINPUTSETTINGS_H

#include <Qt3DInput/qt3dinput_global.h>
#include <Qt3DCore/qcomponent.h>

QT_BEGIN_NAMESPACE

namespace Qt3DInput {

class QInputSettingsPrivate;

class Q_3DINPUTSHARED_EXPORT QInputSettings : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(QObject *eventSource READ eventSource WRITE setEventSource NOTIFY eventSourceChanged)
public:
    explicit QInputSettings(Qt3DCore::QNode *parent = nullptr);
    ~QInputSettings();

    QObject *eventSource() const;

public Q_SLOTS:
    void setEventSource(QObject *eventSource);

Q_SIGNALS:
    void eventSourceChanged(QObject *);

private:
    Q_DECLARE_PRIVATE(QInputSettings)
    void eventSourceDestroyed();
};

} // Qt3DInput

QT_END_NAMESPACE

#endif // QINPUTSETTINGS_H
