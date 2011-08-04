/****************************************************************************
**
** Copyright (C) 2011 Nokia Corporation and/or its subsidiary(-ies).
** All rights reserved.
** Contact: Nokia Corporation (qt-info@nokia.com)
**
** This file is part of the QtQuick3D module of the Qt Toolkit.
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

#include "qglcamera.h"
#include "qglpainter.h"
#include <QtGui/qquaternion.h>
#include <QtCore/qmath.h>

QT_BEGIN_NAMESPACE

/*!
    \class QGLCamera
    \brief The QGLCamera class defines the projection to apply to simulate a camera's position, orientation, and optics.
    \since 4.8
    \ingroup qt3d
    \ingroup qt3d::viewing

    \section1 Modelview and projection transformations

    A QGLCamera instance is applied to the scene in two phases:
    modelview transformation and projection transformation.

    During the modelview transformation, the eye(), center(), and
    upVector() are used to generate a 4x4 transformation matrix that
    reflects the viewer's current position and orientation.

    During the projection transformation, the projectionType(),
    nearPlane(), farPlane(), fieldOfView(), and viewSize() are used
    to define a viewing volume as a 4x4 transformation matrix.

    The modelview transformation matrix is returned by modelViewMatrix().
    The projection transformation matrix is returned by projectionMatrix().

    \section1 Positioning and orienting the view

    The viewer position and orientation are defined by eye(), center(),
    and upVector().  The location of the viewer in world co-ordinates is
    given by eye(), the viewer is looking at the object of interest located
    at center(), and the upVector() specifies the direction that should
    be considered "up" with respect to the viewer.

    The vector from the eye() to the center() is called the "view vector",
    and the cross-product of the view vector and upVector() is called
    the "side vector".  The view vector specifies the direction the
    viewer is looking, and the side vector points off to the right of
    the viewer.

    It is recommended that the view vector and upVector() be at right angles
    to each other, but this is not required as long as the angle between
    them is close to 90 degrees.

    The most common use of view and up vectors that are not at right angles
    is to simulate a human eye at a specific height above the ground looking
    down at a lower object or up at a higher object.  In this case, the
    the view vector will not be true horizontal, but the upVector() indicating
    the human's upright stance will be true vertical.

    \section1 Zooming the camera image

    There are two ways to zoom the image seen through the camera: either
    the camera eye() position can be moved closer to the object of interest,
    or the field of view of the camera lens can be changed to make it appear
    as though the object is moving closer.

    Changing the eye() position changes the lighting calculation in the
    scene because the viewer is in a different position, changing the
    angle of light reflection on the object's surface.

    The setFieldOfView() function can be used to simulate the effect of a
    camera lens.  The smaller the fieldOfView(), the closer the object
    will appear.  The lighting calculation will be the same as for the
    unzoomed scene.

    If fieldOfView() is zero, then a standard perspective frustum of
    viewSize() is used to define the viewing volume.  The viewSize()
    can be adjusted with setViewSize() to zoom the view.  A smaller
    viewSize() will make the the object appear closer.

    The fieldOfView() or viewSize() is applied as part of the
    projectionMatrix().

    \section1 Rotating the viewer or object of interest

    Rotating a viewer in 3D space is a very delicate process.  It is very
    easy to construct the rotation incorrectly and end up in a "gimbal lock"
    state where further rotations are impossible in certain directions.

    To help alleviate this problem, QGLCamera uses a quaternion-based
    approach to generate rotations.  A quaternion is a compact representation
    of a rotation in 3D space.  Rotations can be combined through quaternion
    multiplication.  More information on quaternions can be found in the
    documentation for QQuaternion.

    Before rotating the view, you should first decide the type
    of rotation you want to perform:

    \list
    \i Tilting or panning a fixed eye to reveal the scene in different
       directions and orientations.  This is equivalent to mounting a camera
       on a fixed tripod and then adjusting the direction of view and
       orientation with the tripod controls.
    \i Rotating a moving viewer about the object of interest.  This is
       equivalent to moving the viewer around the object at a fixed distance,
       but with the viewer always pointing at the object.
    \endlist

    In the QGLCamera class, the first type of rotation is performed with
    rotateEye() and the second with rotateCenter().  Each of these functions
    take a quaternion argument that defines the type of rotation to perform.

    The tilt(), pan(), and roll() functions return values that can help with
    constructing the rotation quaternions to pass to rotateEye() and
    rotateCenter().  Tilt and pan are also known as "pitch" and "yaw" in
    flight dynamics.

    Three axes of rotation are used to compute the quaternions.  The tilt()
    quaternion is computed with respect to the side vector, the pan()
    quaterion is computed with respect to the upVector(), and the roll()
    quaternion is computed with respect to the view vector.

    The following example tilts the direction the eye() is pointing
    by 5 degrees, and then pans by 45 degrees:

    \code
    camera.rotateEye(camera.tilt(5));
    camera.rotateEye(camera.pan(45));
    \endcode

    The next example performs the two rotations in a single fluid step
    (note that the rotation to be performed first is multiplied last):

    \code
    camera.rotateEye(camera.pan(45) * camera.tilt(5));
    \endcode

    These two examples will not produce the same visual result, even though
    it looks like they might.  In the first example, the upVector() is tilted
    before the pan() quaternion is computed.  In the second example, the pan()
    quaternion is computed using the original upVector().

    This difference in behavior is useful in different situations.  Some
    applications may wish to perform all rotations relative to the original
    viewer orientation, and other applications may wish to perform rotations
    relative to the current viewer orientation.  These application types
    correspond to the second and first examples above.

    \section1 Moving the viewer or object of interest

    The simplest way to move the viewer or object of interest is to call
    setEye() or setCenter() respectively and supply a new position in
    world co-ordinates.  However, this can lead to non-intuitive movements
    if the viewer orientation is not aligned with the world co-ordinate axes.

    For example, subtracting 3 from the eye() x co-ordinate will appear to
    move the eye left 3 units if the viewer orientation is aligned with the
    world co-ordinate axes.  But it will not appear to move the eye left 3
    units in any other orientation.

    The translation() function can be used to construct a translation
    vector that is aligned with the viewer's current orientation.
    Movement in the x direction will move along the side vector, movement in
    the y direction will move along upVector(), and movement in the z
    direction will move along the view vector.

    The translation() function is useful when implementing operations such
    as "step left", "jump up", and so on where the movement should be
    interpreted relative to the viewer's current orientation, not the
    world co-ordinate axes,

    In other words, the following two lines of code are not equivalent
    unless the view is oriented with the world co-ordinate axes:

    \code
    camera.translateEye(camera.translation(x, y, z));

    camera.translateEye(QVector3D(x, y, z));
    \endcode

    The following example translates the eye() position while
    keeping the object of interest at the original center():

    \code
    camera.translateEye(camera.translation(x, y, z));
    \endcode

    The following example translates the object of interest at
    center() while keeping the eye() position fixed:

    \code
    camera.translateCenter(camera.translation(x, y, z));
    \endcode

    The following example translates both the eye() and the center()
    by the same amount, which will maintain the original view vector.

    \code
    QVector3D vector = camera.translation(x, y, z);
    camera.translateEye(vector);
    camera.translateCenter(vector);
    \endcode

    It is important that the translation vector for center() be computed
    before eye() is translated if both eye() and center() must move by the
    same amount.  The following code translates center() in the viewer
    orientation after the eye() is translated:

    \code
    camera.translateEye(camera.translation(x, y, z));
    camera.translateCenter(camera.translation(x, y, z));
    \endcode

    Translating both eye() and center() by the same amount can be used
    to simulate sliding a viewer past a scene while always looking in the
    same direction (for example, filming a scene from a moving vehicle).
    An alternative is to fix the viewer and move the scene itself:
    the negation of the translation() vector can be applied to the
    scene's modelview transformation.

    \section1 Motion tracking

    Viewing of 3D scenes can be enhanced if there is some way to track
    the motion of the viewer or the orientation of the display device.

    Applications can use setMotionAdjustment() to alter the position
    of the camera to account for the viewer's motion.  This indicates
    the viewer's position relative to the center of the screen.
    The motionAdjustment() vector is used to determine by how much
    the camera position should be adjusted.  The distance of the viewer
    from the screen is ignored.

    On handheld devices that use accelerometers to determine the
    orientation of the device, the down vector due to gravity
    can be adjusted to serve as a motion tracking vector.

    The output of motion tracking hardware can be very noisy,
    with minor fluctuations due to viewer twitch movements or
    environmental factors.  The application is responsible for
    cleaning up the signal and removing these fluctuations before
    setMotionAdjustment() is called.

    \section1 Stereo projections

    QGLCamera can adjust the camera position for rendering separate left
    and right eye images by setting eyeSeparation() to a non-zero value.
    The eyeSeparation() is in world co-ordinates.

    Objects that are placed at center() will coincide in the left and
    right eye images, establishing the logical center of the stereo
    effect.  Objects that are closer to the eye() will be rendered
    to appear closer in the stereo effect, and objects that are further
    away from eye() than center() will be rendered to appear further away.

    Perspective and Orthographic projections incorporate the
    eyeSeparation() into the modelViewMatrix() by altering the
    eye() position.

    \sa QGLView, QGLPainter
*/

