#pragma once

#include <QThread>
#include <QPointer>
#include <qgpioport.h>

class OctoSonarSensor : public QThread
{
    Q_OBJECT

    Q_PROPERTY(float distance READ distance NOTIFY distanceChanged)

public:
    /**
     * @brief HCSR04Sensor
     * @param echoPort
     * @param triggerPort
     * @param parent
     *
     * after creating the class instance, need to call start() method to start measuring
     */
    explicit OctoSonarSensor(int interruptPort, uint8_t i2cPort = 0x20, QObject *parent = nullptr);
    virtual ~OctoSonarSensor();

    /**
     * @brief distance
     * @return current distance in centimeters
     */
    float distance() const;
    /**
     * @brief stop: stops measuring
     */
    void stop();

    int measureRate() const;
    void setMeasureRate(int measureRate);
private:
    void i2csetup();
    void i2cwrite(uint16_t data);
signals:
    // connect via queued connection
    void distanceChanged(float distance);
protected:
    void run() override;
private:
    QPointer<QGpioPort> m_interruptPort;
    float m_distance = -1.0;
    int m_measureRate = 10; //times per second
    uint64_t m_delay = 100000ULL;
    uint8_t m_nAddress = 0x20;

};
