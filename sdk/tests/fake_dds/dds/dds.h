// Test-only ABI shim. Never installed and never used by production DDS builds.
#pragma once
#include <stdint.h>
#ifdef __cplusplus
extern "C" {
#endif
typedef int32_t dds_entity_t;
typedef int32_t dds_return_t;
dds_return_t dds_write(dds_entity_t writer, const void* data);
dds_return_t dds_dispose(dds_entity_t writer, const void* data);
const char* dds_strretcode(int32_t error);
#ifdef __cplusplus
}
#endif
