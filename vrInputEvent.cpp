#include "vrInputEvent.h"
#include <InputDispatcher.h>
#include <binder/IServiceManager.h>

using namespace android;

#define DEVICE_ID_VIRTUAL   0
#define PARCEL_TOKEN_MOTION_EVENT 1
#define PARCEL_TOKEN_KEY_EVENT  2
#define TRANSACTION_injectInputEvent    IBinder::FIRST_CALL_TRANSACTION + 7

enum InjectInputEventMode {
    ASYNC,
    WAIT_FOR_RESULT,
    WAIT_FOR_FINISH
};

static sp<IBinder> inputService = nullptr;

bool VRInputEvent::injectTouchEvent(int x, int y) {
    if(inputService== nullptr) {
        inputService = defaultServiceManager()->getService(String16("input"));
        if (inputService == nullptr) {
            printf("Failed to get the input service.\n");
            return -1;
        }
    }

    MotionEvent event;
    constexpr size_t pointerCount = 1;
    PointerProperties pointerProperties[pointerCount];
    PointerCoords pointerCoords[pointerCount];

    for(size_t i=0;i<pointerCount;i++) {
        pointerProperties[i].clear();
        pointerProperties[i].id = DEVICE_ID_VIRTUAL;
        pointerProperties[i].toolType = AMOTION_EVENT_TOOL_TYPE_FINGER;

        pointerCoords[i].clear();
        pointerCoords[i].setAxisValue(AMOTION_EVENT_AXIS_X, x);
        pointerCoords[i].setAxisValue(AMOTION_EVENT_AXIS_Y, y);
        pointerCoords[i].setAxisValue(AMOTION_EVENT_AXIS_PRESSURE, 1.0);
        pointerCoords[i].setAxisValue(AMOTION_EVENT_AXIS_SIZE, 1.0);
    }

    nsecs_t currentTime = systemTime(SYSTEM_TIME_MONOTONIC);

    event.initialize(DEVICE_ID_VIRTUAL, AINPUT_SOURCE_TOUCHSCREEN, ADISPLAY_ID_DEFAULT, AMOTION_EVENT_ACTION_DOWN, 0, 0,
                     AMOTION_EVENT_EDGE_FLAG_NONE, AMETA_NONE, 0, MotionClassification::NONE,
                     0, 0, 1, 1, currentTime, currentTime,
                     pointerCount, pointerProperties, pointerCoords);

    Parcel parcel;
    parcel.writeInterfaceToken(String16("android.hardware.input.IInputManager"));
    parcel.writeInt32(!0);
    parcel.writeInt32(PARCEL_TOKEN_MOTION_EVENT);
    event.writeToParcel(&parcel);
    parcel.writeInt32(ASYNC);

    status_t status = inputService->transact(TRANSACTION_injectInputEvent, parcel, nullptr, IBinder::FLAG_ONEWAY);
    return status==OK;
}
