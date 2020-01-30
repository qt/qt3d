/****************************************************************************
**
** Copyright (C) 2018 Klaralvdalens Datakonsult AB (KDAB).
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

// TODO Remove in Qt6
#include <QtCore/qcompilerdetection.h>
QT_WARNING_DISABLE_DEPRECATED

#include <QtTest/QTest>
#include <Qt3DRender/qtexture.h>
#include <Qt3DRender/private/qtexture_p.h>
#include <QObject>
#include <QSignalSpy>
#include <Qt3DCore/private/qnodecreatedchangegenerator_p.h>
#include <Qt3DCore/qnodecreatedchange.h>
#include "testpostmanarbiter.h"

class tst_QSharedGLTexture : public QObject
{
    Q_OBJECT

private Q_SLOTS:

    void checkDefaultConstruction()
    {
        // GIVEN
        Qt3DRender::QSharedGLTexture glTexture;

        // THEN
        QCOMPARE(glTexture.textureId(), -1);
        QCOMPARE(glTexture.target(), Qt3DRender::QAbstractTexture::TargetAutomatic);
    }

    void checkPropertyChanges()
    {
        // GIVEN
        Qt3DRender::QSharedGLTexture glTexture;

        {
            // WHEN
            QSignalSpy spy(&glTexture, SIGNAL(textureIdChanged(int)));
            const int newValue = 883;
            glTexture.setTextureId(newValue);

            // THEN
            QVERIFY(spy.isValid());
            QCOMPARE(glTexture.textureId(), newValue);
            QCOMPARE(spy.count(), 1);

            // WHEN
            spy.clear();
            glTexture.setTextureId(newValue);

            // THEN
            QCOMPARE(glTexture.textureId(), newValue);
            QCOMPARE(spy.count(), 0);
        }
    }

    void checkCreationData()
    {
        // GIVEN
        Qt3DRender::QSharedGLTexture glTexture;

        glTexture.setTextureId(1200);

        // WHEN
        QVector<Qt3DCore::QNodeCreatedChangeBasePtr> creationChanges;

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&glTexture);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QAbstractTextureData>>(creationChanges.first());
            const Qt3DRender::QAbstractTextureData cloneData = creationChangeData->data;

            QCOMPARE(glTexture.id(), creationChangeData->subjectId());
            QCOMPARE(glTexture.isEnabled(), true);
            QCOMPARE(glTexture.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(glTexture.metaObject(), creationChangeData->metaObject());
            QCOMPARE(cloneData.sharedTextureId, 1200);
        }

        // WHEN
        glTexture.setEnabled(false);

        {
            Qt3DCore::QNodeCreatedChangeGenerator creationChangeGenerator(&glTexture);
            creationChanges = creationChangeGenerator.creationChanges();
        }

        // THEN
        {
            QCOMPARE(creationChanges.size(), 1);

            const auto creationChangeData = qSharedPointerCast<Qt3DCore::QNodeCreatedChange<Qt3DRender::QAbstractTextureData>>(creationChanges.first());
            const Qt3DRender::QAbstractTextureData cloneData = creationChangeData->data;

            QCOMPARE(glTexture.id(), creationChangeData->subjectId());
            QCOMPARE(glTexture.isEnabled(), false);
            QCOMPARE(glTexture.isEnabled(), creationChangeData->isNodeEnabled());
            QCOMPARE(glTexture.metaObject(), creationChangeData->metaObject());
            QCOMPARE(cloneData.sharedTextureId, 1200);
        }
    }

    void checkTextureIdUpdate()
    {
        // GIVEN
        TestArbiter arbiter;
        Qt3DRender::QSharedGLTexture glTexture;
        arbiter.setArbiterOnNode(&glTexture);

        {
            // WHEN
            glTexture.setTextureId(1584);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 1);
            QCOMPARE(arbiter.dirtyNodes.front(), &glTexture);

            arbiter.dirtyNodes.clear();
        }

        {
            // WHEN
            glTexture.setTextureId(1584);
            QCoreApplication::processEvents();

            // THEN
            QCOMPARE(arbiter.events.size(), 0);
            QCOMPARE(arbiter.dirtyNodes.size(), 0);
        }

    }

};

QTEST_MAIN(tst_QSharedGLTexture)

#include "tst_qsharedgltexture.moc"
