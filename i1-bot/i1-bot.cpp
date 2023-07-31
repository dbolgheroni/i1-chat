#include <chrono>
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

#include <SimpleAmqpClient/SimpleAmqpClient.h>

std::string run_cmd_stock(const std::string&);
std::string parse_stock_price_from_csv(const std::string&);

// Check if the body of the message contains a command and
// reach Stooq if it has.
std::string parse_cmd(const AmqpClient::BasicMessage::ptr_t message) {
    auto cmd = false;

    // Find the /stock command in the message body.
    auto body = message->Body();

    auto say = body;
    if (body.rfind("/stock=", 0) != std::string::npos) {
        auto stock = body.substr(body.find("=") + 1);
        say = run_cmd_stock(stock);
    }

    return say;
}

std::string parse_stock_price_from_csv(const std::string& csv) {
    // Stooq returns 2 rows even for a single stock request.
    // The first is just a description of the columns of the CSV.
    // Skip the first row and get the second one.
    auto row = csv.substr(csv.find("\n") + 1);
    std::cout << "debug: " << row;

    // Parse value for row.
    auto end = row.rfind(',');
    auto beg = row.rfind(',', end - 1) + 1;
    auto quote = row.substr(beg, end - beg);

    return quote;
}

std::string run_cmd_stock(const std::string& stock) {
    //std::cout << "cmd: " << stock << std::endl;

    std::string csvResponse;

    // Make a request to Stooq using curlpp.
    try {
        curlpp::Easy request;

        std::string url = "https://stooq.com/q/l/?s=" + stock +
            "&f=sd2t2ohlcv&h&e=csv";
        request.setOpt(new curlpp::options::Url(url));

        std::stringstream responseStream;
        request.setOpt(new curlpp::options::WriteStream(&responseStream));

        request.perform();
        csvResponse = responseStream.str();
    } catch (curlpp::RuntimeError& e) {
        std::cerr << "error: " << e.what() << std::endl;
    } catch (curlpp::LogicError& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    auto quote = parse_stock_price_from_csv(csvResponse);

    std::string say;
    if (quote == "N/D") {
        say = "There is no such stock.";
    } else {
        say = stock + " quote is $" + quote + " per share.";
    }

    return say;
}

int main() {
    std::string exchangeName {"i1-chat"};
    std::string routingKey {"room1"};

    std::cout << "i1-bot debug messages" << std::endl;

    try {
        std::cout << "connecting to broker" << std::endl;
        AmqpClient::Channel::ptr_t channel =
            AmqpClient::Channel::Create("localhost");
        std::cout << "connected" << std::endl;

        // queue_name, passive, durable, exclusive, auto_delete
        auto tmpQueueName = channel->DeclareQueue("", false, false, true, false);
        channel->BindQueue(tmpQueueName, exchangeName, routingKey);

        // queue, consumer_tag, no_local, no_ack, exclusive
        std::string tag = channel->BasicConsume(tmpQueueName, "");

        while(1) {
            AmqpClient::Envelope::ptr_t envelope =
                channel->BasicConsumeMessage(tag);

            if (envelope) {
                auto message = envelope->Message();
                auto say = parse_cmd(message);

                std::cout << "received: " << say << std::endl;
            }
        }
    } catch (const std::exception& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return 0;
}
