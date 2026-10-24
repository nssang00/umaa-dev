/* Official UMAA 6.0 BatteryReportType on the wire. Official IDL is supplied
 * at build time externally, never checked into this repository. Demo service
 * module returns the battery level; SDK host owns the DDS entities. */
#include <dds/dds.h>
#include "UMAA/EO/BatteryStatus/BatteryReportType.h"
#include "umaa/module_loader.hpp"
#include <cstdio>
#include <exception>
#include <cstdlib>

#define CHECK_DDS(x) do { dds_return_t e_=(x); if(e_<0) { \
  std::fprintf(stderr,"DDS error at %d: %s\n",__LINE__,dds_strretcode(-e_)); return 2; } } while(0)

int main(int argc, char** argv) {
  if (argc != 2) return 2;
  try {
    umaa::ServiceModule battery(argv[1], "Demo::EO::BatteryStatus", "demo-only");
    dds_entity_t participant = dds_create_participant(DDS_DOMAIN_DEFAULT, nullptr, nullptr);
    CHECK_DDS(participant);
    dds_entity_t topic = dds_create_topic(participant, &UMAA_EO_BatteryStatus_BatteryReportType_desc,
                                          "BatteryReportType", nullptr, nullptr);
    CHECK_DDS(topic);
    dds_qos_t* q = dds_create_qos();
    dds_qset_reliability(q,DDS_RELIABILITY_RELIABLE,DDS_SECS(1));
    dds_qset_history(q,DDS_HISTORY_KEEP_LAST,16);
    dds_entity_t writer=dds_create_writer(participant,topic,q,nullptr);
    dds_delete_qos(q);
    CHECK_DDS(writer);
    const char* required = std::getenv("UMAA_EXPECT_READERS");
    const int expected_readers = required ? std::atoi(required) : 1;
    if (expected_readers < 1 || expected_readers > 2) return 9;
    dds_publication_matched_status_t matched{};
    bool found=false;
    for(int n=0;n<200;++n) {
      CHECK_DDS(dds_get_publication_matched_status(writer,&matched));
      if(matched.current_count>=expected_readers){found=true;break;}
      dds_sleepfor(DDS_MSECS(25));
    }
    if(!found){std::fputs("official publisher: reader not discovered\n",stderr);return 3;}
    UMAA_EO_BatteryStatus_BatteryReportType msg{};
    UMAA_Common_Measurement_EnergyPercent charge=20.0;
    msg.chargeRemaining=&charge;
    CHECK_DDS(dds_write(writer,&msg));
    dds_sleepfor(DDS_MSECS(150));
    charge=static_cast<double>(battery.invoke(2,0));
    CHECK_DDS(dds_write(writer,&msg));
    dds_sleepfor(DDS_MSECS(150));
    CHECK_DDS(dds_dispose(writer,&msg));
    dds_sleepfor(DDS_MSECS(100));
    CHECK_DDS(dds_delete(participant));
    std::puts("OFFICIAL_UMAA_WIRE_PUBLISHER: plugin value delivered with official BatteryReportType");
    return 0;
  } catch(const std::exception& e) {
    std::fprintf(stderr,"official module error: %s\n",e.what());return 4;
  }
}
