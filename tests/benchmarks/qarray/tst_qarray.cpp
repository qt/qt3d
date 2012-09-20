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
#include <QtGui/qvector3d.h>
#include <QtCore/qvarlengtharray.h>
#include <QtCore/qvector.h>
#include <QtCore/qlist.h>
#include "qarray.h"
#include <vector>

//#define TEST_QLIST 1

class tst_QArray : public QObject
{
    Q_OBJECT
public:
    tst_QArray() {}
    virtual ~tst_QArray() {}

private slots:
    void append_data();
    void append();
    void appendReserved_data();
    void appendReserved();
    void appendVector3D_data();
    void appendVector3D();
    void appendSmall_data();
    void appendSmall();
    void appendFourAtATime_data();
    void appendFourAtATime();
    void clear_data();
    void clear();
    void randomAccess_data();
    void randomAccess();
};

enum {
    Test_Vector,
    Test_List,
    Test_VarLengthArray,
    Test_Array,
    Test_STLVector
};

void tst_QArray::append_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 0; size < 1024; size += 12) {
        name = "QVector--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Vector);

#if TEST_QLIST
        name = "QList--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_List);
#endif

        name = "QVarLengthArray--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_VarLengthArray);

        name = "QArray--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Array);

        name = "std::vector--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_STLVector);
    }
}

void tst_QArray::append()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_Vector) {
        QVector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_List) {
        QList<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_Array) {
        QArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
    }
}

void tst_QArray::appendReserved_data()
{
    append_data();
}

void tst_QArray::appendReserved()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_Vector) {
        QVector<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_List) {
        QList<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_Array) {
        QArray<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        buffer.reserve(size);
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
    }
}

void tst_QArray::appendVector3D_data()
{
    append_data();
}

void tst_QArray::appendVector3D()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_Vector) {
        QVector<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_List) {
        QList<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_Array) {
        QArray<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(QVector3D(i, i + 1, i + 2));
        }
    } else if (type == Test_STLVector) {
        std::vector<QVector3D> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(QVector3D(i, i + 1, i + 2));
        }
    }
}

void tst_QArray::appendSmall_data()
{
    QTest::addColumn<int>("size");
    QTest::addColumn<int>("type");

    QByteArray name;
    for (int size = 0; size < 16; ++size) {
        name = "QVector--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Vector);

#if TEST_QLIST
        name = "QList--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_List);
#endif

        name = "QVarLengthArray--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_VarLengthArray);

        name = "QArray--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_Array);

        name = "std::vector--";
        name += QByteArray::number(size);
        QTest::newRow(name.constData()) << size << int(Test_STLVector);
    }
}

void tst_QArray::appendSmall()
{
    append();
}

void tst_QArray::appendFourAtATime_data()
{
    append_data();
}

void tst_QArray::appendFourAtATime()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_Vector) {
        QVector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i));
                buffer.append(float(i + 1));
                buffer.append(float(i + 2));
                buffer.append(float(i + 3));
            }
        }
    } else if (type == Test_List) {
        QList<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i));
                buffer.append(float(i + 1));
                buffer.append(float(i + 2));
                buffer.append(float(i + 3));
            }
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i));
                buffer.append(float(i + 1));
                buffer.append(float(i + 2));
                buffer.append(float(i + 3));
            }
        }
    } else if (type == Test_Array) {
        QArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.append(float(i), float(i + 1),
                              float(i + 2), float(i + 3));
            }
        }
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; i += 4) {
                buffer.push_back(float(i));
                buffer.push_back(float(i + 1));
                buffer.push_back(float(i + 2));
                buffer.push_back(float(i + 3));
            }
        }
    }
}

void tst_QArray::clear_data()
{
    append_data();
}

void tst_QArray::clear()
{
    QFETCH(int, size);
    QFETCH(int, type);

    if (type == Test_Vector) {
        QVector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_List) {
        QList<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_Array) {
        QArray<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
            buffer.resize(0);
            for (int i = 0; i < size; ++i)
                buffer.append(float(i));
        }
    } else if (type == Test_STLVector) {
        std::vector<float> buffer;
        QBENCHMARK {
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
            buffer.clear();
            for (int i = 0; i < size; ++i)
                buffer.push_back(float(i));
        }
    }
}

void tst_QArray::randomAccess_data()
{
    QTest::addColumn<int>("type");

    QTest::newRow("QVector") << int(Test_Vector);
#if TEST_QLIST
    QTest::newRow("QList") << int(Test_List);
#endif
    QTest::newRow("QVarLengthArray") << int(Test_VarLengthArray);
    QTest::newRow("QArray") << int(Test_Array);
    QTest::newRow("std::vector") << int(Test_STLVector);
}

// To force the values below to be computed and stored.
static int volatile finalSum;

void tst_QArray::randomAccess()
{
    QFETCH(int, type);

    if (type == Test_Vector) {
        QVector<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer.at(i - 10) + buffer.at(i - 4) * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer.at(i);
            finalSum = sum;
        }
    } else if (type == Test_List) {
        QList<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer[i - 10] + buffer[i - 4] * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer[i];
            finalSum = sum;
        }
    } else if (type == Test_VarLengthArray) {
        QVarLengthArray<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer[i - 10] + buffer[i - 4] * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer[i];
            finalSum = sum;
        }
    } else if (type == Test_Array) {
        QArray<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.append(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer.at(i - 10) + buffer.at(i - 4) * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer.at(i);
            finalSum = sum;
        }
    } else if (type == Test_STLVector) {
        std::vector<int> buffer;
        for (int i = 0; i < 10000; ++i)
            buffer.push_back(i);
        QBENCHMARK {
            for (int i = 10; i < 10000; ++i)
                buffer[i] = buffer[i - 10] + buffer[i - 4] * 2;
            int sum = 0;
            for (int i = 0; i < 10000; ++i)
                sum += buffer[i];
            finalSum = sum;
        }
    }
}

QTEST_MAIN(tst_QArray)

#include "tst_qarray.moc"
