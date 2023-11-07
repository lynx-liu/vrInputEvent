#include "vrInputEvent.h"
#include <stdio.h>

int main() {
    if (!VRInputEvent::injectTouchEvent(360, 640)) {
        printf("Failed to send touch event.\n");
        return -1;
    }
    return 0;
}
