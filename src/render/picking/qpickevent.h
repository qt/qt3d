// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QPICKEVENT_H
#define QT3DRENDER_QPICKEVENT_H

#include <QtCore/QObject>
#include <QtGui/QVector3D>
#include <QtCore/QPointF>
#include <Qt3DCore/qentity.h>
#include <Qt3DRender/qviewport.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QPickEventPrivate;
class QPickEvent;
typedef QSharedPointer<QPickEvent> QPickEventPtr;

class Q_3DRENDERSHARED_EXPORT QPickEvent : public QObject
{
    Q_OBJECT
    Q_PROPERTY(bool accepted READ isAccepted WRITE setAccepted NOTIFY acceptedChanged)
    Q_PROPERTY(QPointF position READ position CONSTANT)
    Q_PROPERTY(float distance READ distance CONSTANT)
    Q_PROPERTY(QVector3D localIntersection READ localIntersection CONSTANT)
    Q_PROPERTY(QVector3D worldIntersection READ worldIntersection CONSTANT)
    Q_PROPERTY(Qt3DRender::QPickEvent::Buttons button READ button CONSTANT)
    Q_PROPERTY(int buttons READ buttons CONSTANT)
    Q_PROPERTY(int modifiers READ modifiers CONSTANT)
    Q_PROPERTY(Qt3DRender::QViewport *viewport READ viewport CONSTANT REVISION 14)
    Q_PROPERTY(Qt3DCore::QEntity *entity READ entity CONSTANT REVISION 14)
public:
    enum Buttons {
        LeftButton = Qt::LeftButton,
        RightButton = Qt::RightButton,
        MiddleButton = Qt::MiddleButton,
        BackButton = Qt::BackButton,
        NoButton = Qt::NoButton
    };
    Q_ENUM(Buttons) // LCOV_EXCL_LINE

    enum Modifiers {
        NoModifier = Qt::NoModifier,
        ShiftModifier = Qt::ShiftModifier,
        ControlModifier = Qt::ControlModifier,
        AltModifier = Qt::AltModifier,
        MetaModifier = Qt::MetaModifier,
        KeypadModifier = Qt::KeypadModifier
    };
    Q_ENUM(Modifiers) // LCOV_EXCL_LINE

    QPickEvent();
    QPickEvent(const QPointF &position, const QVector3D& worldIntersection, const QVector3D& localIntersection, float distance);
    QPickEvent(const QPointF &position, const QVector3D& worldIntersection, const QVector3D& localIntersection, float distance, Buttons button,
               int buttons, int modifiers);
    ~QPickEvent();

    bool isAccepted() const;

public Q_SLOTS:
    void setAccepted(bool accepted);

public:
    QPointF position() const;
    float distance() const;
    QVector3D worldIntersection() const;
    QVector3D localIntersection() const;
    Buttons button() const;
    int buttons() const;
    int modifiers() const;
    QViewport *viewport() const;
    Qt3DCore::QEntity *entity() const;

Q_SIGNALS:
    void acceptedChanged(bool accepted);

protected:
    explicit QPickEvent(QObjectPrivate &dd, QObject *parent = nullptr);

private:
    Q_DECLARE_PRIVATE(QPickEvent)

    friend class QObjectPickerPrivate;
};

} // Qt3DRender

QT_END_NAMESPACE

Q_DECLARE_METATYPE(Qt3DRender::QPickEvent*) // LCOV_EXCL_LINE

#endif // QT3DRENDER_QPICKEVENT_H
