//
// Created by lianyu on 2023/1/5.
//

#include "util.h"
#include "mcs/util/logging.h"

#include <boost/algorithm/string.hpp>
#include <boost/asio.hpp>

namespace mcs {
  namespace internal {

    std::string GetNodeIpAddress(const std::string &address) {
      std::vector<std::string> parts;
      boost::split(parts, address, boost::is_any_of(":"));
      MCS_CHECK(parts.size() == 2);
      try {
        boost::asio::io_service netService;
        boost::asio::ip::udp::resolver resolver(netService);
        boost::asio::ip::udp::resolver::query query(
                boost::asio::ip::udp::v4(), parts[0], parts[1]);
        boost::asio::ip::udp::resolver::iterator endpoints = resolver.resolve(query);
        boost::asio::ip::udp::endpoint ep = *endpoints;
        boost::asio::ip::udp::socket socket(netService);
        socket.connect(ep);
        boost::asio::ip::address addr = socket.local_endpoint().address();
        return addr.to_string();
        return address;
      } catch (std::exception &e) {
        MCS_LOG(FATAL) << "Could not get the node IP address with socket. Exception: "
                       << e.what();
        return "";
      }
    }

    static std::string ScanToken(std::string::const_iterator &c_str, std::string format) {
      int i = 0;
      std::string result;
      format += "%n";
      if (static_cast<size_t>(sscanf(&*c_str, format.c_str(), &i)) <= 1) {
        result.insert(result.end(), c_str, c_str + i);
        c_str += i;
      }
      return result;
    }

    std::string CreateCommandLine(std::vector<std::string> args) {
      std::string result;
      const std::string safe_chars("%*[-A-Za-z0-9%_=+]");
      const char single_quote = '\'';
      for (size_t a = 0; a != args.size(); ++a) {
        std::string arg = args[a], arg_with_null = arg + '\0';
        std::string::const_iterator i = arg_with_null.begin();
        if (ScanToken(i, safe_chars) != arg) {
          // Prefer single-quotes. Double-quotes have unpredictable behavior, e.g. for "\!".
          std::string quoted;
          quoted += single_quote;
          for (char ch : arg) {
            if (ch == single_quote) {
              quoted += single_quote;
              quoted += '\\';
            }
            quoted += ch;
            if (ch == single_quote) {
              quoted += single_quote;
            }
          }
          quoted += single_quote;
          arg = quoted;
        }
        if (a > 0) {
          result += ' ';
        }
        result += arg;
      }
      return result;
    }


  }  // namespace internal
}  // namespace mcs