/*!
    \qmlclass Camera QGLCamera
    \brief The Camera item defines the viewing position and projection for a 3D scene.
    \since 4.8
    \ingroup qt3d::qml3d

    Camera instances are defined on a \l Viewport item using the
    Viewport::camera property:

    \code
    import QtQuick 1.0
    import Qt3D 1.0

    Viewport {
        width: 640; height: 480
        camera: Camera {
            eye: Qt.vector3d(-1, 2, 10)
        }
        light: Light {}
        Item3D {
            mesh: Mesh { source: "meshes/teapot.bez" }
            effect: Effect {}
        }
    }
    \endcode

    \section1 Positioning and orienting the view

    The viewer position and orientation are defined by \l eye, \l center,
    and \l upVector.  The location of the viewer in world co-ordinates is
    given by \l eye, the viewer is looking at the object of interest located
    at \l center, and the \l upVector specifies the direction that should
    be considered "up" with respect to the viewer.

    The vector from the \l eye to the \l center is called the "view vector",
    and the cross-product of the view vector and \l upVector is called
    the "side vector".  The view vector specifies the direction the
    viewer is looking, and the side vector points off to the right of
    the viewer.

    It is recommended that the view vector and \l upVector be at right angles
    to each other, but this is not required as long as the angle between
    them is close to 90 degrees.

    The most common use of view and up vectors that are not at right angles
    is to simulate a human eye at a specific height above the ground looking
    down at a lower object or up at a higher object.  In this case, the
    the view vector will not be true horizontal, but the \l upVector
    indicating the human's upright stance will be true vertical.

    \section1 Zooming the camera image

    There are two ways to zoom the image seen through the camera: either
    the camera \l eye position can be moved closer to the object of interest,
    or the field of view of the camera lens can be changed to make it appear
    as though the object is moving closer.

    Changing the \l eye position changes the lighting calculation in the
    scene because the viewer is in a different position, changing the
    angle of light reflection on the object's surface.

    The \l fieldOfView property function can be used to simulate the effect
    of a camera lens.  The smaller the \l fieldOfView, the closer the object
    will appear.  The lighting calculation will be the same as for the
    unzoomed scene.

    If \l fieldOfView is zero, then a standard perspective frustum of
    is used to define the viewing volume based on the width and height
    of the \l Viewport.

    \section1 Stereo projections

    Camera can adjust the camera position for rendering separate left
    and right eye images by setting the \l eyeSeparation property
    to a non-zero value.  The \l eyeSeparation is in world co-ordinates.

    Objects that are placed at \l center will coincide in the left and
    right eye images, establishing the logical center of the stereo
    effect.  Objects that are closer to the \l eye will be rendered
    to appear closer in the stereo effect, and objects that are further
    away from \l eye than \l center will be rendered to appear further away.

    \sa Viewport
*/

