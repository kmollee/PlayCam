#pragma once

/**
 * @file device_monitor.h
 * @brief Monitor device changes
 */

#include "gstpipe.h"
#include <string>

struct udev;

class DeviceMonitor {
    public:
        DeviceMonitor(const std::string system, GstPipe &pipe);

        ~DeviceMonitor();

        void dispatch();
        void monitor();

        constexpr static const char *k_drm_system = "drm";
        constexpr static const char *k_usb_system = "usb";
        constexpr static const char *k_cam_system = "video4linux";

    private:
        void enumerateDevices_();
        void monitor_(); // monitor changes

    private:
        struct udev *m_udev;
        struct udev_monitor *m_udev_monitor;

        const std::string m_system; // monitor system
        GstPipe &m_gstpipe;
};
