#include <QMutexLocker>
#include <unistd.h>
#include "ultraborg.h"
#include "qgpio.h"

#include <QDebug>

//// Constants /////
#define BUFFER_MAX              (32)
#define HANDLE_UNINITALISED     (-1)
#define UB_DEFAULT_PWM_MAX      (4000)  // Servo maximum limit when set to default
#define UB_DEFAULT_PWM_MIN      (2000)  // Servo minimum limit when set to default
#define UB_USM_US_TO_MM         (0.171500)  // Conversion from 'GET_TIME_USM' commands to millimeters

//// Ultrasonic Commands Mapping ////
static const uint8_t UbCommandGetTimeUSM[4] = { 1, 2, 3, 4 };
static const uint8_t UbCommandGetFilterUSM[4] = { 41, 42, 43, 44 };

//// Servo Command Mapping ////
static const uint8_t UbCommandSetPWM[4] = { 5, 7, 9, 11 };
static const uint8_t UbCommandGetPWM[4] = { 6, 8, 10, 12 };
static const uint8_t UbCommandSetPWMCalibrate[4] = { 13, 14, 15, 16 };
static const uint8_t UbCommandGetPWMMin[4] = { 17, 20, 23, 26 };
static const uint8_t UbCommandGetPWMMax[4] = { 18, 21, 24, 27 };
static const uint8_t UbCommandGetPWMBoot[4] = { 19, 22, 25, 28 };
static const uint8_t UbCommandSetPWMMin[4] = { 29, 32, 35, 38 };
static const uint8_t UbCommandSetPWMMax[4] = { 30, 33, 36, 39 };
static const uint8_t UbCommandSetPWMBoot[4] = { 31, 34, 37, 40 };


uint16_t UbServoMin[4] = { UB_DEFAULT_PWM_MIN, UB_DEFAULT_PWM_MIN, UB_DEFAULT_PWM_MIN, UB_DEFAULT_PWM_MIN};;  // The current minimum limit for servo
uint16_t UbServoMax[4] = { UB_DEFAULT_PWM_MAX, UB_DEFAULT_PWM_MAX, UB_DEFAULT_PWM_MAX, UB_DEFAULT_PWM_MAX };  // The current maximum limit for servo

UltraBorg *UltraBorg::getInstance()
{
    QMutex mutex;
    QMutexLocker lock(&mutex);  // to unlock mutex on exit
    static UltraBorg instance;
    return &instance;
}

UltraBorg::UltraBorg()
{
}

// Gets the minimum PWM level for servo output
// This corresponds to position -1
// The value is an integer where 2000 represents a 1 ms servo burst
UltraBorg::~UltraBorg()
{
    deinit();
}

void UltraBorg::deinit() {
    QGpio::getInstance()->deallocateI2CSlave(m_i2c);
}

void UltraBorg::init(uint8_t address)
{
    if (!m_inited) {
        QGpio* gpio = QGpio::getInstance();
        if (gpio->init() == QGpio::INIT_OK) {
            m_i2c = gpio->allocateI2CSlave(address, 1, 1, 40000);
        }

        // Read in the limits for this board
        uint8_t id = m_i2c->i2cRead(0x99);
        if (id == address) {
            qDebug().nospace() << "Ultraborg found at 0x" << Qt::hex << id;
            qDebug() << "EEPROM values:";
            for (int i = 0; i < 4; i++) {
                UbServoMin[i] = getPWMMin(i);
                UbServoMax[i] = getPWMMax(i);
                uint16_t bootVal = getPWMBootValue(i);
                if (bootVal < UbServoMin[i] || bootVal > UbServoMax[i]) {
                    qDebug().nospace() << " Connector " << i << " changing boot value from " << bootVal << " to " << UbServoMin[i];
                    bootVal = UbServoMin[i];
                    setPWMLimits(i, UbServoMin[i], UbServoMax[i], bootVal);
                }
                qDebug().nospace() << " Connector " << i << " min: " << UbServoMin[i] << " max: " << UbServoMax[i] <<  " boot: " << bootVal;
            }
            m_inited = true;
        } else {
            qWarning().nospace() << "Error detecting Ultraborg at address: 0x" << Qt::hex << address;
        }
    }
}

