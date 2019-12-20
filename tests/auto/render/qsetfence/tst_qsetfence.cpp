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

#include <QtTest/QTest>
#include <Qt3DRender/qsetfence.h>
#include <Qt3DRender/private/qsetfence_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/private/qscene_p.h>
#include "testarbiter.h"

class MySetFence : public Qt3DRender::QSetFence
{
public:
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
            auto *dNode = static_cast<Qt3DRender::QSetFencePrivate *>(Qt3DRender::QSetFencePrivate::get(&setFence));
            dNode->setHandleType(Qt3DRender::QSetFence::OpenGLFenceId);

            // THEN
            QCOMPARE(setFence.handleType(), Qt3DRender::QSetFence::OpenGLFenceId);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            dNode->setHandleType(Qt3DRender::QSetFence::OpenGLFenceId);

            // THEN
            QCOMPARE(spy.count(), 0);
        }

        {
            // WHEN
            QSignalSpy spy(&setFence, SIGNAL(handleChanged(QVariant)));

            // THEN
            QVERIFY(spy.isValid());

            // WHEN
            auto *dNode = static_cast<Qt3DRender::QSetFencePrivate *>(Qt3DRender::QSetFencePrivate::get(&setFence));
            dNode->setHandle(QVariant(984));

            // THEN
            QCOMPARE(setFence.handle(),QVariant(984));
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            dNode->setHandle(QVariant(984));

            // THEN
            QCOMPARE(spy.count(), 0);
        }
    }
};

QTEST_MAIN(tst_QSetFence)

#include "tst_qsetfence.moc"
