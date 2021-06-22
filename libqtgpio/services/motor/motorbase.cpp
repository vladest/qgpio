#include "motorbase.h"

MotorBase::MotorBase(QObject *parent) : QObject(parent)
{

}

void MotorBase::setSpeed(float speed)
{
    if (speed != m_speed) {
        m_speed = speed;
        emit speedChanged(speed);
    }
}

float MotorBase::speed() const
{
    return m_speed;
}

Motors::MotorsEnum MotorBase::motor() const
{
    return m_motor;
}

void MotorBase::setMotor(Motors::MotorsEnum newMotor)
{
    m_motor = newMotor;
}
