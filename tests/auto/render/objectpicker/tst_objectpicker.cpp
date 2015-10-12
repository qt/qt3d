/****************************************************************************
**
** Copyright (C) 2015 Paul Lemire paul.lemire350@gmail.com
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
#include <Qt3DRender/private/objectpicker_p.h>
#include <Qt3DRender/qobjectpicker.h>
#include <Qt3DRender/qattribute.h>
#include <Qt3DCore/qscenepropertychange.h>

class tst_ObjectPicker : public QObject
{
    Q_OBJECT
private Q_SLOTS:

    void checkPeerPropertyMirroring()
    {
        // GIVEN
        Qt3DRender::Render::ObjectPicker objectPicker;
        Qt3DRender::QObjectPicker picker;
        Qt3DRender::QAttribute attr;
        picker.setHoverEnabled(true);
        picker.setPickAttribute(&attr);

        // WHEN
        objectPicker.setPeer(&picker);

        // THEN
        QVERIFY(!objectPicker.peerUuid().isNull());
        QVERIFY(!objectPicker.pickAttributeId().isNull());
        QCOMPARE(objectPicker.hoverEnabled(), true);
        QCOMPARE(objectPicker.isDirty(), true);
    }

    void checkInitialAndCleanedUpState()
    {
        // GIVEN
        Qt3DRender::Render::ObjectPicker objectPicker;

        // THEN
        QVERIFY(objectPicker.peerUuid().isNull());
        QVERIFY(objectPicker.pickAttributeId().isNull());
        QCOMPARE(objectPicker.hoverEnabled(), false);
        QCOMPARE(objectPicker.isDirty(), false);

        // GIVEN
        Qt3DRender::QObjectPicker picker;
        Qt3DRender::QAttribute attr;
        picker.setHoverEnabled(true);
        picker.setPickAttribute(&attr);

        // WHEN
        objectPicker.updateFromPeer(&picker);
        objectPicker.cleanup();

        // THEN
        QVERIFY(objectPicker.peerUuid().isNull());
        QVERIFY(objectPicker.pickAttributeId().isNull());
        QCOMPARE(objectPicker.hoverEnabled(), false);
        QCOMPARE(objectPicker.isDirty(), false);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::Render::ObjectPicker objectPicker;

        QVERIFY(!objectPicker.isDirty());

        // WHEN
        Qt3DCore::QScenePropertyChangePtr updateChange(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(true);
        updateChange->setPropertyName("hoverEnabled");
        objectPicker.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(objectPicker.hoverEnabled(), true);
        QVERIFY(objectPicker.isDirty());

        objectPicker.unsetDirty();
        QVERIFY(!objectPicker.isDirty());

        // WHEN
        Qt3DRender::QAttribute attr;
        updateChange.reset(new Qt3DCore::QScenePropertyChange(Qt3DCore::NodeUpdated, Qt3DCore::QSceneChange::Node, Qt3DCore::QNodeId()));
        updateChange->setValue(QVariant::fromValue(attr.id()));
        updateChange->setPropertyName("pickAttribute");
        objectPicker.sceneChangeEvent(updateChange);

        // THEN
        QCOMPARE(objectPicker.pickAttributeId(), attr.id());
        QVERIFY(objectPicker.isDirty());

        objectPicker.unsetDirty();
        QVERIFY(!objectPicker.isDirty());
    }
};


QTEST_APPLESS_MAIN(tst_ObjectPicker)

#include "tst_objectpicker.moc"
