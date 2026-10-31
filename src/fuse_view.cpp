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

#include "fuse_view.h"
#include "mount_info.h"

#include <android-base/unique_fd.h>

#include <cerrno>
#include <cstring>
#include <fcntl.h>
#include <fstream>
#include <iterator>
#include <sys/stat.h>
#include <sys/statfs.h>
#include <sys/sysmacros.h>
#include <unistd.h>

namespace floral::fuse {
namespace {

bool Fail(const std::string& message, std::string* error) {
    *error = message;
    return false;
}

bool ReadMounts(std::vector<MountInfo>* mounts, std::string* error) {
    std::ifstream file("/proc/self/mountinfo");
    if (!file) return Fail("cannot read /proc/self/mountinfo", error);
    const std::string text((std::istreambuf_iterator<char>(file)), {});
    return (!file.bad() && ParseMountInfo(text, mounts)) ||
           Fail("invalid /proc/self/mountinfo", error);
}

bool UnderView(const std::string& path) {
    return path == "/proc" || path.compare(0, 6, "/proc/") == 0 ||
           path == "/sys" || path.compare(0, 5, "/sys/") == 0;
}

}  // namespace

bool VerifyLxcfsSource(std::string* error) {
    android::base::unique_fd root(open(kLxcfsRoot, O_RDONLY | O_DIRECTORY | O_NOFOLLOW | O_CLOEXEC));
    if (!root.ok()) return Fail(std::string(kLxcfsRoot) + ": " + strerror(errno), error);

    struct statfs filesystem = {};
    if (fstatfs(root.get(), &filesystem) != 0 || filesystem.f_type != 0x65735546) {
        return Fail("LXCFS source is not a FUSE filesystem", error);
    }

    struct stat pinned = {}, current = {};
    if (fstat(root.get(), &pinned) != 0) return Fail("cannot inspect LXCFS root", error);
    const auto device = std::to_string(major(pinned.st_dev)) + ":" + std::to_string(minor(pinned.st_dev));

    std::vector<MountInfo> mounts;
    if (!ReadMounts(&mounts, error)) return false;
    for (const auto& mount : mounts) {
        if (mount.point != kLxcfsRoot) continue;
        if (lstat(kLxcfsRoot, &current) != 0 || current.st_dev != pinned.st_dev ||
            current.st_ino != pinned.st_ino) return Fail("LXCFS source changed during inspection", error);
        return (mount.root == "/" && mount.device == device && mount.filesystem == kLxcfsFilesystem) ||
               Fail("LXCFS source is not a Floral LXCFS mount", error);
    }
    return Fail("LXCFS source mount is missing", error);
}

bool ResolveBootMode(BootConfig* config, std::string* error) {
    struct stat status = {};
    const bool source = lstat(kLxcfsRoot, &status) == 0;
    if (!source && errno != ENOENT) return Fail(strerror(errno), error);

    if (access("/proc/self/mountinfo", F_OK) == 0) {
        std::vector<MountInfo> mounts;
        if (!ReadMounts(&mounts, error)) return false;
        for (const auto& mount : mounts) {
            if (mount.filesystem != kLxcfsFilesystem || !UnderView(mount.point)) continue;
            if (config->explicit_mode && config->mode == Mode::None) {
                return Fail("none conflicts with installed LXCFS views", error);
            }
            if (!source) return Fail("installed LXCFS view has no source", error);
        }
    } else if (errno != ENOENT) {
        return Fail(strerror(errno), error);
    }

    if (!config->explicit_mode) config->mode = source ? Mode::Lxcfs : Mode::None;
    return config->mode != Mode::Lxcfs || VerifyLxcfsSource(error);
}

}  // namespace floral::fuse
