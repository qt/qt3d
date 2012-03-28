/****************************************************************************
**
** Copyright (C) 2012 Nokia Corporation and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General Public
** License version 2.1 as published by the Free Software Foundation and
** appearing in the file LICENSE.LGPL included in the packaging of this
** file. Please review the following information to ensure the GNU Lesser
** General Public License version 2.1 requirements will be met:
** http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights. These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU General
** Public License version 3.0 as published by the Free Software Foundation
** and appearing in the file LICENSE.GPL included in the packaging of this
** file. Please review the following information to ensure the GNU General
** Public License version 3.0 requirements will be met:
** http://www.gnu.org/copyleft/gpl.html.
**
** Other Usage
** Alternatively, this file may be used in accordance with the terms and
** conditions contained in a signed written agreement between you and Nokia.
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include <QImage>
#include "qdeclarativeeffect.h"
#include <qgltexture2d.h>
class TestWidget;

// QDeclarativeEffect.texture2D() is protected, so we need a
// subclass to promote it for testing
class QDeclarativeEffectSubclass : public QDeclarativeEffect
{
public:
    QDeclarativeEffectSubclass(QObject *parent = 0) :
        QDeclarativeEffect(parent)
    {
    }
    virtual ~QDeclarativeEffectSubclass()
    {
    }
    QGLTexture2D *texture2D()
    {
        return QDeclarativeEffect::texture2D();
    }
};

class tst_QDeclarativeEffect : public QObject
{
    Q_OBJECT
public:
    tst_QDeclarativeEffect() {}
    ~tst_QDeclarativeEffect() {}

private slots:
    void initTestCase();

    void testSetTextureImageTextureCleanup();

    void cleanupTestCase();

};

void tst_QDeclarativeEffect::initTestCase()
{
}

void tst_QDeclarativeEffect::testSetTextureImageTextureCleanup()
{
    // Ensure that the QGLTexture2D is deleted when the effect is
    QPointer<QGLTexture2D> testTexturePointer;
    {
        QImage aTestImage;
        QDeclarativeEffectSubclass testParentEffect;
        testParentEffect.setTextureImage(aTestImage);
        testTexturePointer = testParentEffect.texture2D();
        QVERIFY2(testTexturePointer, "Confirm QGLTexture2D exists");
    }

    QVERIFY2(!testTexturePointer, "Confirm QGLTexture2D was deleted");
}


void tst_QDeclarativeEffect::cleanupTestCase()
{
}

QTEST_MAIN(tst_QDeclarativeEffect)

#include "tst_qdeclarativeeffect.moc"
