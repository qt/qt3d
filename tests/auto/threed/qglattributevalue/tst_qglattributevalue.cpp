/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and Digia.  For licensing terms and
** conditions see http://qt.digia.com/licensing.  For further information
** use the contact form at http://qt.digia.com/contact-us.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Digia gives you certain additional
** rights.  These rights are described in the Digia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** GNU General Public License Usage
** Alternatively, this file may be used under the terms of the GNU
** General Public License version 3.0 as published by the Free Software
** Foundation and appearing in the file LICENSE.GPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU General Public License version 3.0 requirements will be
** met: http://www.gnu.org/copyleft/gpl.html.
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include <QtTest/QtTest>
#include "qglattributevalue.h"
#include "qopenglfunctions.h"

class tst_QGLAttributeValue : public QObject
{
    Q_OBJECT
public:
    tst_QGLAttributeValue() {}
    ~tst_QGLAttributeValue() {}

private slots:
    void defaultValue();
    void explicitValue();
    void fromArray();
    void typeSizes_data();
    void typeSizes();
};

void tst_QGLAttributeValue::defaultValue()
{
    QGLAttributeValue value;
    QVERIFY(value.isNull());
    QVERIFY(value.description(QGL::Normal).isNull());
    QVERIFY(value.description(QGL::Normal).attribute() == QGL::Normal);
    QVERIFY(value.type() == GL_FLOAT);
    QCOMPARE(value.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(value.tupleSize(), 0);
    QCOMPARE(value.stride(), 0);
    QVERIFY(value.data() == 0);
    QCOMPARE(value.count(), 0);
}

void tst_QGLAttributeValue::explicitValue()
{
    unsigned char data[4] = {1, 2, 3, 4};
    QGLAttributeValue value(4, GL_UNSIGNED_BYTE, 20, data, 1);
    QVERIFY(!value.isNull());
    QVERIFY(value.type() == GL_UNSIGNED_BYTE);
    QCOMPARE(value.sizeOfType(), int(sizeof(GLubyte)));
    QCOMPARE(value.tupleSize(), 4);
    QCOMPARE(value.stride(), 20);
    QVERIFY(value.data() == data);
    QCOMPARE(value.count(), 1);
    QVERIFY(!value.description(QGL::Color).isNull());
    QVERIFY(value.description(QGL::Color).attribute() == QGL::Color);

    QGLAttributeValue value2(4, GL_UNSIGNED_BYTE, 20, 23, 1);
    QVERIFY(!value2.isNull());
    QVERIFY(value2.type() == GL_UNSIGNED_BYTE);
    QCOMPARE(value2.sizeOfType(), int(sizeof(GLubyte)));
    QCOMPARE(value2.tupleSize(), 4);
    QCOMPARE(value2.stride(), 20);
    QVERIFY(value2.data() == reinterpret_cast<const void *>(23));
    QCOMPARE(value2.count(), 1);

    QGLAttributeDescription desc(QGL::Color, 4, GL_UNSIGNED_BYTE, 20);
    QGLAttributeValue value3(desc, data, 1);
    QVERIFY(!value3.isNull());
    QVERIFY(value3.type() == GL_UNSIGNED_BYTE);
    QCOMPARE(value3.sizeOfType(), int(sizeof(GLubyte)));
    QCOMPARE(value3.tupleSize(), 4);
    QCOMPARE(value3.stride(), 20);
    QVERIFY(value3.data() == data);
    QCOMPARE(value3.count(), 1);

    QGLAttributeValue value4(desc, 23, 1);
    QVERIFY(!value4.isNull());
    QVERIFY(value4.type() == GL_UNSIGNED_BYTE);
    QCOMPARE(value4.sizeOfType(), int(sizeof(GLubyte)));
    QCOMPARE(value4.tupleSize(), 4);
    QCOMPARE(value4.stride(), 20);
    QVERIFY(value4.data() == reinterpret_cast<const void *>(23));
    QCOMPARE(value4.count(), 1);
}

void tst_QGLAttributeValue::fromArray()
{
    QArray<float> floatArray;
    floatArray += 1.0f;
    floatArray += 2.0f;
    floatArray += 3.0f;
    floatArray += 4.0f;
    QGLAttributeValue valueFloat(floatArray);
    QVERIFY(!valueFloat.isNull());
    QVERIFY(valueFloat.type() == GL_FLOAT);
    QCOMPARE(valueFloat.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(valueFloat.tupleSize(), 1);
    QCOMPARE(valueFloat.stride(), 0);
    QVERIFY(valueFloat.data() == floatArray.constData());
    QCOMPARE(valueFloat.count(), floatArray.count());

    QCustomDataArray floatCustomArray(floatArray);
    QGLAttributeValue valueFloatCustom(floatCustomArray);
    QVERIFY(!valueFloatCustom.isNull());
    QVERIFY(valueFloatCustom.type() == GL_FLOAT);
    QCOMPARE(valueFloatCustom.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(valueFloatCustom.tupleSize(), 1);
    QCOMPARE(valueFloatCustom.stride(), 0);
    QVERIFY(valueFloatCustom.data() == floatCustomArray.data());
    QCOMPARE(valueFloatCustom.count(), floatCustomArray.count());

    QArray<QVector2D> vec2Array;
    vec2Array += QVector2D(1.0f, 2.0f);
    vec2Array += QVector2D(2.0f, 3.0f);
    QGLAttributeValue valueVec2(vec2Array);
    QVERIFY(!valueVec2.isNull());
    QVERIFY(valueVec2.type() == GL_FLOAT);
    QCOMPARE(valueVec2.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(valueVec2.tupleSize(), 2);
    QCOMPARE(valueVec2.stride(), 0);
    QVERIFY(valueVec2.data() == vec2Array.constData());
    QCOMPARE(valueVec2.count(), vec2Array.count());

    QCustomDataArray vec2CustomArray(vec2Array);
    QGLAttributeValue valueVec2Custom(vec2CustomArray);
    QVERIFY(!valueVec2Custom.isNull());
    QVERIFY(valueVec2Custom.type() == GL_FLOAT);
    QCOMPARE(valueVec2Custom.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(valueVec2Custom.tupleSize(), 2);
    QCOMPARE(valueVec2Custom.stride(), 0);
    QVERIFY(valueVec2Custom.data() == vec2CustomArray.data());
    QCOMPARE(valueVec2Custom.count(), vec2CustomArray.count());

    QArray<QVector3D> vec3Array;
    vec3Array += QVector3D(1.0f, 2.0f, 3.0f);
    vec3Array += QVector3D(2.0f, 3.0f, 4.0f);
    QGLAttributeValue valueVec3(vec3Array);
    QVERIFY(!valueVec3.isNull());
    QVERIFY(valueVec3.type() == GL_FLOAT);
    QCOMPARE(valueVec3.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(valueVec3.tupleSize(), 3);
    QCOMPARE(valueVec3.stride(), 0);
    QVERIFY(valueVec3.data() == vec3Array.constData());
    QCOMPARE(valueVec3.count(), vec3Array.count());

    QCustomDataArray vec3CustomArray(vec3Array);
    QGLAttributeValue valueVec3Custom(vec3CustomArray);
    QVERIFY(!valueVec3Custom.isNull());
    QVERIFY(valueVec3Custom.type() == GL_FLOAT);
    QCOMPARE(valueVec3Custom.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(valueVec3Custom.tupleSize(), 3);
    QCOMPARE(valueVec3Custom.stride(), 0);
    QVERIFY(valueVec3Custom.data() == vec3CustomArray.data());
    QCOMPARE(valueVec3Custom.count(), vec3CustomArray.count());

    QArray<QVector4D> vec4Array;
    vec4Array += QVector4D(1.0f, 2.0f, 3.0f, 4.0f);
    vec4Array += QVector4D(2.0f, 3.0f, 4.0f, 5.0f);
    QGLAttributeValue valueVec4(vec4Array);
    QVERIFY(!valueVec4.isNull());
    QVERIFY(valueVec4.type() == GL_FLOAT);
    QCOMPARE(valueVec4.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(valueVec4.tupleSize(), 4);
    QCOMPARE(valueVec4.stride(), 0);
    QVERIFY(valueVec4.data() == vec4Array.constData());
    QCOMPARE(valueVec4.count(), vec4Array.count());

    QCustomDataArray vec4CustomArray(vec4Array);
    QGLAttributeValue valueVec4Custom(vec4CustomArray);
    QVERIFY(!valueVec4Custom.isNull());
    QVERIFY(valueVec4Custom.type() == GL_FLOAT);
    QCOMPARE(valueVec4Custom.sizeOfType(), int(sizeof(GLfloat)));
    QCOMPARE(valueVec4Custom.tupleSize(), 4);
    QCOMPARE(valueVec4Custom.stride(), 0);
    QVERIFY(valueVec4Custom.data() == vec4CustomArray.data());
    QCOMPARE(valueVec4Custom.count(), vec4CustomArray.count());

    QArray<QColor4ub> colorArray;
    colorArray += QColor4ub(1, 2, 3, 4);
    colorArray += QColor4ub(2, 3, 4, 5);
    QGLAttributeValue valueColor(colorArray);
    QVERIFY(!valueColor.isNull());
    QVERIFY(valueColor.type() == GL_UNSIGNED_BYTE);
    QCOMPARE(valueColor.sizeOfType(), int(sizeof(GLubyte)));
    QCOMPARE(valueColor.tupleSize(), 4);
    QCOMPARE(valueColor.stride(), 0);
    QVERIFY(valueColor.data() == colorArray.constData());
    QCOMPARE(valueColor.count(), colorArray.count());

    QCustomDataArray colorCustomArray(colorArray);
    QGLAttributeValue valueColorCustom(colorCustomArray);
    QVERIFY(!valueColorCustom.isNull());
    QVERIFY(valueColorCustom.type() == GL_UNSIGNED_BYTE);
    QCOMPARE(valueColorCustom.sizeOfType(), int(sizeof(GLubyte)));
    QCOMPARE(valueColorCustom.tupleSize(), 4);
    QCOMPARE(valueColorCustom.stride(), 0);
    QVERIFY(valueColorCustom.data() == colorCustomArray.data());
    QCOMPARE(valueColorCustom.count(), colorCustomArray.count());
}

void tst_QGLAttributeValue::typeSizes_data()
{
    QTest::addColumn<int>("type");
    QTest::addColumn<int>("size");

    QTest::newRow("byte") << int(GL_BYTE) << int(sizeof(GLbyte));
    QTest::newRow("ubyte") << int(GL_UNSIGNED_BYTE) << int(sizeof(GLubyte));
    QTest::newRow("short") << int(GL_SHORT) << int(sizeof(GLshort));
    QTest::newRow("ushort") << int(GL_UNSIGNED_SHORT) << int(sizeof(GLushort));
    QTest::newRow("int") << int(GL_INT) << int(sizeof(GLint));
    QTest::newRow("uint") << int(GL_UNSIGNED_INT) << int(sizeof(GLuint));
    QTest::newRow("float") << int(GL_FLOAT) << int(sizeof(GLfloat));
#if defined(GL_DOUBLE) && !defined(QT_OPENGL_ES)
    QTest::newRow("double") << int(GL_DOUBLE) << int(sizeof(GLdouble));
#endif
    QTest::newRow("non-type") << int(GL_TEXTURE0) << int(0);
}

void tst_QGLAttributeValue::typeSizes()
{
    QFETCH(int, type);
    QFETCH(int, size);

    QGLAttributeValue value(4, GLenum(type), 0, 0);
    QCOMPARE(value.sizeOfType(), size);
}

QTEST_APPLESS_MAIN(tst_QGLAttributeValue)

#include "tst_qglattributevalue.moc"
