// Real DDS EngineControl protocol proof with pinned official AUVSI 6.0 IDL.
// The Host owns all DDS entities and command state transitions. The user
// module provides only simulated engine behavior, never DDS administration.
// Not an official UMAA service conformance implementation.
#include <dds/dds.h>
#include "UMAA/EO/EngineControl/EngineCommandType.h"
#include "UMAA/EO/EngineControl/EngineCommandStatusType.h"
#include "UMAA/EO/EngineControl/EngineCommandAckReportType.h"
#include "umaa/command_engine.hpp"
#include "umaa/module_loader.hpp"
#include <cstdint>
#include <cstdio>
#include <cstring>
#include <exception>
#include <string>
#include <time.h>

namespace {
using Cmd = UMAA_EO_EngineControl_EngineCommandType;
using Status = UMAA_EO_EngineControl_EngineCommandStatusType;
using Ack = UMAA_EO_EngineControl_EngineCommandAckReportType;
constexpr auto ISSUED=UMAA_Common_MaritimeEnumeration_CommandStatusEnumModule_ISSUED;
constexpr auto COMMANDED=UMAA_Common_MaritimeEnumeration_CommandStatusEnumModule_COMMANDED;
constexpr auto EXECUTING=UMAA_Common_MaritimeEnumeration_CommandStatusEnumModule_EXECUTING;
constexpr auto CANCELED=UMAA_Common_MaritimeEnumeration_CommandStatusEnumModule_CANCELED;
constexpr auto SUCCEEDED=UMAA_Common_MaritimeEnumeration_CommandStatusReasonEnumModule_SUCCEEDED;
constexpr auto CANCELED_REASON=UMAA_Common_MaritimeEnumeration_CommandStatusReasonEnumModule_CANCELED;

std::string hexid(const uint8_t id[16]) {
  static const char* hex="0123456789abcdef";
  std::string result;
  result.reserve(32);
  for(unsigned i=0;i<16;++i){result+=hex[id[i]>>4];result+=hex[id[i]&15];}
  return result;
}
void fillid(uint8_t id[16], uint8_t seed) {
  std::memset(id,0,16);
  id[0]=seed; id[6]=0x40; id[8]=0x80; id[15]=seed;
}
void report(dds_return_t rc,const char* op) {
  if(rc<0) {
    std::fprintf(stderr,"ENGINE_HOST DDS %s: %s (%d)\n",op,dds_strretcode(-rc),rc);
    throw umaa::ModuleLoadError(op);
  }
}
dds_qos_t* qos() {
  auto* q=dds_create_qos();
  dds_qset_reliability(q,DDS_RELIABILITY_RELIABLE,DDS_SECS(1));
  dds_qset_history(q,DDS_HISTORY_KEEP_ALL,0);
  return q;
}
class EngineHandler final : public umaa::Handler<int> {
 public:
  explicit EngineHandler(umaa::ServiceModule& m):module(m){}
  bool validate(const int& state) override {
    return state==UMAA_Common_MaritimeEnumeration_IgnitionControlEnumModule_OFF ||
           state==UMAA_Common_MaritimeEnumeration_IgnitionControlEnumModule_RUN;
  }
  umaa::Outcome execute(const int& state) override {
    module.invoke(1,state);  // demo-only virtual operation, no hardware attached
    return umaa::Outcome::Pending; // ICD: EngineCommand has no natural completion
  }
  bool cancel(const int&) override {
    module.invoke(1,0); // demo-only virtual stop action
    return true;
  }
 private:
  umaa::ServiceModule& module;
};
auto state_enum(const std::string& state) {
  if(state=="ISSUED")return ISSUED;
  if(state=="COMMANDED")return COMMANDED;
  if(state=="EXECUTING")return EXECUTING;
  if(state=="CANCELED")return CANCELED;
  throw umaa::ModuleLoadError("unsupported demo state");
}
void emit(const std::vector<umaa::Event>& events,const Cmd& incoming,
          dds_entity_t status_writer,dds_entity_t ack_writer) {
  for(const auto& e:events) {
    Status s{};
    fillid(s.source.id,0x22);
    std::memcpy(s.sessionID,incoming.sessionID,16);
    s.timeStamp.seconds=(decltype(s.timeStamp.seconds))time(nullptr);
    s.commandStatus=state_enum(e.detail.empty() ? "EXECUTING" : e.detail);
    s.commandStatusReason=(e.detail=="CANCELED"?CANCELED_REASON:SUCCEEDED);
    Ack ack{};
    ack.command=incoming;
    std::memcpy(ack.sessionID,incoming.sessionID,16);
    fillid(ack.source.id,0x22);
    ack.timeStamp.seconds=s.timeStamp.seconds;
    if(e.kind==umaa::Event::Kind::Status) {
      report(dds_write(status_writer,&s),"write status");
      std::printf("ENGINE_HOST: status=%s\n",e.detail.c_str());
    } else if(e.kind==umaa::Event::Kind::Ack) {
      report(dds_write(ack_writer,&ack),"write ack");
      std::puts("ENGINE_HOST: ack published");
    } else if(e.kind==umaa::Event::Kind::DisposeStatus) {
      report(dds_dispose(status_writer,&s),"dispose status");
      std::puts("ENGINE_HOST: status disposed");
    } else if(e.kind==umaa::Event::Kind::DisposeAck) {
      report(dds_dispose(ack_writer,&ack),"dispose ack");
      std::puts("ENGINE_HOST: ack disposed");
    }
  }
}
} // namespace

