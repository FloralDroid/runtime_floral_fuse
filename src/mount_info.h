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

#pragma once

#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace floral::fuse {

struct MountInfo {
    uint64_t id = 0;
    std::string device;
    std::string root;
    std::string point;
    std::string options;
    std::vector<std::string> optional;
    std::string filesystem;
};

bool ParseMountInfo(std::string_view text, std::vector<MountInfo>* mounts);
bool MountOption(std::string_view options, std::string_view option);

std::string UnescapeMountInfoPath(std::string_view value);

bool MountInfoHasFilesystem(std::string_view mountinfo,
                            std::string_view mountpoint,
                            std::string_view filesystem);

bool MountInfoHasMountpoint(std::string_view mountinfo,
                            std::string_view mountpoint);

}  // namespace floral::fuse
