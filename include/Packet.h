#ifndef PACKET_H
#define PACKET_H

#include <cstdint>
#include <cstring>
#include <iostream>

enum class MessageType
{
    CONNECTION = 1,
    DATA = 2,
    DISCONNECTION = 3
};

enum class Status
{
    SUCCESS = 1,
    ERROR = -1
};

class Packet
{
private:
    uint32_t packetId;
    MessageType messageType;
    Status status;
    uint16_t messageSize;
    char message[256];

public:
    Packet();
    Packet(uint32_t id, MessageType type, Status status, const char *msg);

    uint32_t getPacketId() const;
    MessageType getMessageType() const;
    uint16_t getMessageSize() const;
    const char *getMessage() const;

    void setStatus(Status status);

    const char *serialize() const;
    void deserialize(const char *buffer);

    bool isStatusError();
    bool isDataPacket();
    bool isConnectionPacket();
    bool isDisconnectionPacket();

    size_t size() const;
};

#endif
