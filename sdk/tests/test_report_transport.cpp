#include "umaa/report_transport.hpp"
#include <cassert>
#include <stdexcept>
#include <string>
#include <vector>

struct Device : umaa::ReportHandler<int> {
  int value{17};
  bool failing{false};
  int read_report() const override {
    if (failing) throw std::runtime_error("device read failed");
    return value;
  }
};
struct Sink : umaa::ReportTransport<int> {
  std::vector<std::string> history;
  std::string fail_action;
  int reported_value{-1};
  void act(const std::string& name) {
    history.push_back(name);
    if (name == fail_action) throw std::runtime_error("injected vendor failure: " + name);
  }
  void register_source(const std::string& s) override { assert(s=="sensor"); act("register"); }
  void publish(const umaa::ReportSample<int>& r) override {
    assert(r.source=="sensor"); reported_value=r.data; act("publish");
  }
  void dispose(const std::string& s, std::int64_t ts) override {
    assert(s=="sensor" && ts>0); act("dispose");
  }
  void unregister_source(const std::string& s) override { assert(s=="sensor"); act("unregister"); }
};
template<class Exception, class Fun> void must_throw(Fun&& f) {
  bool did = false;
  try { f(); } catch(const Exception&) { did=true; }
  assert(did);
}
using Bound = umaa::BoundReportProvider<int>;
int main() {
  Device d; Sink sink; Bound p("sensor",d,sink);
  assert(p.state()==Bound::State::Idle);
  d.failing=true;
  must_throw<std::runtime_error>([&] { p.start(1); });
  assert(p.state()==Bound::State::Idle && sink.history.empty());
  d.failing=false;
  p.start(1);
  assert(p.state()==Bound::State::Active);
  assert((sink.history == std::vector<std::string>{"register","publish"}));
  assert(sink.reported_value==17);
  must_throw<std::logic_error>([&] { p.start(2); });
  must_throw<std::invalid_argument>([&] { p.update(1); });
  d.failing=true;
  must_throw<std::runtime_error>([&] { p.update(2); });
  assert(p.state()==Bound::State::Active && sink.history.size()==2);
  d.failing=false;
  d.value=22;
  p.update(2);
  assert(sink.reported_value==22 && sink.history.size()==3);
  p.shutdown(3);
  assert((sink.history == std::vector<std::string>{
      "register","publish","publish","dispose","unregister"}));
  assert(p.state()==Bound::State::Stopped);
  p.shutdown(4);
  must_throw<std::logic_error>([&] { p.update(5); });

  // Discovery/writer registration fails: never emit a sample or retry.
  Sink registration_fail;
  registration_fail.fail_action="register";
  Bound p2("sensor",d,registration_fail);
  must_throw<std::runtime_error>([&] { p2.start(10); });
  assert(p2.faulted() && registration_fail.history.size()==1);
  must_throw<std::logic_error>([&] { p2.start(11); });

  // Registration succeeded but transport write fails: state is indeterminate.
  Sink publish_fail; publish_fail.fail_action="publish";
  Bound p3("sensor",d,publish_fail);
  must_throw<std::runtime_error>([&] { p3.start(20); });
  assert(p3.faulted() && publish_fail.history.size()==2);
  must_throw<std::logic_error>([&] { p3.shutdown(21); });

  Sink update_fail;
  Bound p4("sensor",d,update_fail);
  p4.start(30);
  update_fail.fail_action="publish";
  must_throw<std::runtime_error>([&] { p4.update(31); });
  assert(p4.faulted() && update_fail.history.size()==3);
  must_throw<std::logic_error>([&] { p4.update(32); });

  Sink dispose_fail; Bound p5("sensor",d,dispose_fail);
  p5.start(40);
  dispose_fail.fail_action="dispose";
  must_throw<std::runtime_error>([&] { p5.shutdown(41); });
  assert(p5.faulted());
  assert(dispose_fail.history.back()=="dispose");
  must_throw<std::logic_error>([&] { p5.shutdown(42); });

  Sink unregister_fail; Bound p6("sensor",d,unregister_fail);
  p6.start(50);
  unregister_fail.fail_action="unregister";
  must_throw<std::runtime_error>([&] { p6.shutdown(51); });
  assert(p6.faulted() && unregister_fail.history.back()=="unregister");

  Sink unrecoverable_sink; Bound p7("sensor",d,unrecoverable_sink);
  p7.start_unrecoverable(60);
  assert((unrecoverable_sink.history==std::vector<std::string>{"register","dispose"}));
  assert(p7.state()==Bound::State::Active);
  p7.update(61);
  p7.shutdown(62);
  assert(unrecoverable_sink.history.back()=="unregister");
}
