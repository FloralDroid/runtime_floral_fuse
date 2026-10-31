/*
 * Copyright 2026 FloralDroid
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "mount_info.h"

#include <cstdlib>
#include <iostream>

namespace {

bool Check(bool condition, const char* message) {
    if (!condition) {
        std::cerr << "FAIL: " << message << '\n';
    }
    return condition;
}

}  // namespace

int main() {
    const std::string mountinfo =
            "41 31 0:42 / /proc/cpuinfo rw - fuse.floral-lxcfs "
            "/run/floral-lxcfs/proc/cpuinfo rw\n"
            "42 31 0:43 / /proc/meminfo rw - proc proc rw\n"
            "43 31 0:44 /memfd:floral /sys/class/graphics/fb0/virtual_size ro - "
            "tmpfs memfd:floral rw\n";

    const bool ok =
            Check(floral::fuse::UnescapeMountInfoPath("/run/a\\040b") ==
                          "/run/a b",
                  "mountinfo space escape") &&
            Check(floral::fuse::MountInfoHasFilesystem(
                          mountinfo, "/proc/cpuinfo", "fuse.floral-lxcfs"),
                  "find Floral LXCFS mount") &&
            Check(!floral::fuse::MountInfoHasFilesystem(
                          mountinfo, "/proc/meminfo", "fuse.floral-lxcfs"),
                  "reject unrelated filesystem") &&
            Check(!floral::fuse::MountInfoHasFilesystem(
                          mountinfo, "/proc/stat", "fuse.floral-lxcfs"),
                  "reject unrelated mountpoint") &&
            Check(floral::fuse::MountInfoHasMountpoint(
                          mountinfo, "/sys/class/graphics/fb0/virtual_size"),
                  "find generated file bind mount") &&
            Check(!floral::fuse::MountInfoHasMountpoint(mountinfo, "/proc/stat"),
                  "reject absent mountpoint");
    return ok ? EXIT_SUCCESS : EXIT_FAILURE;
}