class QGLCameraPrivate
{
public:
    QGLCameraPrivate();

    QGLCamera::ProjectionType projectionType;
    qreal fieldOfView;
    qreal nearPlane;
    qreal farPlane;
    QSizeF viewSize;
    QSizeF minViewSize;
    int screenRotation;
    QVector3D eye;
    QVector3D upVector;
    QVector3D center;
    QVector3D viewVector;
    qreal eyeSeparation;
    QVector3D motionAdjustment;
    QQuaternion motionQuaternion;
    bool adjustForAspectRatio;
};

QGLCameraPrivate::QGLCameraPrivate()
    : projectionType(QGLCamera::Perspective),
      fieldOfView(0.0f),
      nearPlane(5.0f),
      farPlane(1000.0f),
      viewSize(2.0f, 2.0f),
      minViewSize(0.0001f, 0.0001f),
      screenRotation(0),
      eye(0.0f, 0.0f, 10.0f),
      upVector(0.0f, 1.0f, 0.0f),
      center(0.0f, 0.0f, 0.0f),
      viewVector(0.0f, 0.0f, -10.0f),
      eyeSeparation(0.0f),
      motionAdjustment(0.0f, 0.0f, 1.0f),
      adjustForAspectRatio(true)
{
}

/*!
    Constructs a QGLCamera with the default properties and
    attaches it to \a parent.
*/
QGLCamera::QGLCamera(QObject *parent)
    : QObject(parent), d_ptr(new QGLCameraPrivate)
{
}

/*!
    Destroys this QGLCamera object.
*/
QGLCamera::~QGLCamera()
{
    delete d_ptr;
}

/*!
    \enum QGLCamera::ProjectionType
    This enum defines the type of view projection to use for a QGLCamera.

    \value Perspective Use a perspective view.
    \value Orthographic Use an orthographic view.
*/

/*!
    \property QGLCamera::projectionType
    \brief the projection type for this camera.  The default is Perspective.
*/

/*!
    \qmlproperty enumeration Camera::projectionType

    The projection type for this camera, which is one of:

    \list
    \o Perspective Use a perspective view.  This is the default.
    \o Orthographic Use an orthographic view.
    \endlist
*/

QGLCamera::ProjectionType QGLCamera::projectionType() const
{
    Q_D(const QGLCamera);
    return d->projectionType;
}

void QGLCamera::setProjectionType(QGLCamera::ProjectionType value)
{
    Q_D(QGLCamera);
    if (d->projectionType != value) {
        d->projectionType = value;
        emit projectionChanged();
    }
}

/*!
    \property QGLCamera::fieldOfView
    \brief the field of view in degrees for a perspective projection.

    The default value is zero, which indicates a standard perspective
    frustum view volume of viewSize() in size.  If the value is not
    zero, then viewSize() is ignored.

    This value is ignored if projectionType() is not Perspective.

    \sa viewSize()
*/

/*!
    \qmlproperty real Camera::fieldOfView
    The field of view in degrees for a perspective projection.

    The default value is zero, which indicates a standard perspective
    frustum view volume.

    This value is ignored if projectionType is not Perspective.

    \sa projectionType
*/

qreal QGLCamera::fieldOfView() const
{
    Q_D(const QGLCamera);
    return d->fieldOfView;
}

