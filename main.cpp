/* g++ main.cpp device_monitor.cpp gstpipe.cpp `pkg-config --libs --cflags libudev gstreamer-1.0 gstreamer-video-1.0` -lpthread*/

#include <gst/gst.h>

#include "gstpipe.h"

#include "device_monitor.h"

#define v4l2pipe " v4l2src ! glimagesink"

int main(int argc, char *argv[])
{
        gst_init(&argc, &argv);

        GstPipe pipe(v4l2pipe);
        DeviceMonitor dm(DeviceMonitor::k_cam_system, pipe);
        dm.monitor();
        return 0;
}
