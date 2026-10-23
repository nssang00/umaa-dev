// Development-only line-transport host. NOT DDS and NOT UMAA wire format.
// Uses generated typed interface; all protocol sequencing is inside SDK.
#include "umaa/generated/EngineControl.hpp"
#include <iostream>
#include <sstream>
#include <string>
#include <vector>

class UserEngine final : public umaa::generated::EngineControlHandler {
public:
  bool validate(const umaa::generated::EngineControlPayload& cmd) override {
    return cmd.targetRpm >= 0 && cmd.targetRpm <= 10000;
  }
  umaa::Outcome execute(const umaa::generated::EngineControlPayload& cmd) override {
    return cmd.targetRpm == 0 ? umaa::Outcome::Completed : umaa::Outcome::Pending;
  }
  bool cancel(const umaa::generated::EngineControlPayload&) override {
    return true; // Mock can always stop; real adapter must confirm actual stop.
  }
};

static void emit(const std::vector<umaa::Event>& events) {
  for (const auto& e : events) {
    switch (e.kind) {
      case umaa::Event::Kind::Status: std::cout << "STATUS " << e.session << " " << e.detail << " " << e.reason; break;
      case umaa::Event::Kind::Ack: std::cout << "ACK " << e.session; break;
      case umaa::Event::Kind::DisposeStatus: std::cout << "DISPOSE_STATUS " << e.session; break;
      case umaa::Event::Kind::DisposeAck: std::cout << "DISPOSE_ACK " << e.session; break;
      case umaa::Event::Kind::Ignored: std::cout << "IGNORED " << e.session << " " << e.detail; break;
    }
    std::cout << '\n';
  }
  std::cout.flush();
}
int main() {
  UserEngine impl;
  umaa::generated::EngineControlProvider provider("p1", impl);
  std::string line;
  while (std::getline(std::cin, line)) {
    std::istringstream in(line);
    std::string op; in >> op;
    if (op == "COMMAND") {
      umaa::Identity i;
      double rpm;
      if (!(in >> i.source >> i.destination >> i.session >> i.timestamp >> rpm)) return 3;
      umaa::generated::EngineControlPayload typed;
      typed.sessionID = i.session;
      typed.targetRpm = rpm;
      emit(provider.receive(i, typed));
    } else if (op == "FINISH") {
      std::string session, answer; if (!(in >> session >> answer)) return 3;
      emit(provider.complete(session, answer == "SUCCESS"));
    } else if (op == "DISPOSE" || op == "LOST_CONSUMER") {
      std::string source, session; if (!(in >> source >> session)) return 3;
      emit(provider.command_disposed(source, session));
    } else if (op == "RESTART") {
      // Emulates recovery policy for existing persisted commands. A real DDS
      // adapter must inspect DURABILITY samples and restore state at startup.
      emit(provider.fail_unrecoverable());
    } else if (op == "EXIT") {
      break;
    } else if (!op.empty()) {
      std::cerr << "Unknown fixture command" << std::endl; return 4;
    }
  }
}
