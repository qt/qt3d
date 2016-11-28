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
#include <Qt3DRender/qattribute.h>
#include <Qt3DRender/qeventforward.h>
#include <private/qeventforward_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <private/qnodecreatedchangegenerator_p.h>
#include <private/posteventstofrontend_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include "testpostmanarbiter.h"


class EventReceiver : public QObject
{
public:
    EventReceiver()
        : QObject()
    {

    }

    ~EventReceiver()
    {

    }

    int eventCount()
    {
        return m_events.size();
    }

    QEvent *eventAt(int index) const
    {
        return m_events.at(index);
    }
    bool event(QEvent *event) Q_DECL_OVERRIDE
    {
        m_events.push_back(event);
        return true;
    }

    void clearEvents()
    {
        m_events.clear();
    }

private:
    QVector<QEvent *> m_events;
};

class TestEventForward : public Qt3DRender::QEventForward
{
    Q_OBJECT
public:
    TestEventForward(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QEventForward(parent)
    {}

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_FINAL
    {
        Qt3DRender::QEventForward::sceneChangeEvent(change);
    }

private:
    friend class tst_QEventForward;

};


class tst_QEventForward : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QEventForward eventForward;

        // THEN
        QCOMPARE(eventForward.target(), nullptr);
        QCOMPARE(eventForward.forwardMouseEvents(), true);
        QCOMPARE(eventForward.forwardKeyboardEvents(), false);
        QCOMPARE(eventForward.coordinateTransform(), QMatrix4x4());
        QCOMPARE(eventForward.coordinateAttribute(), QStringLiteral("default"));
        QCOMPARE(eventForward.focus(), false);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QEventForward eventForward;

