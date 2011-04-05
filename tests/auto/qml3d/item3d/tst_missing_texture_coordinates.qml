import Qt 4.7
import Qt3D 1.0

Viewport  {
    width: 1000
    height: 800
    camera: Camera {
        eye.z: 10
    }

    Item3D {
        id: testItem
        mesh: Mesh { source: "square.3ds" }
        effect: Effect { texture: "test_data/test-image.png" }
    }

    // HACK.
    // The test requires that actual drawing happen,
    // and it would be better to verify that directly.
    Timer {
        running: true
        interval: 5
        onTriggered: Qt.quit()
    }

    //  This test is simply testing to see if the item crashes on startup,
    // so this dummy test is probably not even required
    function test_exists()
    {
        verify(testItem.visible);
        Qt.quit();
    }
}