void QGLCamera::setFieldOfView(qreal angle)
{
    Q_D(QGLCamera);
    if (d->fieldOfView != angle) {
        d->fieldOfView = angle;
        emit projectionChanged();
    }
}

/*!
    \property QGLCamera::nearPlane
    \brief the distance from the eye to the near clipping plane.
    The default value is 5.

    \sa farPlane()
*/

/*!
    \qmlproperty real Camera::nearPlane
    The distance from the eye to the near clipping plane.
    The default value is 5.

    \sa farPlane
*/

qreal QGLCamera::nearPlane() const
{
    Q_D(const QGLCamera);
    return d->nearPlane;
}

void QGLCamera::setNearPlane(qreal value)
{
    Q_D(QGLCamera);
    if (d->nearPlane != value) {
        d->nearPlane = value;
        emit projectionChanged();
    }
}

/*!
    \property QGLCamera::farPlane
    \brief the distance from the eye to the far clipping plane.
    The default value is 1000.

    \sa nearPlane()
*/

/*!
    \qmlproperty real Camera::farPlane
    The distance from the eye to the far clipping plane.
    The default value is 1000.

    \sa nearPlane
*/

qreal QGLCamera::farPlane() const
{
    Q_D(const QGLCamera);
    return d->farPlane;
}

void QGLCamera::setFarPlane(qreal value)
{
    Q_D(QGLCamera);
    if (d->farPlane != value) {
        d->farPlane = value;
        emit projectionChanged();
    }
}

/*!
    \property QGLCamera::viewSize
    \brief the size of the front of the projection viewing volume.
    The viewing volume is assumed to be centered on the origin.

    The default value is (2, 2), which indicates a viewing volume front
    from (-1, -1) to (1, 1).

    If the width or height of the viewing volume is negative, then the
    co-ordinates will be swapped.  For example, a size of (2, -2) will
    flip the vertical axis upside down for a viewing volume from
    (-1, 1) to (1, -1).

    The view size will be further adjusted by the window's aspect ratio
    when projectionMatrix() is called.  For best results, the width and
    height of the view size should be the same to define an ideal square
    viewing volume, which is then extended to the final viewing volume
    width and height based on the window's aspect ratio.

    \sa projectionMatrix(), minViewSize()
*/
QSizeF QGLCamera::viewSize() const
{
    Q_D(const QGLCamera);
    return d->viewSize;
}

void QGLCamera::setViewSize(const QSizeF& size)
{
    Q_D(QGLCamera);
    QSizeF sz(size);
    if (qAbs(sz.width()) < d->minViewSize.width()) {
        if (sz.width() >= 0.0f)
            sz.setWidth(d->minViewSize.width());
        else
            sz.setWidth(-d->minViewSize.width());
    }
    if (qAbs(sz.height()) < d->minViewSize.height()) {
        if (sz.height() >= 0.0f)
            sz.setHeight(d->minViewSize.height());
        else
            sz.setHeight(-d->minViewSize.height());
    }
    if (d->viewSize != sz) {
        d->viewSize = sz;
        emit projectionChanged();
    }
}

/*!
    \property QGLCamera::minViewSize
    \brief the minimum size of the front of the projection viewing volume.

    The minimum view size is used to clamp viewSize() when zooming
    the camera closer to an object to prevent it "passing through"
    the object and causing the scale factor to become infinite.

    The default value is (0.0001, 0.0001).

    \sa projectionMatrix(), viewSize()
*/
QSizeF QGLCamera::minViewSize() const
{
    Q_D(const QGLCamera);
    return d->minViewSize;
}

void QGLCamera::setMinViewSize(const QSizeF& size)
{
    Q_D(QGLCamera);
    if (d->minViewSize != size) {
        d->minViewSize = size;
        emit projectionChanged();
    }
}

/*!
    \property QGLCamera::screenRotation
    \brief the screen rotation angle in degrees.  The default
    value is 0.  If this value is 90 or 270, then the view
    will be flipped width for height.  The only supported values
    are 0, 90, 180, and 270.  The screen is rotated around the
    positive z axis.

    This setting is intended for simple screen rotations on handheld
    devices that can be held in either portrait or landscape orientations.
    The entire screen image is rotated so that it can be viewed in a
    different device orientation.

    Use rotateEye() or rotateCenter() for more complex rotations
    that are not aligned with 0, 90, 180, or 270 degrees.
*/

int QGLCamera::screenRotation() const
{
    Q_D(const QGLCamera);
    return d->screenRotation;
}

void QGLCamera::setScreenRotation(int angle)
{
    Q_D(QGLCamera);
    if (d->screenRotation != angle) {
        d->screenRotation = angle;
        emit projectionChanged();
    }
}

/*!
    \property QGLCamera::eye
    \brief the position of the viewer's eye.  The default value is (0, 0, 10).

    \sa translateEye(), upVector(), center(), eyeSeparation()
    \sa motionAdjustment()
*/

/*!
    \qmlproperty vector3D Camera::eye
    The position of the viewer's eye.  The default value is (0, 0, 10).

    \sa upVector, center, eyeSeparation
*/
QVector3D QGLCamera::eye() const
{
    Q_D(const QGLCamera);
    return d->eye;
}

