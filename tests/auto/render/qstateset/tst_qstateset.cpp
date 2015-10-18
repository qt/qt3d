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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qstateset.h>
#include <Qt3DRender/private/qrenderstate_p.h>
#include <Qt3DRender/qrenderstate.h>

#include "testpostmanarbiter.h"

class MyStateSet;
class MyStateSetPrivate : public Qt3DRender::QRenderStatePrivate
{
public :
    MyStateSetPrivate()
        : QRenderStatePrivate(Qt3DRender::QRenderState::DepthTest)
    {}
};


class MyStateSet : public Qt3DRender::QRenderState
{
    Q_OBJECT
public:
    explicit MyStateSet(Qt3DCore::QNode *parent = Q_NULLPTR)
        : Qt3DRender::QRenderState(*new MyStateSetPrivate(), parent)
    {}

    ~MyStateSet()
    {
        QNode::cleanup();
    }

private:
    QT3D_CLONEABLE(MyStateSet)
    Q_DECLARE_PRIVATE(MyStateSet)
};

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QStateSet: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QStateSet()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QStateSet> defaultstateSet(new Qt3DRender::QStateSet);

        QVERIFY(defaultstateSet->renderStates().isEmpty());
    }

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QStateSet *>("stateSet");
        QTest::addColumn<QList<Qt3DRender::QRenderState *> >("states");

        Qt3DRender::QStateSet *defaultConstructed = new Qt3DRender::QStateSet();
        QTest::newRow("defaultConstructed") << defaultConstructed << QList<Qt3DRender::QRenderState *>();

        Qt3DRender::QStateSet *stateSetWithStates = new Qt3DRender::QStateSet();
        Qt3DRender::QRenderState *state1 = new MyStateSet();
        Qt3DRender::QRenderState *state2 = new MyStateSet();
        QList<Qt3DRender::QRenderState *> states = QList<Qt3DRender::QRenderState *>() << state1 << state2;
        stateSetWithStates->addRenderState(state1);
        stateSetWithStates->addRenderState(state2);
        QTest::newRow("stateSetWithStates") << stateSetWithStates << states;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QStateSet*, stateSet);
        QFETCH(QList<Qt3DRender::QRenderState *>, states);

        // THEN
        QCOMPARE(stateSet->renderStates(), states);

        // WHEN
        Qt3DRender::QStateSet *clone = static_cast<Qt3DRender::QStateSet *>(QNode::clone(stateSet));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(stateSet->id(), clone->id());

        QCOMPARE(stateSet->renderStates().count(), clone->renderStates().count());

        for (int i = 0, m = states.count(); i < m; ++i) {
            Qt3DRender::QRenderState *sClone = clone->renderStates().at(i);
            Qt3DRender::QRenderState *sOrig = states.at(i);
            QCOMPARE(sOrig->id(),sClone->id());
            QVERIFY(sClone->parent() == clone);
            QVERIFY(sOrig->parent() == stateSet);
        }

        delete stateSet;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QStateSet> stateSet(new Qt3DRender::QStateSet());
        TestArbiter arbiter(stateSet.data());

        // WHEN
        Qt3DRender::QRenderState *state1 = new MyStateSet();
        stateSet->addRenderState(state1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "renderState");
        QCOMPARE(change->subjectId(), stateSet->id());
        Qt3DCore::QNodePtr clonedState = change->value().value<Qt3DCore::QNodePtr>();
        QVERIFY(!clonedState.isNull());
        QCOMPARE(clonedState->id(), state1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        stateSet->addRenderState(state1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        stateSet->removeRenderState(state1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "renderState");
        QCOMPARE(change->subjectId(), stateSet->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), state1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QStateSet)

#include "tst_qstateset.moc"
