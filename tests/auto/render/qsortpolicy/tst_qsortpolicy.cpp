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
#include <Qt3DRender/private/qsortpolicy_p.h>

#include "testarbiter.h"

// We need to call QNode::clone which is protected
// So we sublcass QNode instead of QObject
class tst_QSortPolicy: public Qt3DCore::QNode
{
    Q_OBJECT

private Q_SLOTS:

    void checkSaneDefaults()
    {
        QScopedPointer<Qt3DRender::QSortPolicy> defaultsortPolicy(new Qt3DRender::QSortPolicy);

        QVERIFY(defaultsortPolicy->sortTypes().isEmpty());
    }

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DRender::QSortPolicy> sortPolicy(new Qt3DRender::QSortPolicy());
        arbiter.setArbiterOnNode(sortPolicy.data());

        // WHEN
        const QList<Qt3DRender::QSortPolicy::SortType> sortTypes = { Qt3DRender::QSortPolicy::BackToFront,
                                                                     Qt3DRender::QSortPolicy::Material,
                                                                     Qt3DRender::QSortPolicy::Material,
                                                                     Qt3DRender::QSortPolicy::FrontToBack };
        auto sortTypesInt = QList<int>();
        transformVector(sortTypes, sortTypesInt);
        sortPolicy->setSortTypes(sortTypes);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), sortPolicy.data());

        arbiter.clear();
    }
};

QTEST_MAIN(tst_QSortPolicy)

#include "tst_qsortpolicy.moc"
