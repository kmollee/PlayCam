#include <libudev.h>
#include <stdio.h>
#include <string.h>

#include <thread>

#include "device_monitor.h"

static void print_device(struct udev_device *dev)
{
        const char *action = udev_device_get_action(dev);
        if (!action)
                action = "exists";

        const char *vendor = udev_device_get_sysattr_value(dev, "idVendor");
        if (!vendor)
                vendor = "0000";

        const char *product = udev_device_get_sysattr_value(dev, "idProduct");
        if (!product)
                product = "0000";

        printf("%s %s %6s %s:%s %s", udev_device_get_subsystem(dev),
               udev_device_get_devtype(dev), action, vendor, product,
               udev_device_get_devnode(dev));
}

static void process_device(struct udev_device *dev)
{
        if (dev) {
                if (udev_device_get_devnode(dev))
                        print_device(dev);

                udev_device_unref(dev);
        }
}

DeviceMonitor::DeviceMonitor(const std::string system, GstPipe &pipe)
        : m_system(system), m_gstpipe(pipe)
{
        m_udev = udev_new();
        enumerateDevices_();
}

void DeviceMonitor::enumerateDevices_()
{
        struct udev_enumerate *enumerate = udev_enumerate_new(m_udev);

        udev_enumerate_add_match_subsystem(enumerate, m_system.c_str());
        printf("using system:%s\n", m_system.c_str());
        udev_enumerate_scan_devices(enumerate);

        struct udev_list_entry *devices =
                udev_enumerate_get_list_entry(enumerate);
        struct udev_list_entry *entry;

        udev_list_entry_foreach(entry, devices)
        {
                const char *path = udev_list_entry_get_name(entry);
                struct udev_device *dev =
                        udev_device_new_from_syspath(m_udev, path);
                process_device(dev);
        }

        udev_enumerate_unref(enumerate);
}

DeviceMonitor::~DeviceMonitor()
{
        udev_monitor_unref(m_udev_monitor);
        udev_unref(m_udev);
}

void DeviceMonitor::dispatch()
{
        std::thread monitor(&DeviceMonitor::monitor_, this);
        monitor.detach();
}

void DeviceMonitor::monitor_()
{
        m_udev_monitor = udev_monitor_new_from_netlink(m_udev, "udev");

        udev_monitor_filter_add_match_subsystem_devtype(m_udev_monitor,
                                                        m_system.c_str(), NULL);
        udev_monitor_enable_receiving(m_udev_monitor);

        int fd = udev_monitor_get_fd(m_udev_monitor);

        while (1) {
                fd_set fds;
                FD_ZERO(&fds);
                FD_SET(fd, &fds);

                if (select(fd + 1, &fds, nullptr, nullptr, nullptr) == -1) {
                        perror("select");
                        exit(4);
                }

                if (FD_ISSET(fd, &fds)) {
                        struct udev_device *dev =
                                udev_monitor_receive_device(m_udev_monitor);
                        if (strcmp(udev_device_get_action(dev), "remove") ==
                            0) {
                                printf("\nrecive remove\n");
                                if (m_gstpipe.stop() != 0) {
                                        fprintf(stderr,
                                                "could not stop pipeline");
                                        exit(1);
                                }
                        } else if (strcmp(udev_device_get_action(dev), "add") ==
                                   0) {
                                printf("\nrecive add\n");
                                if (m_gstpipe.play() != 0) {
                                        fprintf(stderr,
                                                "could not play pipeline");
                                        exit(1);
                                }
                                // print_device(dev);
                        }
                        udev_device_unref(dev);
                }
        }
}

void DeviceMonitor::monitor()
{
        monitor_();
}
