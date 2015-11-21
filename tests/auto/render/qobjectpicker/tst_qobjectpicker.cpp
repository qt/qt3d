/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL3$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see http://www.qt.io/terms-conditions. For further
** information use the contact form at http://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPLv3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or later as published by the Free
** Software Foundation and appearing in the file LICENSE.GPL included in
** the packaging of this file. Please review the following information to
** ensure the GNU General Public License version 2.0 requirements will be
** met: http://www.gnu.org/licenses/gpl-2.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QTest>
#include <QtTest/QSignalSpy>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qbackendscenepropertychange.h>
#include <Qt3DRender/QObjectPicker>
#include <Qt3DRender/QPickEvent>

#include "testpostmanarbiter.h"

class MyObjectPicker : public Qt3DRender::QObjectPicker
{
    Q_OBJECT
public:
    MyObjectPicker(Qt3DCore::QNode *parent = Q_NULLPTR)
        : Qt3DRender::QObjectPicker(parent)
    {}

    void sceneChangeEvent(const Qt3DCore::QSceneChangePtr &change) Q_DECL_FINAL
    {
        Qt3DRender::QObjectPicker::sceneChangeEvent(change);
    }

private:
    friend class tst_ObjectPicker;

};

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QObjectPicker : public Qt3DCore::QNode
{
    Q_OBJECT
public:
    tst_QObjectPicker()
    {
        qRegisterMetaType<Qt3DRender::QPickEvent*>("Qt3DRender::QPickEvent*");
    }

    ~tst_QObjectPicker()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QObjectPicker *>("objectPicker");

        Qt3DRender::QObjectPicker *objectPicker = new Qt3DRender::QObjectPicker();
        QTest::newRow("empty objectPicker") << objectPicker;
        objectPicker = new Qt3DRender::QObjectPicker();
        objectPicker->setHoverEnabled(true);
        QTest::newRow("objectPicker_all_true") << objectPicker;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QObjectPicker *, objectPicker);

        // WHEN
        Qt3DRender::QObjectPicker *clone = static_cast<Qt3DRender::QObjectPicker *>(QNode::clone(objectPicker));
        QCoreApplication::processEvents();

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(objectPicker->id(), clone->id());
        QCOMPARE(objectPicker->hoverEnabled(), clone->hoverEnabled());
        QCOMPARE(objectPicker->isPressed(), clone->isPressed());
        QCOMPARE(objectPicker->containsMouse(), clone->containsMouse());
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QObjectPicker> objectPicker(new Qt3DRender::QObjectPicker());
        TestArbiter arbiter(objectPicker.data());

        // WHEN
        objectPicker->setHoverEnabled(true);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.last().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "hoverEnabled");
        QCOMPARE(change->value().toBool(), true);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();
    }

    void checkBackendUpdates_data()
    {
        QTest::addColumn<QByteArray>("signalPrototype");
        QTest::addColumn<QByteArray>("propertyName");

        QTest::newRow("clicked")
                << QByteArray(SIGNAL(clicked(Qt3DRender::QPickEvent *)))
                << QByteArrayLiteral("clicked");

        QTest::newRow("pressed")
                << QByteArray(SIGNAL(pressed(Qt3DRender::QPickEvent *)))
                << QByteArrayLiteral("pressed");

        QTest::newRow("released")
                << QByteArray(SIGNAL(released(Qt3DRender::QPickEvent *)))
                << QByteArrayLiteral("released");

        QTest::newRow("entered")
                << QByteArray(SIGNAL(entered()))
                << QByteArrayLiteral("entered");

        QTest::newRow("exited")
                << QByteArray(SIGNAL(exited()))
                << QByteArrayLiteral("exited");
    }

    void checkBackendUpdates()
    {
        // GIVEN
        QFETCH(QByteArray, signalPrototype);
        QFETCH(QByteArray, propertyName);
        QScopedPointer<MyObjectPicker> objectPicker(new MyObjectPicker());
        QSignalSpy spy(objectPicker.data(), signalPrototype.constData());

        // WHEN
        // Create Backend Change and distribute it to frontend node
        Qt3DCore::QBackendScenePropertyChangePtr e(new Qt3DCore::QBackendScenePropertyChange(Qt3DCore::NodeUpdated, objectPicker->id()));
        e->setPropertyName(propertyName.constData());
        objectPicker->sceneChangeEvent(e);

        // THEN
        // Check that the QObjectPicker triggers the expected signal
        QCOMPARE(spy.count(), 1);
    }


protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QObjectPicker)

#include "tst_qobjectpicker.moc"
