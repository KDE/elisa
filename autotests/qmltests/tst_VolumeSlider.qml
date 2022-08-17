import QtQuick 2.3
import QtTest 1.0
import "../../src/qml"

Item {
    VolumeSlider {
        id: slider
        readonly property int middleValue: (slider.to - slider.from) / 2
    }

    TestCase {
        name: "Clicking, dragging, wheel test"
        when: windowShown

        function init() {
            mouseClick(slider, 0, 0) // set slider to leftmost part
        }

        function test_click() {
            mouseClick(slider)
            verify(slider.value == slider.middleValue, "Clicking in the middle of the slider should set its value to the middle");
        }

        function test_click_drag() {
            mouseDrag(slider, 0, 0, slider.width, 0) // drag from leftmost side to rightmost side
            verify(slider.value == 100, "Should have slided to the maximum")
            mouseDrag(slider,slider.width,0,-slider.width,0) // drag from rightmost side to leftmost side
            verify(slider.value == 0, "Should have slided to the minimum")
        }

        function test_wheel() { // checking that wheel works
            mouseWheel(slider, 0,0,0, 120) // wheel up one notch
            verify(slider.value == slider.wheelEffect, "The current value of the slider should be equal to the wheel effect")
            mouseWheel(slider, 0,0,0, -120) // wheel down one notch
            verify(slider.value == 0, "The current value of the slider should be back to zero")
        }

    }
}
