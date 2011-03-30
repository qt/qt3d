import QtQuick 1.0
import Qt3D 1.0
import Qt3D.Shapes 1.0

Viewport {
    height: 480
    width: 640
    //camera: Camera { eye: Qt.vector3d(-3, -20, 2) }
    Item3D {
        mesh: Mesh { source: "lunar-lander.3ds"; options: "ForceFaceted" }
        effect: Effect {
            color: "#aaca00"
            texture: "rusty.png"
            decal: true
        }
    }
    Quad {
        y: -0.8
        transform: [
            Rotation3D { axis: Qt.vector3d(1, 0, 0); angle: 90 },
            Scale3D { id: scaleFactor; scale: 1.0 }]
        effect: flame
        SequentialAnimation {
            running:  true
            loops: Animation.Infinite
            NumberAnimation { target: scaleFactor; property: "scale"; from: 1.0; to: 0.8; duration: 100 }
            PauseAnimation { duration: 100 }
            NumberAnimation { target: scaleFactor; property: "scale"; from: 0.8; to: 1.0; duration: 100 }
        }
    }
    ShaderProgram {
        id: flame
        texture: "flame.png"
        property variant texture2 : "flame2.png"
        property real interpolationFactor : 1.0

        SequentialAnimation on interpolationFactor {
            running: true
            loops: Animation.Infinite
            NumberAnimation { to : 1.0; duration: 750; }


            PauseAnimation { duration: 550 }
            NumberAnimation { to : 0.0; duration: 750; }
            PauseAnimation { duration: 550 }
        }

        SequentialAnimation on color{
            running: true
            loops: Animation.Infinite
            ColorAnimation {
                from: "#aaca00"
                to: "#0033ca"
                duration: 500
            }
            ColorAnimation {
                from: "#0033ca"
                to: "#aaca00"
                duration: 500
            }
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
            uniform mediump vec4 qt_Color;
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
