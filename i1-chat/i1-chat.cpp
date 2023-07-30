#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include <string>

int main() {
    try {
        AmqpClient::Channel::ptr_t channel =
            AmqpClient::Channel::Create("localhost");

        std::string exchangeName = "i1-chat";
        std::string exchangeType = "direct";
        bool durable = true;
        bool autoDelete = true;

        channel->DeclareExchange(exchangeName, exchangeType, durable, autoDelete);

        std::string routingKey = "room1";
        std::string messageBody = "Hello room1 from i1-chat already!";

        AmqpClient::BasicMessage::ptr_t message =
            AmqpClient::BasicMessage::Create(messageBody);

        channel->BasicPublish(exchangeName, routingKey, message);
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return 0;
}