uint16_t UltraBorg::getPWMMin(int servo)
{
    if (servo < 0 || servo > 3) {
        qWarning("UltraBord Error:  Please pass servo {0, 1, 2, 3} only.\n");
        return 0;
    }

    return m_i2c->i2cRead16(UbCommandGetPWMMin[servo]);
}


// Gets the minimum PWM level for servo output
// This corresponds to position +1
// The value is an integer where 2000 represents a 1 ms servo burst
uint16_t UltraBorg::getPWMMax(int servo) {

    if (servo < 0 || servo > 3) {
        qWarning("UltraBord Error:  Please pass servo {0, 1, 2, 3} only.\n");
        return 0;
    }
    return m_i2c->i2cRead16(UbCommandGetPWMMax[servo]);
}


uint16_t UltraBorg::getPWMValue(int servo) {
    //unsigned int pwmLevel;

    if (servo < 0 || servo > 3) {
        qWarning("UltraBord Error:  Please pass servo {0, 1, 2, 3} only.\n");
        return 0;
    }

    return m_i2c->i2cRead16(UbCommandGetPWM[servo]);

}


uint16_t UltraBorg::getPWMBootValue(int servo) {
    return m_i2c->i2cRead16(UbCommandGetPWMBoot[servo]);
}

// PWM value can be set anywhere from 0 for a 0% duty cycle to 65535 for a 100% duty cycle

void UltraBorg::setPWMValue(int servo, uint16_t value)
{
    if (servo < 0 || servo > 3) {
        qWarning("UltraBord Error:  Please pass servo {0, 1, 2, 3} only.\n");
        return;
    }

    m_i2c->i2cWrite(UbCommandSetPWM[servo], value);
}

uint8_t UltraBorg::repeatedWrite(uint8_t reg, uint8_t checkReg, uint16_t value, int numRepeats, int delayAfterWrite)
{
    uint8_t rc = 0;
    uint16_t checkValue = 0;
    int count = 0;
    do {
        rc = m_i2c->i2cWrite(reg, value);
        usleep(delayAfterWrite);// delay after eeprom
        checkValue = m_i2c->i2cRead16(checkReg);
        //qDebug() << "writing to" << reg << "val:" << value << "got from reg:" << checkReg << "val:" << checkValue;
    } while (checkValue != value && ++count < numRepeats);
    if (checkValue != value) {
        qWarning() << "Error writing value:" << value << "got:" << checkValue << "to:" << reg << "after" << numRepeats << "repeats";
    }
    return rc;
}

void UltraBorg::setPWMLimits(int servo, uint16_t min, uint16_t max, uint16_t bootvalue)
{
    if (servo < 0 || servo > 3) {
        qWarning("UltraBord Error:  Please pass servo {0, 1, 2, 3} only.\n");
        return;
    }

    repeatedWrite(UbCommandSetPWMMin[servo], UbCommandGetPWMMin[servo], min);
    repeatedWrite(UbCommandSetPWMMax[servo], UbCommandGetPWMMax[servo], max);
    repeatedWrite(UbCommandSetPWMBoot[servo], UbCommandGetPWMBoot[servo], bootvalue);
}

float UltraBorg::getDistance(int usm) {
    uint16_t time_us;

    if (usm < 0 || usm > 3) {
        qWarning("UltraBord Error:  Please pass usm {0, 1, 2, 3} only.\n");
        return 0;
    }
    time_us = m_i2c->i2cRead16(UbCommandGetFilterUSM[usm]);
    if (time_us == 65535) time_us = 0;
    return (float)time_us * UB_USM_US_TO_MM;
}

float UltraBorg::getRawDistance(int usm) {
    uint16_t time_us;

    if (usm < 0 || usm > 3) {
        qWarning("UltraBord Error:  Please pass usm {0, 1, 2, 3} only.\n");
        return 0;
    }

    time_us = m_i2c->i2cRead16(UbCommandGetTimeUSM[usm]);
    if (time_us == 65535) time_us = 0;
    return (float)time_us * UB_USM_US_TO_MM;
}