int main(int argc,char** argv) {
 if(argc!=2){std::fputs("usage: official_engine_host <engine-service-module>\n",stderr);return 2;}
 try {
  umaa::ServiceModule module(argv[1],"Demo::EO::EngineControl","demo-only");
  EngineHandler handler(module);
  uint8_t provider_id[16];fillid(provider_id,0x22);
  umaa::CommandEngine<int> engine(hexid(provider_id),handler);
  dds_entity_t participant=dds_create_participant(DDS_DOMAIN_DEFAULT,nullptr,nullptr);
  report(participant,"participant");
  dds_entity_t command_topic=dds_create_topic(participant,&UMAA_EO_EngineControl_EngineCommandType_desc,
                                           "EngineCommandType",nullptr,nullptr);
  report(command_topic,"command topic");
  dds_entity_t status_topic=dds_create_topic(participant,&UMAA_EO_EngineControl_EngineCommandStatusType_desc,
                                           "EngineCommandStatusType",nullptr,nullptr);
  report(status_topic,"status topic");
  dds_entity_t ack_topic=dds_create_topic(participant,&UMAA_EO_EngineControl_EngineCommandAckReportType_desc,
                                           "EngineCommandAckReportType",nullptr,nullptr);
  report(ack_topic,"ack topic");
  auto* q=qos();
  dds_entity_t reader=dds_create_reader(participant,command_topic,q,nullptr);
  dds_entity_t status_writer=dds_create_writer(participant,status_topic,q,nullptr);
  dds_entity_t ack_writer=dds_create_writer(participant,ack_topic,q,nullptr);
  dds_delete_qos(q);
  report(reader,"command reader");report(status_writer,"status writer");report(ack_writer,"ack writer");
  bool accepted=false,disposed=false;
  std::string last_source,last_session;
  Cmd last_cmd{};
  for(int tick=0;tick<600 && !disposed;++tick) {
    Cmd data{}; void* samples[1]={&data};dds_sample_info_t info[1]{};
    auto n=dds_take(reader,samples,info,1,1);
    report(n,"read command");
    if(n>0) {
      if(info[0].valid_data) {
        if(accepted) throw umaa::ModuleLoadError("unexpected second command");
        if(std::memcmp(data.destination.id,provider_id,16)!=0)
          throw umaa::ModuleLoadError("command destination did not match configured provider");
        last_source=hexid(data.source.id);
        last_session=hexid(data.sessionID);
        umaa::Identity id{last_source,hexid(data.destination.id),last_session,
                          (int64_t)data.timeStamp.seconds*1000000+data.timeStamp.nanoseconds/1000};
        const int state=static_cast<int>(data.state);
        auto events=engine.receive(id,state);
        last_cmd=data;
        // The test uses no optional pointer fields; do not retain generated DDS pointers.
        last_cmd.plugState=nullptr;last_cmd.propulsion=nullptr;
        emit(events,data,status_writer,ack_writer);
        accepted=true;
      } else if(info[0].instance_state==DDS_IST_NOT_ALIVE_DISPOSED && accepted) {
        // One-session proof; identity saved from the preceding valid sample.
        auto events=engine.command_disposed(last_source,last_session);
        emit(events,last_cmd,status_writer,ack_writer);
        disposed=true;
      }
      dds_sample_free(&data,&UMAA_EO_EngineControl_EngineCommandType_desc,DDS_FREE_CONTENTS);
    } else dds_sleepfor(DDS_MSECS(25));
  }
  if(!accepted || !disposed || engine.size()!=0) throw umaa::ModuleLoadError("command lifecycle incomplete");
  report(dds_wait_for_acks(status_writer,DDS_SECS(3)),"status reliable acknowledgment");
  report(dds_wait_for_acks(ack_writer,DDS_SECS(3)),"ack reliable acknowledgment");
  dds_sleepfor(DDS_MSECS(1500)); // keep writers alive for reader invalid-data observation
  report(dds_delete(participant),"participant delete");
  std::puts("ENGINE_HOST: PASS official EngineControl command ack/status/dispose proof");
  return 0;
 }catch(const std::exception& e) {
  std::fprintf(stderr,"ENGINE_HOST: failure: %s\n",e.what());return 3;
 }
}
