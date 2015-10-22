/****************************************************************************
**
** Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the S module of the Qt Toolkit.
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

#include <Qt3DRender/qsortcriterion.h>

#include "testpostmanarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QSortCriterion: public Qt3DCore::QNode
{
    Q_OBJECT
public:
    ~tst_QSortCriterion()
    {
        QNode::cleanup();
    }

private Q_SLOTS:

    void checkCloning_data()
    {
        QTest::addColumn<Qt3DRender::QSortCriterion *>("sortCriterion");
        QTest::addColumn<Qt3DRender::QSortCriterion::SortType>("sortType");

        Qt3DRender::QSortCriterion *defaultConstructed = new Qt3DRender::QSortCriterion();
        QTest::newRow("defaultConstructed") << defaultConstructed << Qt3DRender::QSortCriterion::StateChangeCost;

        Qt3DRender::QSortCriterion *backToFrontSort = new Qt3DRender::QSortCriterion();
        backToFrontSort->setSort(Qt3DRender::QSortCriterion::BackToFront);
        QTest::newRow("backToFrontSort") << backToFrontSort << Qt3DRender::QSortCriterion::BackToFront;

        Qt3DRender::QSortCriterion *materialSort = new Qt3DRender::QSortCriterion();
        materialSort->setSort(Qt3DRender::QSortCriterion::Material);
        QTest::newRow("materialSort") << materialSort << Qt3DRender::QSortCriterion::Material;
    }

    void checkCloning()
    {
        // GIVEN
        QFETCH(Qt3DRender::QSortCriterion *, sortCriterion);
        QFETCH(Qt3DRender::QSortCriterion::SortType, sortType);

        // THEN
        QCOMPARE(sortCriterion->sort(), sortType);

        // WHEN
        Qt3DRender::QSortCriterion *clone = static_cast<Qt3DRender::QSortCriterion *>(QNode::clone(sortCriterion));

        // THEN
        QVERIFY(clone != Q_NULLPTR);
        QCOMPARE(sortCriterion->id(), clone->id());
        QCOMPARE(sortCriterion->sort(), clone->sort());

        delete sortCriterion;
        delete clone;
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        QScopedPointer<Qt3DRender::QSortCriterion> sortCriterion(new Qt3DRender::QSortCriterion());
        TestArbiter arbiter(sortCriterion.data());

        // WHEN
        sortCriterion->setSort(Qt3DRender::QSortCriterion::BackToFront);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        Qt3DCore::QScenePropertyChangePtr change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "sort");
        QCOMPARE(change->subjectId(), sortCriterion->id());
        QCOMPARE(change->value().value<Qt3DRender::QSortCriterion::SortType>(), Qt3DRender::QSortCriterion::BackToFront);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();

        // WHEN
        sortCriterion->setSort(Qt3DRender::QSortCriterion::BackToFront);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 0);

        // WHEN
        sortCriterion->setSort(Qt3DRender::QSortCriterion::Material);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.events.size(), 1);
        change = arbiter.events.first().staticCast<Qt3DCore::QScenePropertyChange>();
        QCOMPARE(change->propertyName(), "sort");
        QCOMPARE(change->subjectId(), sortCriterion->id());
        QCOMPARE(change->value().value<Qt3DRender::QSortCriterion::SortType>(), Qt3DRender::QSortCriterion::Material);
        QCOMPARE(change->type(), Qt3DCore::NodeUpdated);

        arbiter.events.clear();
    }

protected:
    Qt3DCore::QNode *doClone() const Q_DECL_OVERRIDE
    {
        return Q_NULLPTR;
    }

};

QTEST_MAIN(tst_QSortCriterion)

#include "tst_qsortcriterion.moc"