void QGLCamera::setEye(const QVector3D& vertex)
{
    Q_D(QGLCamera);
    if (d->eye != vertex) {
        d->eye = vertex;
        d->viewVector = d->center - d->eye;
        emit viewChanged();
    }
}

/*!
    Adjusts the position of the viewer's eye by the components
    (\a x, \a y, \a z), where the components are interpreted relative
    to the viewer's current orientation.  See translation() for more
    information.

    This function is accessible to QML on the Camera item.

    \sa eye(), setEye(), translateCenter()
*/
void QGLCamera::translateEye(qreal x, qreal y, qreal z)
{
    Q_D(QGLCamera);
    d->eye += translation(x, y, z);
    d->viewVector = d->center - d->eye;
    emit viewChanged();
}

/*!
    \property QGLCamera::upVector
    \brief the up vector for the viewer.  The default value is (0, 1, 0).

    \sa eye(), center()
*/

/*!
    \qmlproperty vector3D Camera::upVector
    The up vector for the viewer.  The default value is (0, 1, 0).

    \sa eye, center
*/

QVector3D QGLCamera::upVector() const
{
    Q_D(const QGLCamera);
    return d->upVector;
}

void QGLCamera::setUpVector(const QVector3D& vector)
{
    Q_D(QGLCamera);
    if (d->upVector != vector) {
        d->upVector = vector;
        emit viewChanged();
    }
}

/*!
    \property QGLCamera::center
    \brief the center of the view visible from the viewer's position.
    The default value is (0, 0, 0).

    \sa translateCenter(), eye(), upVector()
*/

/*!
    \qmlproperty vector3D Camera::center
    The center of the view visible from the viewer's position.
    The default value is (0, 0, 0).

    \sa eye, upVector
*/

QVector3D QGLCamera::center() const
{
    Q_D(const QGLCamera);
    return d->center;
}

void QGLCamera::setCenter(const QVector3D& vertex)
{
    Q_D(QGLCamera);
    if (d->center != vertex) {
        d->center = vertex;
        d->viewVector = d->center - d->eye;
        emit viewChanged();
    }
}

/*!
    Adjusts the center of the view by the components (\a x, \a y, \a z),
    where the components are interpreted relative to the viewer's current
    orientation.  See translation() for more information.

    This function is accessible to QML on the Camera item.

    \sa center(), setCenter(), translateEye()
*/
void QGLCamera::translateCenter(qreal x, qreal y, qreal z)
{
    Q_D(QGLCamera);
    d->center += translation(x, y, z);
    d->viewVector = d->center - d->eye;
    emit viewChanged();
}

/*!
    \property QGLCamera::eyeSeparation
    \brief the separation between the eyes when stereo viewing is in use,
    with eye() specifying the mid-point between the eyes.  The default
    value is 0.

    \sa eye()
*/

/*!
    \qmlproperty real Camera::eyeSeparation
    The separation between the eyes when stereo viewing is in use,
    with \l eye property specifying the mid-point between the eyes.
    The default value is 0.

    \sa eye
*/

qreal QGLCamera::eyeSeparation() const
{
    Q_D(const QGLCamera);
    return d->eyeSeparation;
}

void QGLCamera::setEyeSeparation(qreal value)
{
    Q_D(QGLCamera);
    if (d->eyeSeparation != value) {
        d->eyeSeparation = value;
        emit viewChanged();
    }
}

/*!
    \property QGLCamera::motionAdjustment
    \brief the adjustment vector to apply to the eye() for user motion.

    This property is typically used to implement motion tracking.
    It is interpreted as a vector from the center of the screen to the
    current position of the viewer.  The angle between the motion
    adjustment vector and the screen center is used to adjust the
    position of the eye() when modelViewMatrix() is called.

    The default value is (0, 0, 1), which indicates a viewer
    directly in front of the center of the screen.

    The units for the vector are unspecified.  They could be
    meters, centimeters, or the force due to gravity in various
    directions from an accelerometer.  The angle defined
    by the vector is used to perform the adjustment, not its
    magnitude.

    The output of motion tracking hardware can be very noisy,
    with minor fluctuations due to viewer twitch movements or
    environmental factors.  The application is responsible for
    cleaning up the signal and removing these fluctuations before
    altering this property.

    \sa eye(), modelViewMatrix()
*/

QVector3D QGLCamera::motionAdjustment() const
{
    Q_D(const QGLCamera);
    return d->motionAdjustment;
}

void QGLCamera::setMotionAdjustment(const QVector3D& vector)
{
    Q_D(QGLCamera);
    if (d->motionAdjustment != vector) {
        d->motionAdjustment = vector;
        if (vector.x() == 0.0f && vector.y() == 0.0f) {
            // If the vector is centered, then don't perform any rotations.
            d->motionQuaternion = QQuaternion();
        } else {
            // Determine the pan and tilt angles from the vector.
            QVector3D view = -vector.normalized();
            if (view.z() < 0.0f)
                view = -view;
            qreal xangle = asin(view.x()) * 180.0f / M_PI;
            qreal yangle = asin(-view.y()) * 180.0f / M_PI;

            // Construct the pan and tilt quaternions.
            if (qFuzzyIsNull(xangle))
                d->motionQuaternion = tilt(yangle);
            else if (qFuzzyIsNull(yangle))
                d->motionQuaternion = pan(xangle);
            else
                d->motionQuaternion = tilt(yangle) * pan(xangle);
        }
        emit viewChanged();
    }
}

