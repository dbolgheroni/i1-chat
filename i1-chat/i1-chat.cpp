#include <SimpleAmqpClient/SimpleAmqpClient.h>
#include <iostream>
#include <string>

int main() {
    std::string exchangeName = "i1-chat";
    std::string room1Name = "room1";
    std::string room2Name = "room2";

    std::cout << "info: starting i1-chat" << std::endl;

    AmqpClient::Channel::ptr_t channel;

    try {
        channel = AmqpClient::Channel::Create("localhost");


        // Create the i1-chat exchange. That will be the main exchange for the
        // backend and for all clients connecting to it.
        std::string exchangeType = "direct";
        bool exchangePassive = false;
        bool exchangeDurable = false;
        bool exchangeAutoDelete = false;

        std::cout << "info: creating " << exchangeName << " exchange" << std::endl;
        channel->DeclareExchange(exchangeName, exchangeType,
            exchangePassive, exchangeDurable, exchangeAutoDelete);

        // Create the queues. Each queue represents a room. For now, the number
        // of rooms are fixed and the users cannot create new rooms.
        auto queuePassive = false;
        auto queueDurable = false;
        auto queueExclusive = false;
        auto queueAutoDelete = false;

        std::cout << "info: creating " << room1Name << " queue" << std::endl;
        channel->DeclareQueue(room1Name,
            queuePassive, queueDurable, queueExclusive, queueAutoDelete);

        std::cout << "info: creating " << room2Name << " queue" << std::endl;
        channel->DeclareQueue(room2Name,
            queuePassive, queueDurable, queueExclusive, queueAutoDelete);

        // queue, consumer_tag, no_local, no_ack, exclusive
        std::string tag = channel->BasicConsume("room1", "");

        // Since BasicConsumeMessage is blocking, wait on it until a new
        // message arrives.
        for(;;) {
            AmqpClient::Envelope::ptr_t envelope =
                channel->BasicConsumeMessage(tag);

            if (envelope) {
                std::cout << "debug: received message" << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }


    return 0;
}
