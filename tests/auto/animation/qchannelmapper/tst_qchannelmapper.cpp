// Copyright (C) 2017 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <Qt3DCore/private/qnode_p.h>
#include <Qt3DCore/private/qscene_p.h>

#include <Qt3DAnimation/qchannelmapper.h>
#include <Qt3DAnimation/private/qchannelmapper_p.h>
#include <Qt3DAnimation/qchannelmapping.h>

#include <testarbiter.h>

class tst_QChannelmapper : public Qt3DAnimation::QChannelMapper
{
    Q_OBJECT
public:
    tst_QChannelmapper()
    {
        qRegisterMetaType<Qt3DCore::QNode *>();
    }

private Q_SLOTS:

    void checkPropertyUpdates()
    {
        // GIVEN
        TestArbiter arbiter;
        QScopedPointer<Qt3DAnimation::QChannelMapper> mapper(new Qt3DAnimation::QChannelMapper);
        arbiter.setArbiterOnNode(mapper.data());

        // WHEN
        mapper->setEnabled(false);

        // THEN
        QCOMPARE(arbiter.dirtyNodes().size(), 1);
        QCOMPARE(arbiter.dirtyNodes().front(), mapper.data());

        arbiter.clear();
    }

    void checkMappingBookkeeping()
    {
        // GIVEN
        QScopedPointer<Qt3DAnimation::QChannelMapper> mapper(new Qt3DAnimation::QChannelMapper);
        {
            // WHEN
            Qt3DAnimation::QChannelMapping mapping;
            mapper->addMapping(&mapping);

            // THEN
            QCOMPARE(mapping.parent(), mapper.data());
            QCOMPARE(mapper->mappings().size(), 1);
        }
        // THEN (Should not crash and output be unset)
        QVERIFY(mapper->mappings().empty());

        {
            // WHEN
            Qt3DAnimation::QChannelMapper someOtherMapper;
            QScopedPointer<Qt3DAnimation::QChannelMapping> mapping(new Qt3DAnimation::QChannelMapping(&someOtherMapper));
            mapper->addMapping(mapping.data());

            // THEN
            QCOMPARE(mapping->parent(), &someOtherMapper);
            QCOMPARE(mapper->mappings().size(), 1);

            // WHEN
            mapper.reset();
            mapping.reset();

            // THEN Should not crash when the output is destroyed (tests for failed removal of destruction helper)
        }
    }
};

QTEST_MAIN(tst_QChannelmapper)

#include "tst_qchannelmapper.moc"