/*!
    \property QGLCamera::adjustForAspectRatio
    \brief the adjustment state of the aspect ratio in the viewing volume.

    By default, QGLCamera adjusts the viewing volume for the aspect
    ratio of the window so that pixels appear square without the
    application needing to adjust viewSize() manually.

    If this property is false, then the aspect ratio adjustment is
    not performed.
*/

/*!
    \qmlproperty bool Camera::adjustForAspectRatio
    The adjustment state of the aspect ratio in the viewing volume.

    By default, the camera adjusts the viewing volume for the aspect
    ratio of the window so that pixels appear square without the
    application needing to adjust the view size manually.

    If this property is false, then the aspect ratio adjustment is
    not performed.

    \sa projectionType
*/

bool QGLCamera::adjustForAspectRatio() const
{
    Q_D(const QGLCamera);
    return d->adjustForAspectRatio;
}

void QGLCamera::setAdjustForAspectRatio(bool value)
{
    Q_D(QGLCamera);
    if (d->adjustForAspectRatio != value) {
        d->adjustForAspectRatio = value;
        emit viewChanged();
    }
}

/*!
    Returns the quaternion corresponding to tilting the view up or
    down by \a angle degrees.  The returned quaternion can be applied to
    the eye() position with rotateEye() or to the center() position with
    rotateCenter().

    \sa pan(), roll(), rotateEye(), rotateCenter()
*/
QQuaternion QGLCamera::tilt(qreal angle) const
{
    Q_D(const QGLCamera);
    QVector3D side = QVector3D::crossProduct(d->viewVector, d->upVector);
    return QQuaternion::fromAxisAndAngle(side, angle);
}

/*!
    Returns the quaternion corresponding to panning the view left or
    right by \a angle degrees.  The returned quaternion can be applied to
    the eye() position with rotateEye() or to the center() position with
    rotateCenter().

    \sa tilt(), roll(), rotateEye(), rotateCenter()
*/
QQuaternion QGLCamera::pan(qreal angle) const
{
    Q_D(const QGLCamera);
    return QQuaternion::fromAxisAndAngle(d->upVector, angle);
}

/*!
    Returns the quaternion corresponding to rolling the view left or
    right by \a angle degrees.  The returned quaternion can be applied to
    the eye() position with rotateEye() or to the center() position with
    rotateCenter().

    \sa tilt(), pan(), rotateEye(), rotateCenter()
*/
QQuaternion QGLCamera::roll(qreal angle) const
{
    Q_D(const QGLCamera);
    return QQuaternion::fromAxisAndAngle(d->viewVector, angle);
}

/*!
    Rotates the orientation of the eye() according to the quaternion \a q.
    The eye() will remain in the same position, but the upVector() and
    center() may be altered by the rotation.

    \sa rotateCenter(), tilt(), pan(), roll()
*/
void QGLCamera::rotateEye(const QQuaternion& q)
{
    Q_D(QGLCamera);
    d->upVector = q.rotatedVector(d->upVector);
    d->viewVector = q.rotatedVector(d->viewVector);
    d->center = d->eye + d->viewVector;
    emit viewChanged();
}

/*!
    Rotates the position and orientation of the eye() around center()
    according to the quaternion \a q.  The center() will remain in the
    same position, but the upVector() and eye() may be altered by
    the rotation.

    \sa rotateEye(), tilt(), pan(), roll()
*/
void QGLCamera::rotateCenter(const QQuaternion& q)
{
    Q_D(QGLCamera);
    d->upVector = q.rotatedVector(d->upVector);
    d->viewVector = q.rotatedVector(d->viewVector);
    d->eye = d->center - d->viewVector;
    emit viewChanged();
}

/*!
    Returns a translation vector that can be used to adjust the eye()
    or center() by \a x units side-ways, \a y units up,
    and \a z units forwards.

    This function is useful when implementing operations such as
    "step left", "jump up", and so on where the movement should be
    interpreted relative to the viewer's current orientation, not the
    world co-ordinate axes.

    The following example moves the eye() 2 units to the right of the
    current eye position while keeping the same center() of interest:

    \code
    camera.setEye(camera.eye() + camera.translation(2, 0, 0));
    \endcode

    \sa translateEye(), translateCenter()
*/
QVector3D QGLCamera::translation(qreal x, qreal y, qreal z) const
{
    Q_D(const QGLCamera);
    QVector3D vector(0.0f, 0.0f, 0.0f);
    if (x != 0.0f)
        vector += QVector3D::normal(d->viewVector, d->upVector) * x;
    if (y != 0.0f)
        vector += d->upVector.normalized() * y;
    if (z != 0.0f)
        vector += d->viewVector.normalized() * z;
    return vector;
}

