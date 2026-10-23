#include <dds/dds.h>
#include "DemoBattery.h"
#include <stdio.h>
#include <stdlib.h>

#define ENSURE(expr) do { dds_return_t rc_ = (expr); if (rc_ < 0) { \
    fprintf(stderr,"DDS publisher error at line %d: %s\n",__LINE__,dds_strretcode(-rc_)); return 2; } } while(0)

int main(void) {
  dds_entity_t participant = dds_create_participant(DDS_DOMAIN_DEFAULT, NULL, NULL);
  if (participant < 0) { fprintf(stderr, "DDS participant creation failed: %d (%s)\n", (int)participant, dds_strretcode(-participant)); return 2; }
  dds_entity_t topic = dds_create_topic(participant, &Demo_EO_BatteryReportType_desc,
                                        "Demo::EO::BatteryReportType", NULL, NULL);
  if (topic < 0) { fprintf(stderr, "DDS topic creation failed: %d (%s)\n", (int)topic, dds_strretcode(-topic)); return 2; }
  dds_qos_t *qos = dds_create_qos();
  dds_qset_reliability(qos, DDS_RELIABILITY_RELIABLE, DDS_SECS(1));
  dds_qset_history(qos, DDS_HISTORY_KEEP_LAST, 16);
  dds_entity_t writer = dds_create_writer(participant, topic, qos, NULL);
  dds_delete_qos(qos);
  if (writer < 0) { fprintf(stderr, "DDS writer creation failed: %d (%s)\n", (int)writer, dds_strretcode(-writer)); return 2; }
  dds_publication_matched_status_t matched;
  int found = 0;
  for (int tries = 0; tries < 200; ++tries) {
    ENSURE(dds_get_publication_matched_status(writer, &matched));
    if (matched.current_count > 0) { found = 1; break; }
    dds_sleepfor(DDS_MSECS(25));
  }
  if (!found) { fprintf(stderr,"DDS publisher: no simulator reader discovered\n"); return 3; }
  Demo_EO_BatteryReportType sample = { .source_id = 17, .level_percent = 20 };
  ENSURE(dds_write(writer, &sample));
  dds_sleepfor(DDS_MSECS(150));
  sample.level_percent = 80;
  ENSURE(dds_write(writer, &sample));
  dds_sleepfor(DDS_MSECS(150));
  ENSURE(dds_dispose(writer, &sample));
  dds_sleepfor(DDS_MSECS(100));
  ENSURE(dds_delete(participant));
  puts("PUBLISHER: DDS report update+dispose emitted");
  return 0;
}
