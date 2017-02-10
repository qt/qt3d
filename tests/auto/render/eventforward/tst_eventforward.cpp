/****************************************************************************
**
** Copyright (C) 2017 The Qt Company Ltd.
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
#include <Qt3DRender/qeventforward.h>
#include <private/qeventforward_p.h>
#include <private/eventforward_p.h>
#include <private/posteventstofrontend_p.h>
#include <private/qbackendnode_p.h>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include "qbackendnodetester.h"
#include "testrenderer.h"
#include "testpostmanarbiter.h"


class tst_EventForward : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT

private Q_SLOTS:

    void checkInitialState()
    {
        // GIVEN
        Qt3DRender::Render::EventForward backendEventForward;

        // THEN
        QCOMPARE(backendEventForward.isEnabled(), false);
        QVERIFY(backendEventForward.peerId().isNull());
        QCOMPARE(backendEventForward.coordinateAttribute(), QStringLiteral(""));
        QCOMPARE(backendEventForward.coordinateTransform(), QMatrix4x4());
        QCOMPARE(backendEventForward.forwardMouseEvents(), false);
        QCOMPARE(backendEventForward.forwardKeyboardEvents(), false);
        QCOMPARE(backendEventForward.focus(), false);
    }

    void checkCleanupState()
    {
        // GIVEN
        Qt3DRender::Render::EventForward backendEventForward;
        QScopedPointer<QObject> obj(new QObject());
        QMatrix4x4 transform;
        transform.scale(1.0f, 2.0f, 3.0f);

        // WHEN
        backendEventForward.setEnabled(true);
        backendEventForward.setCoordinateAttribute(QStringLiteral("default"));
        backendEventForward.setCoordinateTransform(transform);
        backendEventForward.setForwardMouseEvents(true);
        backendEventForward.setForwardKeyboardEvents(true);
        backendEventForward.setFocus(true);

        backendEventForward.cleanup();

        // THEN
        QCOMPARE(backendEventForward.isEnabled(), false);
        QCOMPARE(backendEventForward.coordinateAttribute(), QStringLiteral(""));
        QCOMPARE(backendEventForward.coordinateTransform(), QMatrix4x4());
        QCOMPARE(backendEventForward.forwardMouseEvents(), false);
        QCOMPARE(backendEventForward.forwardKeyboardEvents(), false);
        QCOMPARE(backendEventForward.focus(), false);
    }

    void checkInitializeFromPeer()
    {
        // GIVEN
        Qt3DRender::QEventForward eventForward;

        {
            // WHEN
            Qt3DRender::Render::EventForward backendEventForward;
            simulateInitialization(&eventForward, &backendEventForward);

            // THEN
            QCOMPARE(backendEventForward.isEnabled(), true);
            QCOMPARE(backendEventForward.peerId(), eventForward.id());
            QCOMPARE(backendEventForward.coordinateAttribute(), QStringLiteral("default"));
            QCOMPARE(backendEventForward.coordinateTransform(), QMatrix4x4());
            QCOMPARE(backendEventForward.forwardMouseEvents(), true);
            QCOMPARE(backendEventForward.forwardKeyboardEvents(), false);
            QCOMPARE(backendEventForward.focus(), false);
        }
        {
            // WHEN
            Qt3DRender::Render::EventForward backendEventForward;
            eventForward.setEnabled(false);
            eventForward.setFocus(true);
            simulateInitialization(&eventForward, &backendEventForward);

            // THEN
            QCOMPARE(backendEventForward.peerId(), eventForward.id());
            QCOMPARE(backendEventForward.isEnabled(), false);
            QCOMPARE(backendEventForward.focus(), true);
        }
    }

    void checkSceneChangeEvents()
    {
        // GIVEN
        Qt3DRender::Render::EventForward backendEventForward;
        TestRenderer renderer;
        backendEventForward.setRenderer(&renderer);

        {
             // WHEN
             const bool newValue = false;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("enabled");
             change->setValue(newValue);
             backendEventForward.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendEventForward.isEnabled(), newValue);
        }
        {
             // WHEN
             const QString newValue = QStringLiteral("position");
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("coordinateAttribute");
             change->setValue(QVariant::fromValue(newValue));
             backendEventForward.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendEventForward.coordinateAttribute(), newValue);
        }
        {
             // WHEN
             QMatrix4x4 newValue;
             newValue.rotate(2.0f, QVector3D(0, 0, 1.0f));
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("coordinateTransform");
             change->setValue(QVariant::fromValue(newValue));
             backendEventForward.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendEventForward.coordinateTransform(), newValue);
        }
        {
             // WHEN
             const bool newValue = true;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("forwardMouseEvents");
             change->setValue(QVariant::fromValue(newValue));
             backendEventForward.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendEventForward.forwardMouseEvents(), newValue);
        }
        {
             // WHEN
             const bool newValue = true;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("forwardKeyboardEvents");
             change->setValue(QVariant::fromValue(newValue));
             backendEventForward.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendEventForward.forwardKeyboardEvents(), newValue);
        }
        {
             // WHEN
             const bool newValue = true;
             const auto change = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
             change->setPropertyName("focus");
             change->setValue(QVariant::fromValue(newValue));
             backendEventForward.sceneChangeEvent(change);

             // THEN
            QCOMPARE(backendEventForward.focus(), newValue);
        }
    }
    void checkEventForwarding()
    {
        Qt3DRender::Render::EventForward backendEventForward;
        TestRenderer renderer;
        TestArbiter arbiter;
        backendEventForward.setRenderer(&renderer);
        Qt3DCore::QBackendNodePrivate::get(&backendEventForward)->setArbiter(&arbiter);

        {
            // WHEN
            const QPointF localPos;
            QMouseEvent event = QMouseEvent(QEvent::MouseButtonPress, localPos,
                                            Qt::LeftButton, Qt::LeftButton, 0);

            const QVector4D coordinate;
            backendEventForward.forward(event, coordinate);
            QCoreApplication::processEvents();

            // THEN
            QVERIFY(arbiter.events.size() == 1);
            Qt3DCore::QPropertyUpdatedChangePtr change
                    = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "events");

            Qt3DRender::PostEventsToFrontendPtr postedEvents
                    = change->value().value<Qt3DRender::PostEventsToFrontendPtr>();
            QVERIFY(postedEvents->events().size() == 1);

            QEvent *receivedEvent = postedEvents->events().first();
            QCOMPARE(receivedEvent->type(), QEvent::MouseButtonPress);

            QMouseEvent *me = static_cast<QMouseEvent*>(receivedEvent);
            QCOMPARE(me->localPos(), localPos);
            QCOMPARE(me->button(), Qt::LeftButton);
            QCOMPARE(me->buttons(), Qt::LeftButton);
            QCOMPARE(me->modifiers(), 0);

            arbiter.events.clear();
        }

        {
            // WHEN
            QKeyEvent event1 = QKeyEvent(QEvent::KeyPress, 48, 0);
            QKeyEvent event2 = QKeyEvent(QEvent::KeyRelease, 48, 0);
            QList<QKeyEvent> eventList;
            eventList.push_back(event1);
            eventList.push_back(event2);

            backendEventForward.forward(eventList);
            QCoreApplication::processEvents();

            // THEN
            QVERIFY(arbiter.events.size() == 1);
            Qt3DCore::QPropertyUpdatedChangePtr change
                    = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "events");

            Qt3DRender::PostEventsToFrontendPtr postedEvents
                    = change->value().value<Qt3DRender::PostEventsToFrontendPtr>();
            QVERIFY(postedEvents->events().size() == 2);

            QEvent *receivedEvent1 = postedEvents->events().first();
            QCOMPARE(receivedEvent1->type(), QEvent::KeyPress);

            QEvent *receivedEvent2 = postedEvents->events().at(1);
            QCOMPARE(receivedEvent2->type(), QEvent::KeyRelease);

            QKeyEvent *ke1 = static_cast<QKeyEvent*>(receivedEvent1);
            QKeyEvent *ke2 = static_cast<QKeyEvent*>(receivedEvent2);
            QCOMPARE(ke1->key(), 48);
            QCOMPARE(ke1->modifiers(), 0);
            QCOMPARE(ke2->key(), 48);
            QCOMPARE(ke2->modifiers(), 0);

            arbiter.events.clear();
        }
    }
};

QTEST_MAIN(tst_EventForward)

#include "tst_eventforward.moc"
