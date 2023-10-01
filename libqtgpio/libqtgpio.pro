QT -= gui
QT += core
CONFIG += c++17 static
TEMPLATE = lib
DESTDIR = $$OUT_PWD
INCLUDEPATH += $$PWD $$PWD/rpi $$PWD/hats $$PWD/services $$PWD/displays
MOC_DIR = .moc
OBJECTS_DIR = .obj
TARGET = qgpio

SOURCES += \
    $$PWD/qgpio.cpp \
    $$PWD/qgpioi2cslave.cpp \
    $$PWD/qgpioport.cpp \
    $$PWD/qgpiospi.cpp \
    $$PWD/rpi/rpicpuinfo.cpp \
    $$PWD/rpi/bcm2835.c \
    $$PWD/hats/pca9685/pca9685.cpp \
    $$PWD/hats/ultraborg/ultraborg.cpp \
    $$PWD/services/buttons/QwiicButton.cpp \
    $$PWD/services/displays/GroveLEDMatrix.cpp \
    $$PWD/services/motor/motor_driver_hat.cpp \
    #$$PWD/services/motor/motor_hat_stepper.cpp \
    $$PWD/services/motor/motor_thunderborg.cpp \
    $$PWD/services/motor/pca9685_board.cpp \
    $$PWD/services/pwm/pwmsoftware.cpp \
    $$PWD/services/pwm/pwmpca9685.cpp \
    $$PWD/services/pwm/pwmultraborg.cpp \
    $$PWD/services/sensors/octosonarsensor.cpp \
    $$PWD/services/sensors/ultraborgsensor.cpp \
    $$PWD/services/servo/servocontrol9685.cpp \
    $$PWD/services/servo/servobase.cpp \
    $$PWD/services/servo/servocontrolultraborg.cpp \
    $$PWD/services/servo/servosoftware.cpp \
    $$PWD/services/sensors/hcsr04sensor.cpp \
    $$PWD/services/motor/motorbase.cpp \
    $$PWD/services/motor/motor_driver_board.cpp

HEADERS += \
    $$PWD/qgpio.h \
    $$PWD/qgpioi2cslave.h \
    $$PWD/qgpioport.h \
    $$PWD/qgpiospi.h \
    $$PWD/rpi/rpicpuinfo.h \
    $$PWD/rpi/bcm2835.h \
    $$PWD/hats/pca9685/pca9685.h \
    $$PWD/hats/ultraborg/ultraborg.h \
    $$PWD/services/buttons/QwiicButton.h \
    $$PWD/services/displays/GroveLEDMatrix.h \
    $$PWD/services/motor/motor_driver_hat.h \
    #$$PWD/services/motor/motor_hat_stepper.h \
    $$PWD/services/motor/motor_thunderborg.h \
    $$PWD/services/motor/pca9685_board.h \
    $$PWD/services/pwm/pwmsoftware.h \
    $$PWD/services/pwm/pwmpca9685.h \
    $$PWD/services/pwm/pwmultraborg.h \
    $$PWD/services/sensors/octosonarsensor.h \
    $$PWD/services/sensors/ultraborgsensor.h \
    $$PWD/services/servo/servocontrol9685.h \
    $$PWD/services/servo/servobase.h \
    $$PWD/services/servo/servocontrolultraborg.h \
    $$PWD/services/servo/servosoftware.h \
    $$PWD/services/pwm/pwmbase.h \
    $$PWD/services/sensors/hcsr04sensor.h \
    $$PWD/services/motor/motorbase.h \
    $$PWD/services/motor/motor_driver_board.h

include($$PWD/services/sensors/icm20948/imu20948sensor.pri)
