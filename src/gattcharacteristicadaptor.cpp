/*
 * BluezQt - Asynchronous Bluez wrapper library
 *
 * SPDX-FileCopyrightText: 2019 Manuel Weichselbaumer
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "gattcharacteristicadaptor.h"
#include "gattcharacteristic.h"
#include "gattservice.h"

namespace BluezQt
{
GattCharacteristicAdaptor::GattCharacteristicAdaptor(GattCharacteristic *parent)
    : QDBusAbstractAdaptor(parent)
    , m_gattCharacteristic(parent)
{
}

QString GattCharacteristicAdaptor::uuid() const
{
    return m_gattCharacteristic->uuid();
}

QDBusObjectPath GattCharacteristicAdaptor::service() const
{
    return m_gattCharacteristic->service()->objectPath();
}

QStringList GattCharacteristicAdaptor::flags() const
{
    return m_gattCharacteristic->flags();
}

bool GattCharacteristicAdaptor::writeAcquired() const {
    return true;
}

bool GattCharacteristicAdaptor::notifyAcquired() const {
    return true;
}

QByteArray GattCharacteristicAdaptor::ReadValue(const QVariantMap &options)
{
    return m_gattCharacteristic->readValue(options);
}

void GattCharacteristicAdaptor::WriteValue(const QByteArray &value, const QVariantMap &options)
{
    m_gattCharacteristic->writeValue(value, options);
}

void GattCharacteristicAdaptor::StartNotify()
{
    m_gattCharacteristic->startNotify();
}

void GattCharacteristicAdaptor::StopNotify()
{
    m_gattCharacteristic->stopNotify();
}

void GattCharacteristicAdaptor::AcquireWrite(const QVariantMap &options, const QDBusMessage &message)
{
    m_gattCharacteristic->acquireWrite(options, message);
}

void GattCharacteristicAdaptor::AcquireNotify(const QVariantMap &options, const QDBusMessage &message)
{
    m_gattCharacteristic->acquireNotify(options, message);
}

} // namespace BluezQt

#include "moc_gattcharacteristicadaptor.cpp"
