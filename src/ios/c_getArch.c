//
//  c_getArch
//  Platform-specific implementation to retrieve CPU architecture.
//  iOS implementation
//

#include "c_getArch.h"
#include <sys/types.h>
#include <sys/sysctl.h>
#include <mach/machine.h>


char* getCPUArch()
{
    char* cpu;
    size_t size;
    cpu_type_t type;
    cpu_subtype_t subtype;
    size = sizeof(type);
    sysctlbyname("hw.cputype", &type, &size, NULL, 0);
    
    size = sizeof(subtype);
    sysctlbyname("hw.cpusubtype", &subtype, &size, NULL, 0);
    
    // values for cputype and cpusubtype defined in mach/machine.h
    if (type == CPU_TYPE_X86_64) {
        cpu = "x86_64";
    } else if (type == CPU_TYPE_X86) {
        cpu = "x86";
    } else if (type == CPU_TYPE_ARM) {
        cpu = "ARM";
    } else if (type == CPU_TYPE_ARM64) {
        cpu = "ARM_64";
    }else{
        cpu = "UNKNOWN";
    }
    return cpu;
}
