#include <iostream>
#include <sstream>

#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>

int main() {
    try {
        curlpp::Easy request;

        request.setOpt(new curlpp::options::Url(
            "https://stooq.com/q/l/?s=aapl.us&f=sd2t2ohlcv&h&e=csv"));

        std::stringstream responseStream;
        request.setOpt(new curlpp::options::WriteStream(&responseStream));

        request.perform();

        std::string csvResponse = responseStream.str();

        std::cout << csvResponse;

    } catch (curlpp::RuntimeError& e) {
        std::cerr << "error: " << e.what() << std::endl;
    } catch (curlpp::LogicError& e) {
        std::cerr << "error: " << e.what() << std::endl;
    }

    return 0;
}
