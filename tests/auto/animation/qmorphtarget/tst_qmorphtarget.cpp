// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/qtest.h>
#include <Qt3DAnimation/qmorphtarget.h>
#include <qobject.h>
#include <qsignalspy.h>

class tst_QMorphTarget : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DAnimation::QMorphTarget morphTarget;

        // THEN
        QCOMPARE(morphTarget.attributeNames(), QStringList());
    }

    void testSetAttributes()
    {
        // GIVEN
        Qt3DAnimation::QMorphTarget morphTarget;
        Qt3DCore::QAttribute attribute1;
        Qt3DCore::QAttribute attribute2;

        attribute1.setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
        attribute2.setName(Qt3DCore::QAttribute::defaultNormalAttributeName());

        {
            // WHEN
            morphTarget.addAttribute(&attribute1);

            // THEN
            QStringList names = morphTarget.attributeNames();
            QCOMPARE(names.size(), 1);
            QCOMPARE(names.at(0), Qt3DCore::QAttribute::defaultPositionAttributeName());
        }

        {
            // WHEN
            morphTarget.addAttribute(&attribute2);

            // THEN
            QStringList names = morphTarget.attributeNames();
            QCOMPARE(names.size(), 2);
            QCOMPARE(names.at(1), Qt3DCore::QAttribute::defaultNormalAttributeName());
        }
    }

    void testFromGeometry()
    {
        // GIVEN
        Qt3DCore::QGeometry geometry;
        Qt3DCore::QAttribute *attribute1 = new Qt3DCore::QAttribute;
        Qt3DCore::QAttribute *attribute2 = new Qt3DCore::QAttribute;
        attribute1->setName(Qt3DCore::QAttribute::defaultPositionAttributeName());
        attribute2->setName(Qt3DCore::QAttribute::defaultTextureCoordinateAttributeName());
        geometry.addAttribute(attribute1);
        geometry.addAttribute(attribute2);

        QStringList attributes;
        attributes.push_back(Qt3DCore::QAttribute::defaultPositionAttributeName());

        {
            // WHEN
            QScopedPointer<Qt3DAnimation::QMorphTarget> morphTarget(
                        Qt3DAnimation::QMorphTarget::fromGeometry(&geometry, attributes));

            // THEN
            QStringList names = morphTarget->attributeNames();
            QCOMPARE(names.size(), 1);
            QCOMPARE(names.at(0), Qt3DCore::QAttribute::defaultPositionAttributeName());
        }

        {
            // WHEN
            attributes.push_back(Qt3DCore::QAttribute::defaultTextureCoordinateAttributeName());
            QScopedPointer<Qt3DAnimation::QMorphTarget> morphTarget(
                        Qt3DAnimation::QMorphTarget::fromGeometry(&geometry, attributes));

            // THEN
            QStringList names = morphTarget->attributeNames();
            QCOMPARE(names.size(), 2);
            QCOMPARE(names.at(0), Qt3DCore::QAttribute::defaultPositionAttributeName());
            QCOMPARE(names.at(1), Qt3DCore::QAttribute::defaultTextureCoordinateAttributeName());
        }
    }

};

QTEST_APPLESS_MAIN(tst_QMorphTarget)

#include "tst_qmorphtarget.moc"
