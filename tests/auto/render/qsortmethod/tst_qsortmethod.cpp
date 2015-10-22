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

#include <Qt3DRender/qsortmethod.h>
#include <Qt3DRender/qsortcriterion.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QSortMethod: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QSortMethod()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QSortMethod> defaultsortMethod(new Qt3DRender::QSortMethod);

        QVERIFY(defaultsortMethod->criteria().isEmpty());
    }

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QSortMethod *>("sortMethod");
        QTest::addColumn<QList<Qt3DRender::QSortCriterion *> >("criteria");

        Qt3DRender::QSortMethod *defaultConstructed = new Qt3DRender::QSortMethod();
        QTest::newRow("defaultConstructed") << defaultConstructed << QList<Qt3DRender::QSortCriterion *>();

        Qt3DRender::QSortMethod *sortMethodWithCriteria = new Qt3DRender::QSortMethod();
        Qt3DRender::QSortCriterion *criterion1 = new Qt3DRender::QSortCriterion();
        Qt3DRender::QSortCriterion *criterion2 = new Qt3DRender::QSortCriterion();
        criterion1->setSort(Qt3DRender::QSortCriterion::BackToFront);
        criterion2->setSort(Qt3DRender::QSortCriterion::Material);
        QList<Qt3DRender::QSortCriterion *> criteria = QList<Qt3DRender::QSortCriterion *>() << criterion1 << criterion2;
        sortMethodWithCriteria->addCriterion(criterion1);
        sortMethodWithCriteria->addCriterion(criterion2);
        QTest::newRow("sortMethodWithCriteria") << sortMethodWithCriteria << criteria;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QSortMethod*, sortMethod);
        QFETCH(QList<Qt3DRender::QSortCriterion *>, criteria);

        // THEN
        QCOMPARE(sortMethod->criteria(), criteria);

        // WHEN
        Qt3DRender::QSortMethod *clone = static_cast<Qt3DRender::QSortMethod *>(QNode::clone(sortMethod));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(sortMethod->id(), clone->id());

        QCOMPARE(sortMethod->criteria().count(), clone->criteria().count());

        for (int i = 0, m = criteria.count(); i < m; ++i) {
            Qt3DRender::QSortCriterion *cClone = clone->criteria().at(i);
            Qt3DRender::QSortCriterion *cOrig = criteria.at(i);
            QCOMPARE(cOrig->id(),cClone->id());
            QCOMPARE(cOrig->sort(), cClone->sort());
            QVERIFY(cClone->parent() == clone);
            QVERIFY(cOrig->parent() == sortMethod);
        }

        delete sortMethod;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QSortMethod> sortMethod(new Qt3DRender::QSortMethod());
        TestArbiter arbiter(sortMethod.data());

        // WHEN
        Qt3DRender::QSortCriterion *criterion1 = new Qt3DRender::QSortCriterion();
        sortMethod->addCriterion(criterion1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "sortCriterion");
        QCOMPARE(change->subjectId(),sortMethod->id());
        QCOMPARE(change->value().value<Qt3DCore::QNodeId>(), criterion1->id());
        QCOMPARE(change->type(), Qt3DCore::NodeAdded);

        arbiter.events.clear();

        // WHEN
        sortMethod->addCriterion(criterion1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        sortMethod->removeCriterion(criterion1);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "sortCriterion");
        QCOMPARE(change->subjectId(), sortMethod->id());
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

QTEST_MAIN(tst_QSortMethod)

#include "tst_qsortmethod.moc"
