#include "sonarexample.h"
#include <QDebug>
#include <QCoreApplication>

SonarExample::SonarExample(QObject *parent) : QObject(parent)
{
    ultrasound = new OctoSonarSensor(14);
    connect(ultrasound, &OctoSonarSensor::distanceChanged, this, &SonarExample::onDistanceChanged, Qt::QueuedConnection);
    ultrasound->start(QThread::NormalPriority);
}

SonarExample::~SonarExample()
{
    delete ultrasound;
}

void SonarExample::onDistanceChanged(float distance)
{
    qDebug() << "distance" << distance;
}
