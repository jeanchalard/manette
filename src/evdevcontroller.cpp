// SPDX-FileCopyrightText: 2025 J <jean.chalard@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#include <QDebug>
#include <fcntl.h>
#include <iostream>
#include <linux/input.h>
#include <sys/ioctl.h>
#include <systemd/sd-device.h>
#include <unistd.h>

#include "evdevcontroller.h"

using std::cout;
using std::endl;
using std::string;

static const char VENDOR_ID[] = "0583";
static const char PRODUCT_ID[] = "2060";

static int openFd(const QString &path)
{
    const int fd = ::open(path.toUtf8().constData(), O_RDONLY | O_NONBLOCK);
    if (fd < 0)
        cout << "Can't open device " << path.toStdString() << " : " << std::strerror(errno) << endl;
    return fd;
}

static QString findDevice(const char vendor_id[4], const char product_id[4])
{
    sd_device_enumerator *enumerator = nullptr;
    sd_device *device;

    if (0 > sd_device_enumerator_new(&enumerator)) {
        cout << "Can't enumerate ? " << std::strerror(errno) << endl;
        return QString::fromUtf8("");
    }
    if (0 > sd_device_enumerator_add_match_subsystem(enumerator, "input", true)) {
        cout << "Can't add subsystem ? " << std::strerror(errno) << endl;
        return QString::fromUtf8("");
    }
    sd_device_enumerator_add_match_property_required(enumerator, "ID_VENDOR_ID", vendor_id);
    sd_device_enumerator_add_match_property_required(enumerator, "ID_MODEL_ID", product_id);

    const char *devicePath = nullptr;
    for (device = sd_device_enumerator_get_device_first(enumerator); device; device = sd_device_enumerator_get_device_next(enumerator)) {
        const char *path;
        int ret = sd_device_get_devname(device, &path);
        if (0 <= ret && nullptr != path)
            cout << "Found dev : " << path << endl;
        else
            continue;
        if (0 == strncmp(path, "/dev/input/event", strlen("/dev/input/event"))) {
            const QString res = QString::fromUtf8(path);
            sd_device_enumerator_unref(enumerator);
            return res;
        }
        devicePath = path;
    }

    if (!devicePath)
        devicePath = "";
    const QString res = QString::fromUtf8(devicePath);
    sd_device_enumerator_unref(enumerator);
    return res;
}

EvdevController::EvdevController(QObject *parent)
    : QObject(parent)
    , path(findDevice(VENDOR_ID, PRODUCT_ID))
    , fd(openFd(path))
{
    notifier = new QSocketNotifier(fd, QSocketNotifier::Read, this);
    connect(notifier, &QSocketNotifier::activated, this, &EvdevController::readNewEvents);

    // Try to open and set up the device immediately
    char name[256] = "Generic Gamepad";
    if (ioctl(fd, EVIOCGNAME(sizeof(name)), name) >= 0)
        Q_EMIT(deviceConnected(QString::fromUtf8(name)));
    else
        Q_EMIT(deviceConnected(QString::fromUtf8("Device: %1").arg(path)));
}

EvdevController::~EvdevController()
{
    if (notifier)
        delete notifier;
    Q_EMIT(deviceDisconnected());
    ::close(fd);
}

const QString EvdevController::devicePath() const
{
    return path;
}

void EvdevController::readNewEvents()
{
    struct input_event ev;

    while (true) {
        const ssize_t bytes_read = ::read(fd, &ev, sizeof(ev));

        if (bytes_read < static_cast<signed long>(sizeof(ev))) {
            if (bytes_read < 0 && errno == EAGAIN)
                break; // expected
            else if (bytes_read < 0)
                cout << "Read returns " << bytes_read << " from event device : " << std::strerror(errno) << endl;
            else if (bytes_read == 0)
                cout << "Controller unplugged ?" << endl;
            else
                cout << "Incomplete read on event device (" << bytes_read << " bytes read, expected " << sizeof(ev) << endl;
            break;
        }
        if (ev.type != EV_KEY && ev.type != EV_ABS)
            continue;
        cout << "EVENT " << " type=" << ev.type << " val=" << ev.value << " code=" << ev.code << endl;

        if (ev.type == EV_KEY) {
            const bool pressed = ev.value == 1;
            switch (ev.code) {
            case B_A:
                Q_EMIT(buttonStateChanged(M_A, pressed));
                break;
            case B_B:
                Q_EMIT(buttonStateChanged(M_B, pressed));
                break;
            case B_X:
                Q_EMIT(buttonStateChanged(M_X, pressed));
                break;
            case B_Y:
                Q_EMIT(buttonStateChanged(M_Y, pressed));
                break;
            case B_L:
                Q_EMIT(buttonStateChanged(M_L, pressed));
                break;
            case B_R:
                Q_EMIT(buttonStateChanged(M_R, pressed));
                break;
            }
        }
        if (ev.type == EV_ABS) {
            switch (ev.code) { // Axis ; 0 for left-right and 1 for up-down
            case 0:
                if (xAxis < AXIS_CENTER && ev.value >= AXIS_CENTER)
                    Q_EMIT(buttonStateChanged(M_LEFT, false));
                if (xAxis >= AXIS_CENTER && ev.value < AXIS_CENTER)
                    Q_EMIT(buttonStateChanged(M_LEFT, true));
                if (xAxis > AXIS_CENTER && ev.value <= AXIS_CENTER)
                    Q_EMIT(buttonStateChanged(M_RIGHT, false));
                if (xAxis <= AXIS_CENTER && ev.value > AXIS_CENTER)
                    Q_EMIT(buttonStateChanged(M_RIGHT, true));
                xAxis = ev.value;
                break;
            case 1:
                if (yAxis < AXIS_CENTER && ev.value >= AXIS_CENTER)
                    Q_EMIT(buttonStateChanged(M_UP, false));
                if (yAxis >= AXIS_CENTER && ev.value < AXIS_CENTER)
                    Q_EMIT(buttonStateChanged(M_UP, true));
                if (yAxis > AXIS_CENTER && ev.value <= AXIS_CENTER)
                    Q_EMIT(buttonStateChanged(M_DOWN, false));
                if (yAxis <= AXIS_CENTER && ev.value > AXIS_CENTER)
                    Q_EMIT(buttonStateChanged(M_DOWN, true));
                yAxis = ev.value;
                break;
            }
        }
    }
}

#include "moc_evdevcontroller.cpp"
