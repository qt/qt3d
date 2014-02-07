/****************************************************************************
**
** Copyright (C) 2012 Digia Plc and/or its subsidiary(-ies).
** Contact: http://www.qt-project.org/legal
**
** This file is part of the Qt3D examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** You may use this file under the terms of the BSD license as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
**     of its contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

import Qt3D 2.0

// This is an example of how to implement a custom navigation strategy
// to replace the default strategy implemented by "Viewport".

MouseArea {
    // Set this property to the "Viewport" item to be controlled
    // by this navigator.  By default we assume that it is our parent.
    property variant viewport: parent

    // The navigator fills the entire viewport.
    anchors.fill: viewport

    // Private variables.
    QtObject {
        id: d_ptr
        property bool panning: false
        property int startX: 0
        property int startY: 0
        property variant startEye
        property variant startCenter
        property variant startUpVector
        property variant pressedObject: null
        property variant enteredObject: null
        property bool sawDoubleClick: false
    }

    // Handle mouse press events.
    onPressed: {
        // Determine which object was pressed in the viewport.
        var objectUnderMouse;
        if (!d_ptr.panning && viewport.picking)
            objectUnderMouse = viewport.objectForPoint(mouse.x, mouse.y);
        else
            objectUnderMouse = null;
        if (d_ptr.pressedObject) {
            // We already have a pressed object, so nothing to do.
            mouse.accepted = true;
        } else if (Qt.isQtObject(objectUnderMouse)) {
            // Mouse press on a new object.
            d_ptr.pressedObject = objectUnderMouse;
            d_ptr.enteredObject = null;
            d_ptr.sawDoubleClick = false;
            objectUnderMouse.pressed();
            mouse.accepted = true;
        } else if (viewport.navigation && mouse.button == Qt.LeftButton) {
            // Start panning the view.
            d_ptr.panning = true;
            d_ptr.startX = mouse.x;
            d_ptr.startY = mouse.y;
            d_ptr.startEye = viewport.camera.eye;
            d_ptr.startCenter = viewport.camera.center;
            d_ptr.startUpVector = viewport.camera.upVector;
            mouse.accepted = true;
        }
    }

    // Handle mouse release events.
    onReleased: {
        if (d_ptr.panning && mouse.button == Qt.LeftButton) {
            d_ptr.panning = false;
            mouse.accepted = true;
        }
        if (d_ptr.pressedObject && mouse.button == Qt.LeftButton) {
            // Deliver the release event to the pressed object.
            var objectUnderMouse = viewport.objectForPoint(mouse.x, mouse.y);
            if (objectUnderMouse == d_ptr.pressedObject && !d_ptr.sawDoubleClick) {
                d_ptr.pressedObject.released();
                d_ptr.pressedObject.clicked();
            } else {
                d_ptr.pressedObject.released();
            }
            if (hoverEnabled) {
                if (Qt.isQtObject(objectUnderMouse))
                    d_ptr.enteredObject = objectUnderMouse;
                else
                    d_ptr.enteredObject = null;
                if (d_ptr.enteredObject != d_ptr.pressedObject) {
                    d_ptr.pressedObject.hoverLeave();
                    if (d_ptr.enteredObject)
                        d_ptr.enteredObject.hoverEnter();
                }
            }
            d_ptr.pressedObject = null;
            mouse.accepted = true;
        }
    }

    // Handle mouse double click events.
    onDoubleClicked: {
        if (d_ptr.pressedObject) {
            var objectUnderMouse = viewport.objectForPoint(mouse.x, mouse.y);
            if (objectUnderMouse == d_ptr.pressedObject) {
                d_ptr.pressedObject.doubleClicked();
                d_ptr.sawDoubleClick = true;
                mouse.accepted = true;
            }
        }
    }

    // Handle mouse move events.
    onPositionChanged: {
        if (d_ptr.panning) {
            var deltaX = mouse.x - d_ptr.startX;
            var deltaY = mouse.y - d_ptr.startY;
            var angleAroundY = deltaX * 90 / width;
            var angleAroundX = deltaY * 90 / height;

            viewport.camera.eye = d_ptr.startEye;
            viewport.camera.center = d_ptr.startCenter;
            viewport.camera.upVector = d_ptr.startUpVector;
            viewport.camera.tiltPanRollCenter
                (-angleAroundX, -angleAroundY, 0, "TiltPanRoll");
        } else if (viewport.picking && hoverEnabled) {
            // Handle object enter/leave events when mouse hover is enabled.
            var objectUnderMouse = viewport.objectForPoint(mouse.x, mouse.y);
            if (!d_ptr.pressedObject) {
                if (Qt.isQtObject(objectUnderMouse)) {
                    if (d_ptr.enteredObject != objectUnderMouse) {
                        if (d_ptr.enteredObject)
                            d_ptr.enteredObject.hoverLeave();
                        d_ptr.enteredObject = objectUnderMouse;
                        d_ptr.enteredObject.hoverEnter();
                    }
                } else if (d_ptr.enteredObject) {
                    d_ptr.enteredObject.hoverLeave();
                    d_ptr.enteredObject = null;
                }
            }
        }
    }

    // Handle leave events.
    onExited: {
        if (hoverEnabled && !d_ptr.pressedObject && d_ptr.enteredObject) {
            d_ptr.enteredObject.hoverLeave();
            d_ptr.enteredObject = null;
        }
    }
}
