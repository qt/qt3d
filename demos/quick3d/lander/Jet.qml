import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Item3D {
    // We want to scale the flame, but keep the top constant so it
    // looks like it's attached to the lander's foot, so we use a
    // container Item3D to translate the flame's origin

    // This magic number lines up the jet with the bottom of the lander model
    y: -0.97 - 0.25 + 0.5
    id: jet

    property real scaleFactor: 1.0
    property real internalScaleFactor: 1.0

    Behavior on scaleFactor { NumberAnimation { duration: 200 }}

    transform: Scale3D { scale: jet.scaleFactor * internalScaleFactor * 0.5 }

    SequentialAnimation {
        running:  true
        loops: Animation.Infinite
        NumberAnimation { target: jet;property: "internalScaleFactor";
            from: 1.0; to: 0.8; duration: 100 }
        PauseAnimation { duration: 100 }
        NumberAnimation { target: jet; property: "internalScaleFactor";
            from: 0.8; to: 1.0; duration: 100 }
        PauseAnimation { duration: 100 }
    }

    Quad {
        y: -1.0
        pretransform: Rotation3D { axis: Qt.vector3d(1, 0, 0); angle: 90 }
        effect: flame
    }

    ShaderProgram {
        id: flame
        blending: true
        texture: "flame.png"
        property real interpolationFactor : 1.0

        SequentialAnimation on interpolationFactor {
            running: true
            loops: Animation.Infinite
            NumberAnimation { to : 1.0; duration: 150; }
            NumberAnimation { to : 0.0; duration: 150; }
        }

        vertexShader: "
        attribute highp vec4 qt_Vertex;
        attribute highp vec4 qt_MultiTexCoord0;
        uniform mediump mat4 qt_ModelViewProjectionMatrix;
        varying highp vec4 texCoord;

        void main(void)
        {
            gl_Position = qt_ModelViewProjectionMatrix * qt_Vertex;
            texCoord = qt_MultiTexCoord0;
        }
        "
        fragmentShader: "
        varying highp vec4 texCoord;
        uniform sampler2D qt_Texture0;
        uniform sampler2D texture2;
        uniform mediump float interpolationFactor;

        void main(void)
        {
            mediump vec4 col1 = texture2D(qt_Texture0, texCoord.st);
            mediump vec4 col2 = texture2D(texture2, texCoord.st);
            gl_FragColor = mix(col1, col2, interpolationFactor);
        }
        "
    }
}
