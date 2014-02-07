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

#include "qvector4darray.h"
#include <QtGui/qmatrix4x4.h>

QT_BEGIN_NAMESPACE

/*!
    \class QVector4DArray
    \brief The QVector4DArray class is a convenience for wrapping a QArray of QVector4D values.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::arrays

    QVector4DArray is used to build an array of 4D vector values
    based on floating-point x, y, and z arguments:

    \code
    QVector4DArray array;
    array.append(1.0f, 2.0f, 3.0f, -4.0f);
    array.append(-1.0f, 2.0f, 3.0f, -4.0f);
    array.append(1.0f, -2.0f, 3.0f, -4.0f);
    \endcode

    This is more convenient and readable than the equivalent with
    QArray:

    \code
    QArray<QVector4D> array;
    array.append(QVector4D(1.0f, 2.0f, 3.0f, -4.0f));
    array.append(QVector4D(-1.0f, 2.0f, 3.0f, -4.0f));
    array.append(QVector4D(1.0f, -2.0f, 3.0f, -4.0f));
    \endcode

    QVector4DArray also has convenience functions for transforming
    the contents of the array with translate(), translated(),
    transform(), and transformed().

    \sa QArray, QVector2DArray, QVector3DArray
*/

/*!
    \fn QVector4DArray::QVector4DArray()

    Constructs an empty array of QVector4D values.
*/

/*!
    \fn QVector4DArray::QVector4DArray(int size, const QVector4D& value)

    Constructs an array of QVector4D values with an initial \a size.
    All elements in the array are initialized to \a value.
*/

/*!
    \fn QVector4DArray::QVector4DArray(const QArray<QVector4D>& other)

    Constructs a copy of \a other.
*/

/*!
    \fn void QVector4DArray::append(float x, float y, float z, float w)

    Appends (\a x, \a y, \a z, \a w) to this array of QVector4D values.
*/

/*!
    Multiplies the elements in this array of QVector4D values by
    the \a scale.

    \sa scaled()
*/
void QVector4DArray::scale(float scale)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector4D *dst = data();
        for (int index = 0; index < size; ++index)
            *dst++ *= scale;
    } else {
        // Create a new array, translate the values, and assign.
        QArray<QVector4D> result;
        int size = count();
        const QVector4D *src = constData();
        QVector4D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = *src++ * scale;
        *this = result;
    }
}

/*!
    Returns a copy of this array of QVector4D values, multiplied
    by the \a scale.

    \sa scale()
*/
QVector4DArray QVector4DArray::scaled(float scale) const
{
    QArray<QVector4D> result;
    int size = count();
    const QVector4D *src = constData();
    QVector4D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = *src++ * scale;
    return result;
}

/*!
    Translates the elements in this array of QVector4D values
    by the components of \a value.

    \sa translated()
*/
void QVector4DArray::translate(const QVector4D& value)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector4D *dst = data();
        for (int index = 0; index < size; ++index)
            *dst++ += value;
    } else {
        // Create a new array, translate the values, and assign.
        QArray<QVector4D> result;
        int size = count();
        const QVector4D *src = constData();
        QVector4D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = *src++ + value;
        *this = result;
    }
}

/*!
    \fn void QVector4DArray::translate(float x, float y, float z, float w);
    \overload

    Translates the elements in this array of QVector4D values
    by (\a x, \a y, \a z, \a w).

    \sa translated()
*/

/*!
    Returns a copy of this array of QVector4D values, translated
    by the components of \a value.

    \sa translate()
*/
QArray<QVector4D> QVector4DArray::translated(const QVector4D& value) const
{
    QArray<QVector4D> result;
    int size = count();
    const QVector4D *src = constData();
    QVector4D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = *src++ + value;
    return result;
}

/*!
    \fn QArray<QVector4D> QVector4DArray::translated(float x, float y, float z, float w) const
    \overload

    Returns a copy of this array of QVector4D values, translated
    by (\a x, \a y, \a z, \a w).

    \sa translate()
*/

/*!
    Transforms the elements in this array of QVector4D values
    by \a matrix.

    \sa transformed()
*/
void QVector4DArray::transform(const QMatrix4x4& matrix)
{
    if (isDetached()) {
        // Modify the array in-place.
        int size = count();
        QVector4D *dst = data();
        for (int index = 0; index < size; ++index) {
            *dst = matrix * *dst;
            ++dst;
        }
    } else {
        // Create a new array, transform the values, and assign.
        QArray<QVector4D> result;
        int size = count();
        const QVector4D *src = constData();
        QVector4D *dst = result.extend(size);
        for (int index = 0; index < size; ++index)
            *dst++ = matrix * *src++;
        *this = result;
    }
}

/*!
    Returns a copy of this array of QVector3D values, transformed
    by \a matrix.

    \sa transform()
*/
QArray<QVector4D> QVector4DArray::transformed(const QMatrix4x4& matrix) const
{
    QArray<QVector4D> result;
    int size = count();
    const QVector4D *src = constData();
    QVector4D *dst = result.extend(size);
    for (int index = 0; index < size; ++index)
        *dst++ = matrix * *src++;
    return result;
}

QT_END_NAMESPACE
