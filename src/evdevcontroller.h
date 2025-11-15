// SPDX-FileCopyrightText: 2025 J <jean.chalard@gmail.com>
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef EVDEVCONTROLLER_H
#define EVDEVCONTROLLER_H

#include <QObject>
#include <QSocketNotifier>

#define B_A 0x120
#define B_B 0x121
#define B_X 0x122
#define B_Y 0x123
#define B_L 0x124
#define B_R 0x125

class EvdevController : public QObject
{
    Q_OBJECT
    Q_PROPERTY(const QString devicePath READ devicePath CONSTANT)
    Q_PROPERTY(const int A MEMBER M_A CONSTANT)
    Q_PROPERTY(const int B MEMBER M_B CONSTANT)
    Q_PROPERTY(const int X MEMBER M_X CONSTANT)
    Q_PROPERTY(const int Y MEMBER M_Y CONSTANT)
    Q_PROPERTY(const int L MEMBER M_L CONSTANT)
    Q_PROPERTY(const int R MEMBER M_R CONSTANT)
    Q_PROPERTY(const int LEFT MEMBER M_LEFT CONSTANT)
    Q_PROPERTY(const int RIGHT MEMBER M_RIGHT CONSTANT)
    Q_PROPERTY(const int UP MEMBER M_UP CONSTANT)
    Q_PROPERTY(const int DOWN MEMBER M_DOWN CONSTANT)

public:
    const int M_A = 0x1;
    const int M_B = 0x2;
    const int M_X = 0x4;
    const int M_Y = 0x8;
    const int M_L = 0x10;
    const int M_R = 0x20;
    const int M_LEFT = 0x40;
    const int M_RIGHT = 0x80;
    const int M_UP = 0x100;
    const int M_DOWN = 0x200;
    const int AXIS_CENTER = 128;
    // Device path is passed during construction
    explicit EvdevController(QObject *parent = nullptr);
    ~EvdevController() override;

    const QString devicePath() const;

Q_SIGNALS:
    void buttonStateChanged(const int btn, const bool pressed);

    // Signals errors or connection status
    void error(const QString &message);
    void deviceConnected(const QString &name);
    void deviceDisconnected();

private Q_SLOTS:
    // The core slot triggered by QSocketNotifier when new data is available on the file descriptor
    void readNewEvents();

private:
    const QString path;
    const int fd; // File descriptor for the evdev device
    QSocketNotifier *notifier;
    int xAxis = AXIS_CENTER;
    int yAxis = AXIS_CENTER;
};

#endif // EVDEVCONTROLLER_H
