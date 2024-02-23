// Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only


#include <QtTest/QTest>
#include <Qt3DInput/qmouseevent.h>
#include <QtGui/QMouseEvent>
#include <QObject>
#include <QSignalSpy>

class tst_QMouseEvent : public QObject
{
    Q_OBJECT

private Q_SLOTS:
    void checkModifiers()
    {
        {
            // GIVEN
            QT_PREPEND_NAMESPACE(QMouseEvent) event(QEvent::MouseButtonPress,
                                                    QPointF(500, 500),
                                                    QPointF(500, 500),
                                                    Qt::LeftButton,
                                                    Qt::LeftButton,
                                                    Qt::NoModifier);

            // WHEN
            Qt3DInput::QMouseEvent event3D(event);

            // THEN
            QCOMPARE(event3D.modifiers(), Qt3DInput::QMouseEvent::NoModifier);
        }
        {
            // GIVEN
            QT_PREPEND_NAMESPACE(QMouseEvent) event(QEvent::MouseButtonPress,
                                                    QPointF(500, 500),
                                                    QPointF(500, 500),
                                                    Qt::LeftButton,
                                                    Qt::LeftButton,
                                                    Qt::ShiftModifier);

            // WHEN
            Qt3DInput::QMouseEvent event3D(event);

            // THEN
            QCOMPARE(event3D.modifiers(), Qt3DInput::QMouseEvent::ShiftModifier);
        }
        {
            // GIVEN
            QT_PREPEND_NAMESPACE(QMouseEvent) event(QEvent::MouseButtonPress,
                                                    QPointF(500, 500),
                                                    QPointF(500, 500),
                                                    Qt::LeftButton,
                                                    Qt::LeftButton,
                                                    Qt::ShiftModifier|Qt::ControlModifier);

            // WHEN
            Qt3DInput::QMouseEvent event3D(event);

            // THEN
            QCOMPARE(int(event3D.modifiers()), int(Qt3DInput::QMouseEvent::ShiftModifier|Qt3DInput::QMouseEvent::ControlModifier));
        }
    }
};

QTEST_MAIN(tst_QMouseEvent)

#include "tst_qmouseevent.moc"
