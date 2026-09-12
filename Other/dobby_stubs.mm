#include "dobby_defines.h"
#include <mach/mach.h>
#include <string.h>

// Internal Dobby symbols that libdobby_fixed.a references but does not define.
// Providing real definitions here keeps the final dylib fully self-contained
// (no undefined symbols), so dyld loads the injected tweak cleanly.
// Marked weak so a strong definition from the archive, if ever pulled, wins.

#define DOBBY_STUB __attribute__((weak, visibility("default"), used))

extern "C" {

DOBBY_STUB void log_set_level(int level) { }

DOBBY_STUB void log_switch_to_syslog() { }

DOBBY_STUB void log_switch_to_file(const char *path) { }

DOBBY_STUB void log_internal_impl(int level, const char *format, ...) { }

DOBBY_STUB MemoryOperationError CodePatch(void *address, uint8_t *buffer, uint32_t buffer_size) {
    if (!address || !buffer || buffer_size == 0) return kMemoryOperationError;
    kern_return_t kr = vm_protect(mach_task_self(), (vm_address_t)address, buffer_size, false, VM_PROT_READ | VM_PROT_WRITE | VM_PROT_COPY);
    if (kr != KERN_SUCCESS) return kMemoryOperationError;
    memcpy(address, buffer, buffer_size);
    return kMemoryOperationSuccess;
}

DOBBY_STUB void intercept_routing_common_bridge_handler(RegisterContext *ctx, const HookEntryInfo *info) { }

}