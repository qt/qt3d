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
#include "qglattributeset.h"

class tst_QGLAttributeSet : public QObject
{
    Q_OBJECT
public:
    tst_QGLAttributeSet() {}
    ~tst_QGLAttributeSet() {}

private slots:
    void defaultValue();
    void clear();
    void insert();
    void remove();
    void list();
    void compare();
    void unite();
    void intersect();
    void subtract();
};

void tst_QGLAttributeSet::defaultValue()
{
    QGLAttributeSet set;
    QVERIFY(set.isEmpty());
    for (int index = -50; index <= 50; ++index)
        QVERIFY(!set.contains(QGL::VertexAttribute(index)));
}

void tst_QGLAttributeSet::clear()
{
    QGLAttributeSet set;
    set.insert(QGL::TextureCoord0);
    QVERIFY(!set.isEmpty());
    QVERIFY(set.contains(QGL::TextureCoord0));
    set.clear();
    QVERIFY(set.isEmpty());
    QVERIFY(!set.contains(QGL::TextureCoord0));
}

void tst_QGLAttributeSet::insert()
{
    QSKIP("Fails on Qt5");

    for (int index = -50; index <= 50; ++index) {
        QGLAttributeSet set;
        set.insert(QGL::VertexAttribute(index));
        if (index < 0 || index > 31)
            QVERIFY(!set.contains(QGL::VertexAttribute(index)));
        else
            QVERIFY(set.contains(QGL::VertexAttribute(index)));
        for (int index2 = -50; index2 <= 50; ++index2) {
            if (index2 != index)
                QVERIFY(!set.contains(QGL::VertexAttribute(index2)));
        }
    }
}

void tst_QGLAttributeSet::remove()
{
    for (int index = -50; index <= 50; ++index) {
        QGLAttributeSet set;
        for (int index2 = 0; index2 < 32; ++index2)
            set.insert(QGL::VertexAttribute(index2));
        set.remove(QGL::VertexAttribute(index));
        for (int index2 = 0; index2 < 32; ++index2) {
            if (index == index2)
                QVERIFY(!set.contains(QGL::VertexAttribute(index2)));
            else
                QVERIFY(set.contains(QGL::VertexAttribute(index2)));
        }
    }
}

void tst_QGLAttributeSet::list()
{
    QList<QGL::VertexAttribute> list;
    QVERIFY(QGLAttributeSet::fromList(list).isEmpty());

    list.append(QGL::VertexAttribute(-50));
    list.append(QGL::TextureCoord1);
    list.append(QGL::VertexAttribute(50));

    QGLAttributeSet set1 = QGLAttributeSet::fromList(list);
    QGLAttributeSet set2;
    set2.insert(QGL::TextureCoord1);
    QVERIFY(set1 == set2);

    QList<QGL::VertexAttribute> list2 = set1.toList();
    QCOMPARE(list2.count(), 1);
    QVERIFY(list2[0] == QGL::TextureCoord1);
}

void tst_QGLAttributeSet::compare()
{
    QGLAttributeSet set1;
    QGLAttributeSet set2;

    QVERIFY(set1 == set2);
    QVERIFY(!(set1 != set2));

    set1.insert(QGL::Position);
    QVERIFY(set1 != set2);
    QVERIFY(!(set1 == set2));

    set2.insert(QGL::Position);
    QVERIFY(set1 == set2);
    QVERIFY(!(set1 != set2));

    set2.insert(QGL::TextureCoord0);
    QVERIFY(set1 != set2);
    QVERIFY(!(set1 == set2));
}

void tst_QGLAttributeSet::unite()
{
    QGLAttributeSet set1;
    QGLAttributeSet set2;

    set1.insert(QGL::Position);
    set1.insert(QGL::Color);

    set2.insert(QGL::Position);
    set2.insert(QGL::TextureCoord0);

    set1.unite(set2);
    QVERIFY(set1.contains(QGL::Position));
    QVERIFY(set1.contains(QGL::Color));
    QVERIFY(set1.contains(QGL::TextureCoord0));
    QVERIFY(!set1.contains(QGL::Normal));
}

void tst_QGLAttributeSet::intersect()
{
    QGLAttributeSet set1;
    QGLAttributeSet set2;

    set1.insert(QGL::Position);
    set1.insert(QGL::Color);

    set2.insert(QGL::Position);
    set2.insert(QGL::TextureCoord0);

    set1.intersect(set2);
    QVERIFY(set1.contains(QGL::Position));
    QVERIFY(!set1.contains(QGL::Color));
    QVERIFY(!set1.contains(QGL::TextureCoord0));
    QVERIFY(!set1.contains(QGL::Normal));
}

void tst_QGLAttributeSet::subtract()
{
    QGLAttributeSet set1;
    QGLAttributeSet set2;

    set1.insert(QGL::Position);
    set1.insert(QGL::Color);

    set2.insert(QGL::Position);
    set2.insert(QGL::TextureCoord0);

    set1.subtract(set2);
    QVERIFY(!set1.contains(QGL::Position));
    QVERIFY(set1.contains(QGL::Color));
    QVERIFY(!set1.contains(QGL::TextureCoord0));
    QVERIFY(!set1.contains(QGL::Normal));
}

QTEST_APPLESS_MAIN(tst_QGLAttributeSet)

#include "tst_qglattributeset.moc"
