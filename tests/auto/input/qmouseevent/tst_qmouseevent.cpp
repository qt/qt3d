/****************************************************************************
**
** Copyright (C) 2019 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:GPL-EXCEPT$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3 as published by the Free Software
** Foundation with exceptions as appearing in the file LICENSE.GPL3-EXCEPT
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/


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
