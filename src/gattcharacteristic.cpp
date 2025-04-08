/*
 * BluezQt - Asynchronous Bluez wrapper library
 *
 * SPDX-FileCopyrightText: 2019 Manuel Weichselbaumer <mincequi@web.de>
 *
 * SPDX-License-Identifier: LGPL-2.1-only OR LGPL-3.0-only OR LicenseRef-KDE-Accepted-LGPL
 */

#include "gattcharacteristic.h"

#include <unistd.h>

#include "gattcharacteristic_p.h"
#include "gattservice.h"
#include <sys/socket.h>

namespace BluezQt
{
GattCharacteristic::GattCharacteristic(const QString &uuid, GattService *service)
    : GattCharacteristic(uuid, {QLatin1String("read"), QLatin1String("write")}, service)
{
}

GattCharacteristic::GattCharacteristic(const QString &uuid, const QStringList &flags, GattService *service)
    : QObject(service)
    , d(new GattCharacterisiticPrivate(uuid, flags, service))
{
}

GattCharacteristic::~GattCharacteristic() = default;

QByteArray GattCharacteristic::readValue(const QVariantMap &options)
{
    const auto offset = options.value(QStringLiteral("offset")).toUInt();
    const auto mtu = options.value(QStringLiteral("mtu")).toUInt();
    const auto device = options.value(QStringLiteral("device")).value<QDBusObjectPath>();
    const auto link = options.value(QStringLiteral("link")).toString();

    if (d->m_readCallback) {
        return d->m_readCallback(offset, mtu, device, link);
    }

    return {};
}

void GattCharacteristic::writeValue(const QByteArray &value, const QVariantMap &options)
{
    const auto offset = options.value(QStringLiteral("offset")).toUInt();
    const auto type = options.value(QStringLiteral("type")).toString();
    const auto mtu = options.value(QStringLiteral("mtu")).toUInt();
    const auto device = options.value(QStringLiteral("device")).value<QDBusObjectPath>();
    const auto link = options.value(QStringLiteral("link")).toString();
    const auto prepareAuthorize = options.value(QStringLiteral("prepare-authorize")).toBool();

    Q_EMIT valueWritten(value, offset, mtu, type, device, link, prepareAuthorize);
}

QString GattCharacteristic::uuid() const
{
    return d->m_uuid;
}

const GattService *GattCharacteristic::service() const
{
    return d->m_service;
}

QStringList GattCharacteristic::flags() const
{
    return d->m_flags;
}

void GattCharacteristic::startNotify()
{
    if (d->m_canNotify) {
        d->m_notifying = true;
    }
}

void GattCharacteristic::stopNotify()
{
    d->m_notifying = false;
}

bool GattCharacteristic::isNotifying() const
{
    return d->m_notifying;
}

QDBusObjectPath GattCharacteristic::objectPath() const
{
    return d->m_objectPath;
}

void GattCharacteristic::setReadCallback(ReadCallback callback)
{
    d->m_readCallback = callback;
}

void GattCharacteristic::acquireWrite(const QVariantMap &options, const QDBusMessage &message) {
    const auto mtu = static_cast<quint16>(options.value(QStringLiteral("mtu")).toUInt());
    const auto device = options.value(QStringLiteral("device")).value<QDBusObjectPath>();

    int fds[2];
    if (socketpair(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC | SOCK_NONBLOCK, 0, fds) == -1) {
        QDBusConnection::systemBus().send(message.createErrorReply(QDBusError::Failed, QStringLiteral("socketpair() failed")));
        return;
    }

    auto socket = std::make_shared<QLocalSocket>();
    socket->setSocketDescriptor(fds[0]);

    QDBusConnection::systemBus().send(message.createReply(QVariantList{
        QVariant::fromValue(QDBusUnixFileDescriptor(fds[1])),
        QVariant::fromValue(mtu)
    }));

    Q_EMIT newWriteConnection(device, mtu, socket);
}

void GattCharacteristic::acquireNotify(const QVariantMap &options, const QDBusMessage &message) {
    const auto mtu = static_cast<quint16>(options.value(QStringLiteral("mtu")).toUInt());
    const auto device = options.value(QStringLiteral("device")).value<QDBusObjectPath>();

    int fds[2];
    if (socketpair(AF_UNIX, SOCK_SEQPACKET | SOCK_CLOEXEC | SOCK_NONBLOCK, 0, fds) == -1) {
        QDBusConnection::systemBus().send(message.createErrorReply(QDBusError::Failed, QStringLiteral("socketpair() failed")));
        return;
    }

    auto socket = std::make_shared<QLocalSocket>();
    socket->setSocketDescriptor(fds[0]);

    QDBusConnection::systemBus().send(message.createReply(QVariantList{
        QVariant::fromValue(QDBusUnixFileDescriptor(fds[1])),
        QVariant::fromValue(mtu)
    }));

    Q_EMIT newNotifyConnection(device, mtu, socket);
}



} // namespace BluezQt

#include "moc_gattcharacteristic.cpp"
