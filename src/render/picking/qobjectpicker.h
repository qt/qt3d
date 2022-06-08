// Copyright (C) 2015 Klaralvdalens Datakonsult AB (KDAB).
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR LGPL-3.0-only OR GPL-2.0-only OR GPL-3.0-only

#ifndef QT3DRENDER_QOBJECTPICKER_H
#define QT3DRENDER_QOBJECTPICKER_H

#include <Qt3DCore/qcomponent.h>
#include <Qt3DRender/qt3drender_global.h>

QT_BEGIN_NAMESPACE

namespace Qt3DRender {

class QObjectPickerPrivate;
class QPickEvent;

class Q_3DRENDERSHARED_EXPORT QObjectPicker : public Qt3DCore::QComponent
{
    Q_OBJECT
    Q_PROPERTY(bool hoverEnabled READ isHoverEnabled WRITE setHoverEnabled NOTIFY hoverEnabledChanged)
    Q_PROPERTY(bool dragEnabled READ isDragEnabled WRITE setDragEnabled NOTIFY dragEnabledChanged)
    Q_PROPERTY(bool pressed READ isPressed NOTIFY pressedChanged)
    Q_PROPERTY(bool containsMouse READ containsMouse NOTIFY containsMouseChanged)
    Q_PROPERTY(int priority READ priority WRITE setPriority NOTIFY priorityChanged REVISION 13)

public:
    explicit QObjectPicker(QNode *parent = nullptr);
    ~QObjectPicker();

    bool isHoverEnabled() const;
    bool isDragEnabled() const;

    bool containsMouse() const;
    bool isPressed() const;

    int priority() const;

public Q_SLOTS:
    void setHoverEnabled(bool hoverEnabled);
    void setDragEnabled(bool dragEnabled);
    Q_REVISION(13) void setPriority(int priority);

Q_SIGNALS:
    void pressed(Qt3DRender::QPickEvent *pick);
    void released(Qt3DRender::QPickEvent *pick);
    void clicked(Qt3DRender::QPickEvent *pick);
    void moved(Qt3DRender::QPickEvent *pick);
    void entered();
    void exited();
    void hoverEnabledChanged(bool hoverEnabled);
    void dragEnabledChanged(bool dragEnabled);
    void pressedChanged(bool pressed);
    void containsMouseChanged(bool containsMouse);
    Q_REVISION(13) void priorityChanged(int priority);

private:
    Q_DECLARE_PRIVATE(QObjectPicker)
};

} // Qt3D

QT_END_NAMESPACE

#endif // QT3DRENDER_QOBJECTPICKER_H
