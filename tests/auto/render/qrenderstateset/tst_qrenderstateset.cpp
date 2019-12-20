/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
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
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>
#include <Qt3DCore/qentity.h>

#include <Qt3DRender/qrenderstateset.h>
#include <Qt3DRender/private/qrenderstate_p.h>
#include <Qt3DRender/qrenderstate.h>
#include <Qt3DRender/private/qrenderstateset_p.h>

#include "testarbiter.h"

class MyStateSet;
class MyStateSetPrivate : public Qt3DRender::QRenderStatePrivate
{
public :
    MyStateSetPrivate()
        : QRenderStatePrivate(Qt3DRender::Render::DepthTestStateMask)
    {}
};


class MyStateSet : public Qt3DRender::QRenderState
{
    Q_OBJECT
public:
    explicit MyStateSet(Qt3DCore::QNode *parent = nullptr)
        : Qt3DRender::QRenderState(*new MyStateSetPrivate(), parent)
    {}

private:
    Q_DECLARE_PRIVATE(MyStateSet)
};

class tst_QRenderStateSet: public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QRenderStateSet> defaultstateSet(new Qt3DRender::QRenderStateSet);
        QVERIFY(defaultstateSet->renderStates().isEmpty());
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QRenderStateSet> stateSet(new Qt3DRender::QRenderStateSet());
        arbiter.setArbiterOnNode(stateSet.data());

        // WHEN
        Qt3DRender::QRenderState *state1 = new MyStateSet();
        stateSet->addRenderState(state1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(stateSet.data()));

        arbiter.clear();

        // WHEN
        stateSet->addRenderState(state1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 0);
        // WHEN
        stateSet->removeRenderState(state1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QVERIFY(arbiter.dirtyNodes().contains(stateSet.data()));

        arbiter.clear();
    }

    void checkRenderStateBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QRenderStateSet> renderStateSet(new Qt3DRender::QRenderStateSet);
        {
            // WHEN
            MyStateSet state;
            renderStateSet->addRenderState(&state);

            // THEN
            QCOMPARE(state.parent(), renderStateSet.data());
            QCOMPARE(renderStateSet->renderStates().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(renderStateSet->renderStates().empty());

        {
            // WHEN
            Qt3DRender::QRenderStateSet someOtherStateSet;
            QScopedPointer<Qt3DRender::QRenderState> state(new MyStateSet(&someOtherStateSet));
            renderStateSet->addRenderState(state.data());

            // THEN
            QCOMPARE(state->parent(), &someOtherStateSet);
            QCOMPARE(renderStateSet->renderStates().size(), 1);

            // WHEN
            renderStateSet.reset();
            state.reset();

            // THEN Should not crash when the state is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QRenderStateSet)

#include "tst_qrenderstateset.moc"
