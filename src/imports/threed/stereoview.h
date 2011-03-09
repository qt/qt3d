/****************************************************************************
**
** Copyright (C) 2010 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:LGPL$
** No Commercial Usage
** This file contains pre-release code and may not be distributed.
** You may use this file in accordance with the terms and conditions
** contained in the Technology Preview License Agreement accompanying
** this package.
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
** In addition, as a special exception, Nokia gives you certain additional
** rights.  These rights are described in the Nokia Qt LGPL Exception
** version 1.1, included in the file LGPL_EXCEPTION.txt in this package.
**
** If you have questions regarding the use of this file, please contact
** Nokia at qt-info@nokia.com.
**
**
**
**
**
**
**
**
** $QT_END_LICENSE$
**
****************************************************************************/

#ifndef STEREOVIEW_H
#define STEREOVIEW_H

#include <QtDeclarative/qdeclarativeitem.h>

#include <Qt3D/qglnamespace.h>

QT_BEGIN_HEADER

QT_BEGIN_NAMESPACE

class StereoViewEffect;

class StereoView : public QDeclarativeItem
{
    Q_OBJECT
    Q_ENUMS(Layout)
    Q_PROPERTY(Layout layout READ layout WRITE setLayout NOTIFY layoutChanged)
    Q_PROPERTY(QGraphicsObject *viewport READ viewport NOTIFY viewportChanged)
    Q_PROPERTY(QDeclarativeListProperty<QObject> stereoViewData READ stereoViewData DESIGNABLE false)
    Q_CLASSINFO("DefaultProperty", "stereoViewData")
public:
    StereoView(QDeclarativeItem *parent = 0);
    ~StereoView();

    enum Layout
    {
        Default,
        Hardware,
        RedCyan,
        LeftRight,
        RightLeft,
        TopBottom,
        BottomTop,
        StretchedLeftRight,
        StretchedRightLeft,
        StretchedTopBottom,
        StretchedBottomTop,
        Disabled
    };

    StereoView::Layout layout() const { return m_layout; }
    void setLayout(StereoView::Layout layout);

    QGraphicsObject *viewport() const { return m_viewport; }

    QGL::Eye eye() const { return m_eye; }
    void setEye(QGL::Eye eye) { m_eye = eye; }

    QDeclarativeListProperty<QObject> stereoViewData();

    static StereoView *findView(QDeclarativeItem *item);

    void paint(QPainter *, const QStyleOptionGraphicsItem *, QWidget *);

    qreal aspectRatioAdjustment() const;

Q_SIGNALS:
    void layoutChanged();
    void viewportChanged();

private Q_SLOTS:
    void switchToOpenGL();

protected:
    void geometryChanged(const QRectF &newGeometry, const QRectF &oldGeometry);

private:
    StereoView::Layout m_layout;
    QGL::Eye m_eye;
    QDeclarativeItem *m_viewport;
    StereoViewEffect *m_effect;

    void updateViewportSize();
};

QML_DECLARE_TYPE(StereoView)

QT_END_NAMESPACE

QT_END_HEADER

#endif
