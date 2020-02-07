/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
** Contact: https://www.qt.io/licensing/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL3 included in the
** packaging of this file. Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: https://www.gnu.org/licenses/lgpl-3.0.html.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 2.0 or (at your option) the GNU General
** Public license version 3 or any later version approved by the KDE Free
** Qt Foundation. The licenses are as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL2 and LICENSE.GPL3
** included in the packaging of this file. Please review the following
** information to ensure the GNU General Public License requirements will
** be met: https://www.gnu.org/licenses/gpl-2.0.html and
** https://www.gnu.org/licenses/gpl-3.0.html.
**
** $QT_END_LICENSE$
**
****************************************************************************/

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DRender/qsetfence.h>
#include <Qt3DRender/private/qsetfence_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/qpropertyupdatedchange.h>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include <Qt3DCore/private/qscene_p.h>
#include "testpostmanarbiter.h"

class MySetFence : public Qt3DRender::QSetFence
{
public:
    using Qt3DRender::QSetFence::sceneChangeEvent;
};

class tst_QSetFence : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void initTestCase()
    {
        qRegisterMetaType<Qt3DRender::QSetFence::HandleType>("HandleType");
    }

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QSetFence setFence;

        // THEN
        QCOMPARE(setFence.handleType(), Qt3DRender::QSetFence::NoHandle);
        QCOMPARE(setFence.handle(), QVariant());
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DCore::QScene scene;
        MySetFence setFence;

        Qt3DCore::QNodePrivate::get(&setFence)->setScene(&scene);

        {
            // WHEN
            QSignalSpy spy(&setFence, SIGNAL(handleTypeChanged(HandleType)));

            // THEN
            QVERIFY(spy.isValid());

            // THEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("handleType");
            valueChange->setValue(QVariant::fromValue(Qt3DRender::QSetFence::OpenGLFenceId));
            setFence.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(setFence.handleType(), Qt3DRender::QSetFence::OpenGLFenceId);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            setFence.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&setFence, SIGNAL(handleChanged(QVariant)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            Qt3DCore::QPropertyUpdatedChangePtr valueChange(new Qt3DCore::QPropertyUpdatedChange(Qt3DCore::QNodeId()));
            valueChange->setPropertyName("handle");
            valueChange->setValue(QVariant(984));
            setFence.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(setFence.handle(),QVariant(984));
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            setFence.sceneChangeEvent(valueChange);

            // THEN
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DRender::QSetFence setFence;


        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&setFence);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QSetFenceData>>(creationChanges.first());

            QCOMPARE(setFence.id(), creationChangeData->subjectId());
            QCOMPARE(setFence.isEnabled(), true);
            QCOMPARE(setFence.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(setFence.metaObject(), creationChangeData->metaObject());
        }

        // WHEN
        setFence.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&setFence);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QSetFenceData>>(creationChanges.first());

            QCOMPARE(setFence.id(), creationChangeData->subjectId());
            QCOMPARE(setFence.isEnabled(), false);
            QCOMPARE(setFence.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(setFence.metaObject(), creationChangeData->metaObject());
        }
    }

};

QTEST_MAIN(tst_QSetFence)

#include "tst_qsetfence.moc"
