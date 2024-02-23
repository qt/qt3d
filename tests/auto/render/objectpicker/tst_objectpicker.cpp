// Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR GPL-3.0-only

#include <QtTest/QTest>
#include <qbackendnodetester.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/qpickevent.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testarbiter.h"
#include "testrenderer.h"

class tst_ObjectPicker : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:

    void checkInitialSync()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::ObjectPicker objectPicker;
        Qt3DRender::QObjectPicker picker;

        // WHEN
        objectPicker.setRenderer(&renderer);
        simulateInitializationSync(&picker, &objectPicker);

        // THEN
        QVERIFY(renderer.dirtyBits() != 0);
    }

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::ObjectPicker objectPicker;
        Qt3DRender::QObjectPicker picker;
        picker.setHoverEnabled(true);
        picker.setPriority(883);

        // WHEN
        objectPicker.setRenderer(&renderer);
        simulateInitializationSync(&picker, &objectPicker);

        // THEN
        QVERIFY(!objectPicker.peerId().isNull());
        QCOMPARE(objectPicker.isHoverEnabled(), true);
        QCOMPARE(objectPicker.priority(), 883);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        TestRenderer renderer;
        Qt3DRender::Render::ObjectPicker objectPicker;

        // THEN
        QVERIFY(objectPicker.peerId().isNull());
        QCOMPARE(objectPicker.isHoverEnabled(), false);
        QCOMPARE(objectPicker.isDragEnabled(), false);
        QCOMPARE(objectPicker.priority(), 0);

        // GIVEN
        Qt3DRender::QObjectPicker picker;
        picker.setHoverEnabled(true);
        picker.setDragEnabled(true);
        picker.setPriority(1584);

        // WHEN
        objectPicker.setRenderer(&renderer);
        simulateInitializationSync(&picker, &objectPicker);
        objectPicker.cleanup();

        // THEN
        QCOMPARE(objectPicker.isHoverEnabled(), false);
        QCOMPARE(objectPicker.isDragEnabled(), false);
        QCOMPARE(objectPicker.priority(), 0);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        TestRenderer renderer;
        {
            Qt3DRender::QObjectPicker picker;
            Qt3DRender::Render::ObjectPicker objectPicker;
            objectPicker.setRenderer(&renderer);
            simulateInitializationSync(&picker, &objectPicker);

            // WHEN
            picker.setHoverEnabled(true);
            objectPicker.syncFromFrontEnd(&picker, false);

            // THEN
            QCOMPARE(objectPicker.isHoverEnabled(), true);
            QVERIFY(renderer.dirtyBits() != 0);
        }
        {
            Qt3DRender::QObjectPicker picker;
            Qt3DRender::Render::ObjectPicker objectPicker;
            objectPicker.setRenderer(&renderer);
            simulateInitializationSync(&picker, &objectPicker);

            // WHEN
            picker.setDragEnabled(true);
            objectPicker.syncFromFrontEnd(&picker, false);


            // THEN
            QCOMPARE(objectPicker.isDragEnabled(), true);
            QVERIFY(renderer.dirtyBits() != 0);
        }
        {
            Qt3DRender::QObjectPicker picker;
            Qt3DRender::Render::ObjectPicker objectPicker;
            objectPicker.setRenderer(&renderer);
            simulateInitializationSync(&picker, &objectPicker);

            // WHEN
            picker.setPriority(15);
            objectPicker.syncFromFrontEnd(&picker, false);

            // THEN
            QCOMPARE(objectPicker.priority(), 15);
            QVERIFY(renderer.dirtyBits() != 0);
        }
    }
};


QTEST_APPLESS_MAIN(tst_ObjectPicker)

#include "tst_objectpicker.moc"