        {
            // WHEN
            QScopedPointer<QObject> obj(new QObject());
            QSignalSpy spy(&eventForward, SIGNAL(targetChanged(QObject*)));
            QObject* newValue = obj.data();
            eventForward.setTarget(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(eventForward.target(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            eventForward.setTarget(newValue);

            // THEN
            QCOMPARE(eventForward.target(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&eventForward, SIGNAL(forwardMouseEventsChanged(bool)));
            const bool newValue = false;
            eventForward.setForwardMouseEvents(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(eventForward.forwardMouseEvents(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            eventForward.setForwardMouseEvents(newValue);

            // THEN
            QCOMPARE(eventForward.forwardMouseEvents(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&eventForward, SIGNAL(forwardKeyboardEventsChanged(bool)));
            const bool newValue = true;
            eventForward.setForwardKeyboardEvents(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(eventForward.forwardKeyboardEvents(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            eventForward.setForwardKeyboardEvents(newValue);

            // THEN
            QCOMPARE(eventForward.forwardKeyboardEvents(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&eventForward, SIGNAL(coordinateTransformChanged(QMatrix4x4)));
            QMatrix4x4 newValue;
            newValue.scale(3.0f);
            eventForward.setCoordinateTransform(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(eventForward.coordinateTransform(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            eventForward.setCoordinateTransform(newValue);

            // THEN
            QCOMPARE(eventForward.coordinateTransform(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&eventForward, SIGNAL(coordinateAttributeChanged(QString)));
            const QString newValue = Qt3DRender::QAttribute::defaultColorAttributeName();
            eventForward.setCoordinateAttribute(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(eventForward.coordinateAttribute(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            eventForward.setCoordinateAttribute(newValue);

            // THEN
            QCOMPARE(eventForward.coordinateAttribute(), newValue);
            QCOMPARE(spy.count(), 0);
        }
        {
            // WHEN
            QSignalSpy spy(&eventForward, SIGNAL(focusChanged(bool)));
            const bool newValue = true;
            eventForward.setFocus(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(eventForward.focus(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            eventForward.setFocus(newValue);

            // THEN
            QCOMPARE(eventForward.focus(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DRender::QEventForward eventForward;
        QScopedPointer<QObject> obj(new QObject());
        QMatrix4x4 transform;
        transform.translate(1.0f, 2.0f, 3.0f);

        eventForward.setTarget(obj.data());
        eventForward.setForwardMouseEvents(true);
        eventForward.setForwardKeyboardEvents(true);
        eventForward.setCoordinateTransform(transform);
        eventForward.setCoordinateAttribute(QStringLiteral("position"));
        eventForward.setFocus(true);

        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&eventForward);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData
                    = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<
                        Qt3DRender::QEventForwardData>>(creationChanges.first());
            const Qt3DRender::QEventForwardData cloneData = creationChangeData->data;

            QCOMPARE(eventForward.target(), cloneData.target);
            QCOMPARE(eventForward.forwardMouseEvents(), cloneData.forwardMouseEvents);
            QCOMPARE(eventForward.forwardKeyboardEvents(), cloneData.forwardKeyboardEvents);
            QCOMPARE(eventForward.coordinateTransform(), cloneData.coordinateTransform);
            QCOMPARE(eventForward.coordinateAttribute(), cloneData.coordinateAttribute);
            QCOMPARE(eventForward.focus(), cloneData.focus);
            QCOMPARE(eventForward.id(), creationChangeData->subjectId());
            QCOMPARE(eventForward.isEnabled(), true);
            QCOMPARE(eventForward.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(eventForward.metaObject(), creationChangeData->metaObject());
        }

        // WHEN
        eventForward.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&eventForward);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData
                    = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<
                        Qt3DRender::QEventForwardData>>(creationChanges.first());
            const Qt3DRender::QEventForwardData cloneData = creationChangeData->data;

            QCOMPARE(eventForward.target(), cloneData.target);
            QCOMPARE(eventForward.forwardMouseEvents(), cloneData.forwardMouseEvents);
            QCOMPARE(eventForward.forwardKeyboardEvents(), cloneData.forwardKeyboardEvents);
            QCOMPARE(eventForward.coordinateTransform(), cloneData.coordinateTransform);
            QCOMPARE(eventForward.coordinateAttribute(), cloneData.coordinateAttribute);
            QCOMPARE(eventForward.focus(), cloneData.focus);
            QCOMPARE(eventForward.id(), creationChangeData->subjectId());
            QCOMPARE(eventForward.isEnabled(), false);
            QCOMPARE(eventForward.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(eventForward.metaObject(), creationChangeData->metaObject());
        }
    }

    void checkTargetUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<QObject> obj(new QObject());
        Qt3DRender::QEventForward eventForward;
        arbiter.setArbiterOnNode(&eventForward);

        {
            // WHEN
            eventForward.setTarget(obj.data());
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "target");
            QCOMPARE(change->value().value<QObject*>(), eventForward.target());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            eventForward.setTarget(obj.data());
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

    void checkForwardMouseEventsUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QEventForward eventForward;
        arbiter.setArbiterOnNode(&eventForward);

        {
            // WHEN
            eventForward.setForwardMouseEvents(false);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "forwardMouseEvents");
            QCOMPARE(change->value().value<bool>(), eventForward.forwardMouseEvents());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            eventForward.setForwardMouseEvents(false);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

    void checkForwardKeyboardEventsUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QEventForward eventForward;
        arbiter.setArbiterOnNode(&eventForward);

        {
            // WHEN
            eventForward.setForwardKeyboardEvents(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "forwardKeyboardEvents");
            QCOMPARE(change->value().value<bool>(), eventForward.forwardKeyboardEvents());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            eventForward.setForwardKeyboardEvents(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

    void checkCoordinateTransformUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        QMatrix4x4 transform;
        Qt3DRender::QEventForward eventForward;
        arbiter.setArbiterOnNode(&eventForward);
        transform.translate(2.0f, 1.0f, 4.0f);

        {
            // WHEN
            eventForward.setCoordinateTransform(transform);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "coordinateTransform");
            QCOMPARE(change->value().value<QMatrix4x4>(), eventForward.coordinateTransform());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            eventForward.setCoordinateTransform(transform);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

    void checkCoordinateAttributeUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QEventForward eventForward;
        arbiter.setArbiterOnNode(&eventForward);

        {
            // WHEN
            eventForward.setCoordinateAttribute(QStringLiteral("normal"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "coordinateAttribute");
            QCOMPARE(change->value().value<QString>(), eventForward.coordinateAttribute());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            eventForward.setCoordinateAttribute(QStringLiteral("normal"));
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }

    }

    void checkFocusUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QEventForward eventForward;
        arbiter.setArbiterOnNode(&eventForward);

        {
            // WHEN
            eventForward.setFocus(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 1);
            auto change = arbiter.events.first().staticCast<Qt3DCore::QPropertyUpdatedChange>();
            QCOMPARE(change->propertyName(), "focus");
            QCOMPARE(change->value().toBool(), eventForward.focus());
            QCOMPARE(change->type(), Qt3DCore::PropertyUpdated);

            arbiter.events.clear();
        }

        {
            // WHEN
            eventForward.setFocus(true);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
        }
    }

    void checkReceiveEvents()
    {
        EventReceiver receiver;
        TestEventForward eventForward;
        eventForward.setTarget(&receiver);
        eventForward.setForwardKeyboardEvents(true);
        eventForward.setForwardMouseEvents(true);

        {
            // WHEN
            const QPointF local = QPointF();
            QMouseEvent* mouseEvent
                    = new QMouseEvent(QEvent::MouseButtonPress, local, local, local,
                            Qt::LeftButton,0,0, Qt::MouseEventSynthesizedByApplication);
            Qt3DRender::PostEventsToFrontendPtr events
                    = Qt3DRender::PostEventsToFrontendPtr::create(mouseEvent);

            auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            e->setPropertyName("events");
            e->setValue(QVariant::fromValue(events));

            eventForward.sceneChangeEvent(e);

            // THEN
            QVERIFY(receiver.eventCount() == 1);
            QEvent *event = receiver.eventAt(0);
            QCOMPARE(event->type(), QEvent::MouseButtonPress);

            QMouseEvent *me = static_cast<QMouseEvent *>(receiver.eventAt(0));
            QCOMPARE(me->localPos(), local);
            QCOMPARE(me->button(), Qt::LeftButton);

            receiver.clearEvents();
        }

        {
            // WHEN
            QKeyEvent *event1 = new QKeyEvent(QEvent::KeyPress, 48, 0);
            QKeyEvent *event2 = new QKeyEvent(QEvent::KeyRelease, 48, 0);
            QVector<QEvent *> eventList;
            eventList.push_back(event1);
            eventList.push_back(event2);
            Qt3DRender::PostEventsToFrontendPtr events
                    = Qt3DRender::PostEventsToFrontendPtr::create(eventList);

            auto e = Qt3DCore::QPropertyUpdatedChangePtr::create(Qt3DCore::QNodeId());
            e->setDeliveryFlags(Qt3DCore::QSceneChange::DeliverToAll);
            e->setPropertyName("events");
            e->setValue(QVariant::fromValue(events));

            eventForward.sceneChangeEvent(e);

            // THEN
            QVERIFY(receiver.eventCount() == 2);
            QEvent *event = receiver.eventAt(0);
            QCOMPARE(event->type(), QEvent::KeyPress);
            QCOMPARE(static_cast<QKeyEvent *>(event)->key(), 48);

            event = receiver.eventAt(1);
            QCOMPARE(event->type(), QEvent::KeyRelease);
            QCOMPARE(static_cast<QKeyEvent *>(event)->key(), 48);
        }
    }
};

QTEST_MAIN(tst_QEventForward)

#include "tst_qeventforward.moc"
