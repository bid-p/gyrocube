#pragma once

#include <tap/architecture/timeout.hpp>
#include <tap/util_macros.hpp>

#include <utils/common_types.hpp>
#include <vision/jetson_protocol.hpp>
#include <vision/vision_buffer.hpp>

namespace src {
class Drivers;
}

namespace src::vision {

    class JetsonCommunicator {
       public:
        JetsonCommunicator(src::Drivers* drivers);
        DISALLOW_COPY_AND_ASSIGN(JetsonCommunicator);
        ~JetsonCommunicator() = default;

        void initialize();
        void updateSerial();

        inline bool isJetsonOnline() const { return !jetsonOfflineTimeout.isExpired(); }

        inline JetsonMessage const& lastValidMessage() const { return lastMessage; }

       private:
        src::Drivers* drivers;

        uint8_t rawSerialByte;
        visionBuffer<512, uint8_t> messageBuffer;
        JetsonMessage lastMessage;

        size_t nextByteIndex;

        tap::arch::MilliTimeout jetsonOfflineTimeout;

        static constexpr uint32_t JETSON_BAUD_RATE = 115200;
        static constexpr uint16_t JETSON_OFFLINE_TIMEOUT_MILLISECONDS = 5000;
        static constexpr UartPort JETSON_UART_PORT = UartPort::Uart1;
    };
}