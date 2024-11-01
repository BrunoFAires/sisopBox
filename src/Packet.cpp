#include "Packet.h"

Packet::Packet(uint32_t id, MessageType type, Status status, const char *msg)
    : packetId(id), messageType(type), status(status), messageSize(strlen(msg))
{
    strncpy(message, msg, sizeof(message) - 1);
    message[sizeof(message) - 1] = '\0';
}

Packet::Packet()
{
}

uint32_t Packet::getPacketId() const { return packetId; }
MessageType Packet::getMessageType() const { return messageType; }
uint16_t Packet::getMessageSize() const { return messageSize; }
const char *Packet::getMessage() const { return message; }

bool Packet::isStatusError()
{
    return status == Status::ERROR;
}

bool Packet::isConnectionPacket()
{
    return messageType == MessageType::CONNECTION;
}

bool Packet::isDisconnectionPacket()
{
    return messageType == MessageType::DISCONNECTION;
}

bool Packet::isDataPacket()
{
    return messageType == MessageType::DATA;
}

void Packet::setStatus(Status status)
{
    this->status = status;
}

void Packet::deserialize(const char *buffer)
{
    size_t offset = 0;

    memcpy(&packetId, buffer + offset, sizeof(packetId));
    offset += sizeof(packetId);

    uint8_t msgType;
    memcpy(&msgType, buffer + offset, sizeof(msgType));
    messageType = static_cast<MessageType>(msgType);
    offset += sizeof(msgType);

    int8_t status;
    memcpy(&status, buffer + offset, sizeof(status));
    this->status = static_cast<Status>(status);
    offset += sizeof(status);

    memcpy(&messageSize, buffer + offset, sizeof(messageSize));
    offset += sizeof(messageSize);

    //Todo talvez seja um problema futuramente.
    if (messageSize >= sizeof(message))
    {
        messageSize = sizeof(message) - 1;
    }
    memcpy(message, buffer + offset, messageSize);
    message[messageSize] = '\0';
}

const char *Packet::serialize() const
{
    static char buffer[sizeof(packetId) + sizeof(uint8_t) + sizeof(int8_t) +
                       sizeof(messageSize) + 256];
    size_t offset = 0;

    memcpy(buffer + offset, &packetId, sizeof(packetId));
    offset += sizeof(packetId);

    uint8_t msgType = static_cast<uint8_t>(messageType);
    memcpy(buffer + offset, &msgType, sizeof(msgType));
    offset += sizeof(msgType);

    int8_t stat = static_cast<int8_t>(status);
    memcpy(buffer + offset, &stat, sizeof(stat));
    offset += sizeof(stat);

    memcpy(buffer + offset, &messageSize, sizeof(messageSize));
    offset += sizeof(messageSize);

    memcpy(buffer + offset, message, messageSize);

    return buffer;
}

size_t Packet::size() const
{
    return sizeof(packetId) + sizeof(uint8_t) + sizeof(int8_t) +
           sizeof(messageSize) + messageSize;
}
