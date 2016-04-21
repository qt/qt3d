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

#include <Qt3DRender/qsortpolicy.h>
#include <Qt3DRender/qsortcriterion.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QSortPolicy: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QSortPolicy()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QSortPolicy> defaultsortPolicy(new Qt3DRender::QSortPolicy);

        QVERIFY(defaultsortPolicy->criteria().isEmpty());
    }

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QSortPolicy *>("sortPolicy");
        QTest::addColumn<QVector<Qt3DRender::QSortCriterion *> >("criteria");

        Qt3DRender::QSortPolicy *defaultConstructed = new Qt3DRender::QSortPolicy();
        QTest::newRow("defaultConstructed") << defaultConstructed << QVector<Qt3DRender::QSortCriterion *>();

        Qt3DRender::QSortPolicy *sortPolicyWithCriteria = new Qt3DRender::QSortPolicy();
        Qt3DRender::QSortCriterion *criterion1 = new Qt3DRender::QSortCriterion();
        Qt3DRender::QSortCriterion *criterion2 = new Qt3DRender::QSortCriterion();
        criterion1->setSort(Qt3DRender::QSortCriterion::BackToFront);
        criterion2->setSort(Qt3DRender::QSortCriterion::Material);
        QVector<Qt3DRender::QSortCriterion *> criteria = QVector<Qt3DRender::QSortCriterion *>() << criterion1 << criterion2;
        sortPolicyWithCriteria->addCriterion(criterion1);
        sortPolicyWithCriteria->addCriterion(criterion2);
        QTest::newRow("sortPolicyWithCriteria") << sortPolicyWithCriteria << criteria;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QSortPolicy*, sortPolicy);
        QFETCH(QVector<Qt3DRender::QSortCriterion *>, criteria);

        // THEN
        QCOMPARE(sortPolicy->criteria(), criteria);

        // WHEN
        Qt3DRender::QSortPolicy *clone = static_cast<Qt3DRender::QSortPolicy *>(QNode::clone(sortPolicy));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(sortPolicy->id(), clone->id());

        QCOMPARE(sortPolicy->criteria().count(), clone->criteria().count());

        for (int i = 0, m = criteria.count(); i < m; ++i) {
            Qt3DRender::QSortCriterion *cClone = clone->criteria().at(i);
            Qt3DRender::QSortCriterion *cOrig = criteria.at(i);
            QCOMPARE(cOrig->id(),cClone->id());
            QCOMPARE(cOrig->sort(), cClone->sort());
            QVERIFY(cClone->parent() == clone);
            QVERIFY(cOrig->parent() == sortPolicy);
        }

        delete sortPolicy;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QSortPolicy> sortPolicy(new Qt3DRender::QSortPolicy());
        TestArbiter arbiter(sortPolicy.data());

        // WHEN
        Qt3DRender::QSortCriterion *criterion1 = new Qt3DRender::QSortCriterion();
        sortPolicy->addCriterion(criterion1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QNodePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "sortCriterion");
        QCOMPARE(change->subjectId(),sortPolicy->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), criterion1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        sortPolicy->addCriterion(criterion1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        sortPolicy->removeCriterion(criterion1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QNodePropertyChange>();
        QCOMPARE(change->propertyName(), "sortCriterion");
        QCOMPARE(change->subjectId(), sortPolicy->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), criterion1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeRemoved);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QSortPolicy)

#include "tst_qsortpolicy.moc"
