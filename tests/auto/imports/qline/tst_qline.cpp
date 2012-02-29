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
#include <Qt3DQuick/line.h>

class tst_QLine : public QObject
{
    Q_OBJECT
public:
    tst_QLine() {}
    ~tst_QLine() {}

private slots:
    void test();
};

void tst_QLine::test()
{
    Line line;

    {
        QCOMPARE(line.width(),3.0);
        QCOMPARE(line.vertices().toList().count(),0);
    }

    {
        QSignalSpy spyWidth(&line,SIGNAL(widthChanged()));
        line.setWidth(5.0);
        QCOMPARE(spyWidth.size(), 1);
        QCOMPARE(line.width(),5.0);
        line.setWidth(5.0);
        QCOMPARE(spyWidth.size(), 1);
    }

    {
        QSignalSpy spyVertices(&line,SIGNAL(verticesChanged()));
        QVariantList vertices;
        vertices.append(QVariant(qreal(1.0)));
        vertices.append(QVariant(qreal(2.0)));
        vertices.append(QVariant(qreal(3.0)));
        line.setVertices(vertices);
        QCOMPARE(spyVertices.size(), 1);

        QVariantList readVertices = line.vertices().toList();
        QCOMPARE(readVertices.count(),3);
        QCOMPARE(readVertices.at(0).toReal(),1.0);
        QCOMPARE(readVertices.at(1).toReal(),2.0);
        QCOMPARE(readVertices.at(2).toReal(),3.0);

        line.setVertices(vertices);
        QCOMPARE(spyVertices.size(), 2);
    }

}

QTEST_APPLESS_MAIN(tst_QLine)

#include "tst_qline.moc"
