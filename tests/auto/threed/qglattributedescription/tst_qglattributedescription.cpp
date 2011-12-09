/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
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
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "qglattributedescription.h"
#include "qopenglfunctions.h"

class tst_QGLAttributeDescription : public QObject
{
    Q_OBJECT
public:
    tst_QGLAttributeDescription() {}
    ~tst_QGLAttributeDescription() {}

private slots:
    void defaultValue();
    void explicitValue();
    void modifyValue();
    void typeSizes();
};

void tst_QGLAttributeDescription::defaultValue()
{
    QGLAttributeDescription desc;
    QVERIFY(desc.isNull());
    QVERIFY(desc.attribute() == QGL::Position);
    QVERIFY(desc.type() == GL_FLOAT);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(desc.tupleSize(), 0);
    QCOMPARE(desc.stride(), 0);
}

void tst_QGLAttributeDescription::explicitValue()
{
    QGLAttributeDescription desc(QGL::Color, 4, GL_UNSIGNED_BYTE, 20);
    QVERIFY(!desc.isNull());
    QVERIFY(desc.attribute() == QGL::Color);
    QVERIFY(desc.type() == GL_UNSIGNED_BYTE);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLubyte)));
    QCOMPARE(desc.tupleSize(), 4);
    QCOMPARE(desc.stride(), 20);
}

void tst_QGLAttributeDescription::modifyValue()
{
    QGLAttributeDescription desc;
    desc.setAttribute(QGL::Color);
    desc.setType(GL_UNSIGNED_BYTE);
    desc.setTupleSize(4);
    desc.setStride(20);
    QVERIFY(!desc.isNull());
    QVERIFY(desc.attribute() == QGL::Color);
    QVERIFY(desc.type() == GL_UNSIGNED_BYTE);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLubyte)));
    QCOMPARE(desc.tupleSize(), 4);
    QCOMPARE(desc.stride(), 20);
}

void tst_QGLAttributeDescription::typeSizes()
{
    QGLAttributeDescription desc;

    desc.setType(GL_BYTE);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLbyte)));

    desc.setType(GL_UNSIGNED_BYTE);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLubyte)));

    desc.setType(GL_SHORT);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLshort)));

    desc.setType(GL_UNSIGNED_SHORT);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLushort)));

    desc.setType(GL_INT);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLint)));

    desc.setType(GL_UNSIGNED_INT);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLuint)));

    desc.setType(GL_FLOAT);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLfloat)));

#if defined(GL_DOUBLE) && !defined(QT_OPENGL_ES)
    desc.setType(GL_DOUBLE);
    QCOMPARE(desc.sizeOfType(), int(sizeof(GLdouble)));
#endif

    desc.setType(GL_TEXTURE0);  // Something that isn't a GL type.
    QCOMPARE(desc.sizeOfType(), 0);

    QVERIFY(desc.isNull());
    desc.setTupleSize(3);
    QVERIFY(!desc.isNull());
}

QTEST_APPLESS_MAIN(tst_QGLAttributeDescription)

#include "tst_qglattributedescription.moc"
