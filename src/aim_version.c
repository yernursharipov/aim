#include "aim_version.h"

#define AIM_VERSION_MAJOR 0
#define AIM_VERSION_MINOR 4
#define AIM_VERSION_PATCH 4

#define AIM_VERSION_CONCAT(major, minor, patch) #major "." #minor "." #patch

#define AIM_VERSION_STRING(major, minor, patch) AIM_VERSION_CONCAT(major, minor, patch)

const char *aim_version_string() {
    return AIM_VERSION_STRING(AIM_VERSION_MAJOR, AIM_VERSION_MINOR, AIM_VERSION_PATCH);
}