/*!
    Returns the transformation matrix to apply to the projection matrix
    to present the scene as viewed from the camera position.

    The \a aspectRatio specifies the aspect ratio of the window the
    camera view is being displayed in.  An \a aspectRatio of 1 indicates that
    the window is square.  An \a aspectRatio greater than 1 indicates that
    the window is wider than it is high.  An \a aspectRatio less than 1
    indicates that the window is higher than it is wide.

    \sa modelViewMatrix()
*/
QMatrix4x4 QGLCamera::projectionMatrix(qreal aspectRatio) const
{
    Q_D(const QGLCamera);
    QMatrix4x4 m;
    if (!d->adjustForAspectRatio)
        aspectRatio = 1.0f;
    if (d->screenRotation != 0) {
        m.rotate((qreal)(d->screenRotation), 0.0f, 0.0f, 1.0f);
        if (d->screenRotation == 90 || d->screenRotation == 270) {
            if (aspectRatio != 0.0f)
                aspectRatio = 1.0f / aspectRatio;
        }
    }
    if (d->projectionType == Perspective && d->fieldOfView != 0.0f) {
        m.perspective(d->fieldOfView, aspectRatio,
                      d->nearPlane, d->farPlane);
    } else {
        qreal halfWidth = d->viewSize.width() / 2.0f;
        qreal halfHeight = d->viewSize.height() / 2.0f;
        if (aspectRatio > 1.0f) {
            halfWidth *= aspectRatio;
        } else if (aspectRatio > 0.0f && aspectRatio < 1.0f) {
            halfHeight /= aspectRatio;
        }
        if (d->projectionType == Perspective) {
            m.frustum(-halfWidth, halfWidth, -halfHeight, halfHeight,
                      d->nearPlane, d->farPlane);
        } else {
            m.ortho(-halfWidth, halfWidth, -halfHeight, halfHeight,
                    d->nearPlane, d->farPlane);
        }
    }
    return m;
}

/*!
    Returns the transformation to apply to the modelview matrix
    to present the scene as viewed from the eye position.

    The \a eye parameter is used to adjust the camera's position
    horizontally by half of eyeSeparation() if \a eye is QGL::LeftEye
    or QGL::RightEye.

    \sa projectionMatrix()
*/
QMatrix4x4 QGLCamera::modelViewMatrix(QGL::Eye eye) const
{
    Q_D(const QGLCamera);
    QMatrix4x4 m;
    QVector3D adjust;
    if (eye == QGL::LeftEye)
        adjust = translation(-d->eyeSeparation / 2.0f, 0.0f, 0.0f);
    else if (eye == QGL::RightEye)
        adjust = translation(d->eyeSeparation / 2.0f, 0.0f, 0.0f);
    if (d->motionQuaternion.isIdentity()) {
        m.lookAt(d->eye + adjust, d->center, d->upVector);
    } else {
        QVector3D up = d->motionQuaternion.rotatedVector(d->upVector);
        QVector3D view = d->motionQuaternion.rotatedVector(d->viewVector);
        m.lookAt(d->center - view + adjust, d->center, up);
    }
    return m;
}

/*!
    Maps \a point from viewport co-ordinates to eye co-ordinates.
    The size of the viewport is given by \a viewportSize, and its
    aspect ratio by \a aspectRatio.

    The returned vector will have its x and y components set to the
    position of the point on the near plane, and the z component
    set to -nearPlane().

    This function is used for converting a mouse event's position
    into eye co-ordinates within the current camera view.
*/
QVector3D QGLCamera::mapPoint
    (const QPoint& point, qreal aspectRatio, const QSize& viewportSize) const
{
    Q_D(const QGLCamera);

    // Rotate the co-ordinate system to account for the screen rotation.
    int x = point.x();
    int y = point.y();
    int width = viewportSize.width();
    int height = viewportSize.height();
    if (!d->adjustForAspectRatio)
        aspectRatio = 1.0f;
    if (d->screenRotation == 90) {
        if (aspectRatio != 0.0f)
            aspectRatio = 1.0f / aspectRatio;
        qSwap(x, y);
        qSwap(width, height);
        y = height - 1 - y;
    } else if (d->screenRotation == 180) {
        x = width - 1 - x;
        y = height - 1 - y;
    } else if (d->screenRotation == 270) {
        if (aspectRatio != 0.0f)
            aspectRatio = 1.0f / aspectRatio;
        qSwap(x, y);
        qSwap(width, height);
    }

    // Determine the relative distance from the middle of the screen.
    // After this xrel and yrel are typically between -1.0 and +1.0
    // (unless the point was outside the viewport).  The yrel is
    // flipped upside down to account for the incoming co-ordinate
    // being left-handed, but the world being right-handed.
    qreal xrel, yrel;
    if (width)
        xrel = (((qreal)(x * 2)) - (qreal)width) / (qreal)width;
    else
        xrel = 0.0f;
    if (height)
        yrel = -(((qreal)(y * 2)) - (qreal)height) / (qreal)height;
    else
        yrel = 0.0f;

    // Reverse the projection and return the point in world co-ordinates.
    QMatrix4x4 m = projectionMatrix(aspectRatio);
    QMatrix4x4 invm = m.inverted();
    return invm.map(QVector3D(xrel, yrel, -1.0f));
}

