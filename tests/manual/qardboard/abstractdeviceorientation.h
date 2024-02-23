// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#ifndef ABSTRACT_DEVICEORIENTATION_H
#define ABSTRACT_DEVICEORIENTATION_H

#include <QObject>

class AbstractDeviceOrientation : public QObject
{
    Q_OBJECT
    Q_PROPERTY(qreal roll READ roll WRITE setRoll NOTIFY rollChanged)
    Q_PROPERTY(qreal pitch READ pitch WRITE setPitch NOTIFY pitchChanged)
    Q_PROPERTY(qreal yaw READ yaw WRITE setYaw NOTIFY yawChanged)
    Q_PROPERTY(bool enabled READ enabled WRITE setEnabled NOTIFY enabledChanged)

public:
    AbstractDeviceOrientation(QObject* parent = 0);

    qreal roll() const { return m_roll; }
    qreal pitch() const { return m_pitch; }
    qreal yaw() const { return m_yaw; }

    bool enabled() const;

signals:
    void rollChanged(qreal roll);
    void pitchChanged(qreal pitch);
    void yawChanged(qreal yaw);
    void enabledChanged(bool);

public slots:
    virtual void setRoll(qreal v);
    virtual void setPitch(qreal v);
    virtual void setYaw(qreal v);
    virtual void setEnabled(bool v);

    virtual bool start() = 0;            //!< Called to start the sensor services.
    virtual void stop();                 //!< Called to stop updates from the #CompassClino.

protected:
    qreal m_roll;
    qreal m_pitch;
    qreal m_yaw;
    bool m_enabled;
};


#endif // ABSTRACT_DEVICEORIENTATION_H
