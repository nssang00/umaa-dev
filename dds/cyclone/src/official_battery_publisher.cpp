/* Official AUVSI UMAA 6.0 BatteryReportType demo wire publisher.
 * This is a selected-service transport-binding proof, not a QoS declaration
 * or full UMAA service conformance claim. Official IDL stays external. */
#include <dds/dds.h>
#include "UMAA/EO/BatteryStatus/BatteryReportType.h"
#include "umaa/module_loader.hpp"
#include "umaa/report_transport.hpp"
#include <cstdio>
#include <cstdlib>
#include <exception>
#include <stdexcept>
#include <string>

struct BatteryHandler final : umaa::ReportHandler<double> {
  double value{20.0};
  double read_report() const override { return value; }
};

static void require_dds(dds_return_t result, const char* action) {
  if (result < 0)
    throw std::runtime_error(std::string(action) + ": " + dds_strretcode(-result));
}

// The verified SDK lifecycle now controls when the DDS Writer is allocated,
// used, disposed and deleted. Topic/QoS values below are demo test settings;
// mandatory, per-service UMAA QoS is not yet established by Component specs.
struct CycloneBatteryWriter final : umaa::ReportTransport<double> {
  dds_entity_t participant{};
  dds_entity_t topic{};
  dds_entity_t writer{};
  int expected_readers{1};
  double charge{};
  UMAA_EO_BatteryStatus_BatteryReportType message{};

  CycloneBatteryWriter(dds_entity_t p, dds_entity_t t, int readers)
      : participant(p), topic(t), expected_readers(readers) {
    if (readers < 1 || readers > 2) throw std::invalid_argument("invalid reader count");
    message.chargeRemaining = &charge;
  }

  void register_source(const std::string& source) override {
    if (source != "battery-demo" || writer > 0)
      throw std::logic_error("unexpected Battery source registration");
    dds_qos_t* q = dds_create_qos();
    if (!q) throw std::runtime_error("cannot allocate DDS QoS");
    dds_qset_reliability(q, DDS_RELIABILITY_RELIABLE, DDS_SECS(1));
    dds_qset_history(q, DDS_HISTORY_KEEP_LAST, 16);
    writer = dds_create_writer(participant, topic, q, nullptr);
    dds_delete_qos(q);
    require_dds(writer, "create writer");
    dds_publication_matched_status_t matched{};
    const int attempts = std::getenv("UMAA_NETEM_RECOVERY") ? 1600 : 200;
    for (int n = 0; n < attempts; ++n) {
      require_dds(dds_get_publication_matched_status(writer, &matched), "reader discovery");
      if (matched.current_count >= expected_readers) return;
      dds_sleepfor(DDS_MSECS(25));
    }
    throw std::runtime_error("official publisher: reader not discovered");
  }
  void publish(const umaa::ReportSample<double>& sample) override {
    if (writer <= 0 || sample.source != "battery-demo")
      throw std::logic_error("write without registered Report source");
    charge = sample.data;
    require_dds(dds_write(writer, &message), "publish Report");
  }
  void dispose(const std::string& source, std::int64_t timestamp) override {
    if (writer <= 0 || source != "battery-demo" || timestamp <= 0)
      throw std::logic_error("dispose without registered Report source");
    require_dds(dds_dispose(writer, &message), "dispose Report");
  }
  void unregister_source(const std::string& source) override {
    if (writer <= 0 || source != "battery-demo")
      throw std::logic_error("unregister without active Report source");
    // Allow the peer to observe DDS instance disposal before deleting Writer.
    dds_sleepfor(DDS_MSECS(100));
    require_dds(dds_delete(writer), "delete writer");
    writer = 0;
  }
};

int main(int argc, char** argv) {
  if (argc != 2) return 2;
  dds_entity_t participant = 0;
  try {
    umaa::ServiceModule battery(argv[1], "Demo::EO::BatteryStatus", "demo-only");
    BatteryHandler handler;
    participant = dds_create_participant(DDS_DOMAIN_DEFAULT, nullptr, nullptr);
    require_dds(participant, "create participant");
    const dds_entity_t topic =
        dds_create_topic(participant, &UMAA_EO_BatteryStatus_BatteryReportType_desc,
                         "BatteryReportType", nullptr, nullptr);
    require_dds(topic, "create official BatteryReportType topic");
    const char* required = std::getenv("UMAA_EXPECT_READERS");
    const int readers = required ? std::atoi(required) : 1;
    CycloneBatteryWriter transport(participant, topic, readers);
    umaa::BoundReportProvider<double> provider("battery-demo", handler, transport);
    provider.start(1);
    dds_sleepfor(DDS_MSECS(150));
    handler.value = static_cast<double>(battery.invoke(2, 0));
    provider.update(2);
    dds_sleepfor(DDS_MSECS(150));
    provider.shutdown(3);
    if (provider.state() != umaa::BoundReportProvider<double>::State::Stopped)
      throw std::logic_error("Report provider not stopped");
    require_dds(dds_delete(participant), "delete participant");
    participant = 0;
    std::puts("OFFICIAL_UMAA_WIRE_PUBLISHER: transport-bound Report lifecycle PASS");
    return 0;
  } catch (const std::exception& e) {
    std::fprintf(stderr, "official module/transport error: %s\n", e.what());
    if (participant > 0) dds_delete(participant);
    return 4;
  }
}
