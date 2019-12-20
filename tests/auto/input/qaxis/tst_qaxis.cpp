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

#include <Qt3DInput/QAxis>
#include <Qt3DInput/QAnalogAxisInput>
#include <Qt3DInput/private/qaxis_p.h>
#include <testarbiter.h>

class tst_QAxis: public QObject
{
    Q_OBJECT
public:
    tst_QAxis()
    {
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DInput::QAxis> axis(new Qt3DInput::QAxis());
        arbiter.setArbiterOnNode(axis.data());

        // WHEN
        Qt3DInput::QAbstractAxisInput *input = new Qt3DInput::QAnalogAxisInput();
        axis->addInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axis.data());

        arbiter.clear();

        // WHEN
        axis->removeInput(input);
        QCoreApplication::processEvents();

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), axis.data());

        arbiter.clear();
    }

    void checkAxisInputBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DInput::QAxis> axis(new Qt3DInput::QAxis);
        {
            // WHEN
            Qt3DInput::QAnalogAxisInput input;
            axis->addInput(&input);

            // THEN
            QCOMPARE(input.parent(), axis.data());
            QCOMPARE(axis->inputs().size(), 1);
        }
        // THEN (Should not crash and parameter be unset)
        QVERIFY(axis->inputs().empty());

        {
            // WHEN
            Qt3DInput::QAxis someOtherAxis;
            QScopedPointer<Qt3DInput::QAbstractAxisInput> input(new Qt3DInput::QAnalogAxisInput(&someOtherAxis));
            axis->addInput(input.data());

            // THEN
            QCOMPARE(input->parent(), &someOtherAxis);
            QCOMPARE(axis->inputs().size(), 1);

            // WHEN
            axis.reset();
            input.reset();

            // THEN Should not crash when the input is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QAxis)

#include "tst_qaxis.moc"
