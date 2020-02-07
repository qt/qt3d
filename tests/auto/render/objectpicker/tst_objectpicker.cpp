/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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
#include <qbackendnodetester.h>
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/qpickevent.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DCore/private/qbackendnode_p.h>
#include "testpostmanarbiter.h"
#include "testrenderer.h"

class tst_ObjectPicker : public Qt3DCore::QBackendNodeTester
{
    Q_OBJECT
private Q_SLOTS:

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
