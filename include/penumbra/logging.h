/* Copyright (c) 2023 Big Ladder Software LLC. All rights reserved.
 * See the LICENSE file for additional terms and conditions. */

#ifndef PENUMBRA_LOGGING_H_
#define PENUMBRA_LOGGING_H_

#include <courier/courier.h>
#include <fmt/format.h>

namespace Penumbra {

class PenumbraLogger : public Courier::Courier {
public:
  void receive_error(const std::string& message) override {
    write_message("ERROR", message);
  }

  void receive_warning(const std::string& message) override {
    write_message("WARNING", message);
  }

  void receive_info(const std::string& message) override {
    write_message("NOTE", message);
  }

  void receive_debug(const std::string& message) override {
    write_message("DEBUG", message);
  }

protected:

void write_message(const std::string_view message_type, const std::string_view message) {
    std::cout << fmt::format("  [{}] {}", message_type, message) << std::endl;
  }
};


// class PenumbraException : public Courier::CourierException {
// public:
//   explicit PenumbraException(const std::string &message, Courier::Courier &logger)
//       : CourierException(message, logger) {}
// };

// class SurfaceException : public PenumbraException {
// public:
//   explicit SurfaceException(const unsigned int surface_index,
//                             const std::string_view &surface_context, Courier::Courier &logger)
//       : PenumbraException(std::format(error_message_format, surface_context, surface_index),
//                           logger) {}

// private:
//   static constexpr std::string_view error_message_format = "{} index, {}, does not exist.";
// };
} // namespace Penumbra

#endif // define PENUMBRA_LOGGING_H_