/*!
    \fn void QGLCamera::projectionChanged()

    This signal is emitted when one of projectionType(), fieldOfView(),
    nearPlane(), farPlane(), viewSize(), or screenRotation() changes,
    indicating a modification to the optical properties of the camera
    looking at the scene.

    \sa viewChanged()
*/

/*!
    \fn void QGLCamera::viewChanged()

    This signal is emitted when one of eye(), upVector(), or center()
    changes, indicating a modification to the viewer's position or
    orientation.

    \sa projectionChanged()
*/

/*!
    \enum QGLCamera::RotateOrder
    This enum defines the order to perform a tilt, pan, and roll
    of a QGLCamera eye or center.

    \value TiltPanRoll Tilt, then pan, then roll.
    \value TiltRollPan Tilt, then roll, then pan.
    \value PanTiltRoll Pan, then tilt, then roll.
    \value PanRollTilt Pan, then roll, then tilt.
    \value RollTiltPan Roll, then tilt, then pan.
    \value RollPanTilt Roll, then pan, then tilt.
*/

/*!
    Tilts the center() up or down by \a tiltAngle degrees,
    pans the center() left or right by \a panAngle degrees,
    and rolls the center() left or right by \a rollAngle degrees,
    all in a single fluid movement.  The \a order parameter
    indicates the order in which to perform the rotations.

    This function is accessible to QML on the Camera item.
    It is provided as a convenience for navigation items that
    rotate the center in multiple directions at the same time
    based on mouse movements.

    \sa tiltPanRollEye()
*/
void QGLCamera::tiltPanRollCenter
    (qreal tiltAngle, qreal panAngle, qreal rollAngle,
     QGLCamera::RotateOrder order)
{
    switch (order) {
    case QGLCamera::TiltPanRoll:
        rotateCenter(roll(rollAngle) * pan(panAngle) * tilt(tiltAngle));
        break;
    case QGLCamera::TiltRollPan:
        rotateCenter(pan(panAngle) * roll(rollAngle) * tilt(tiltAngle));
        break;
    case QGLCamera::PanTiltRoll:
        rotateCenter(roll(rollAngle) * tilt(tiltAngle) * pan(panAngle));
        break;
    case QGLCamera::PanRollTilt:
        rotateCenter(tilt(tiltAngle) * roll(rollAngle) * pan(panAngle));
        break;
    case QGLCamera::RollTiltPan:
        rotateCenter(pan(panAngle) * tilt(tiltAngle) * roll(rollAngle));
        break;
    case QGLCamera::RollPanTilt:
        rotateCenter(tilt(tiltAngle) * pan(panAngle) * roll(rollAngle));
        break;
    }
}

/*!
    Tilts the eye() up or down by \a tiltAngle degrees,
    pans the eye() left or right by \a panAngle degrees,
    and rolls the eye() left or right by \a rollAngle degrees,
    all in a single fluid movement.  The \a order parameter
    indicates the order in which to perform the rotations.

    This function is accessible to QML on the Camera item.
    It is provided as a convenience for navigation items that
    rotate the eye in multiple directions at the same time
    based on mouse movements.

    \sa tiltPanRollCenter()
*/
void QGLCamera::tiltPanRollEye
    (qreal tiltAngle, qreal panAngle, qreal rollAngle,
     QGLCamera::RotateOrder order)
{
    switch (order) {
    case QGLCamera::TiltPanRoll:
        rotateEye(roll(rollAngle) * pan(panAngle) * tilt(tiltAngle));
        break;
    case QGLCamera::TiltRollPan:
        rotateEye(pan(panAngle) * roll(rollAngle) * tilt(tiltAngle));
        break;
    case QGLCamera::PanTiltRoll:
        rotateEye(roll(rollAngle) * tilt(tiltAngle) * pan(panAngle));
        break;
    case QGLCamera::PanRollTilt:
        rotateEye(tilt(tiltAngle) * roll(rollAngle) * pan(panAngle));
        break;
    case QGLCamera::RollTiltPan:
        rotateEye(pan(panAngle) * tilt(tiltAngle) * roll(rollAngle));
        break;
    case QGLCamera::RollPanTilt:
        rotateEye(tilt(tiltAngle) * pan(panAngle) * roll(rollAngle));
        break;
    }
}

QDebug operator<<(QDebug dbg, const QGLCamera &cam)
{
    dbg << "QGLCamera";
    if (!cam.objectName().isEmpty())
        dbg << cam.objectName();
    dbg << "\n";
    dbg << "   projection:" << ( cam.projectionType() == QGLCamera::Perspective ?
                                     "Perspective" : "Orthographic" );
    dbg << "-- viewsize:" << cam.viewSize().width() << "x" << cam.viewSize().height() << "\n";
    dbg << "   near-plane:" << cam.nearPlane() << "-- far-plane:" << cam.farPlane();
    dbg << "-- field-of-view:" << cam.fieldOfView() << "\n";
    dbg << "   rotation:" << cam.screenRotation() << " -- motion adjust:" <<
           cam.motionAdjustment() << " -- aspect adjust:" << cam.adjustForAspectRatio() << "\n";
    dbg << "   eye:" << cam.eye() << "-- center:" << cam.center();
    dbg << "-- up:" << cam.upVector() << "\n";
    return dbg;
}

QT_END_NAMESPACE
