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

#include <string_view>

namespace floral::fuse {

const char* ModeName(Mode mode) {
    switch (mode) {
        case Mode::None: return "none";
        case Mode::Lxcfs: return "lxcfs";
    }
    return "invalid";
}

bool ParseBootArguments(const std::vector<std::string>& arguments,
                        BootConfig* config, std::string* error) {
    *config = {};
    constexpr std::string_view key = "androidboot.floral_fuse";

    for (const auto& argument : arguments) {
        if (argument.compare(0, key.size(), key) != 0) continue;
        const auto equal = argument.find('=');
        if (argument.substr(0, equal) != key || equal == std::string::npos) {
            *error = "unknown Floral FUSE boot argument: " + argument;
            return false;
        }

        const auto value = argument.substr(equal + 1);
        Mode mode;
        if (value == "none") mode = Mode::None;
        else if (value == "lxcfs") mode = Mode::Lxcfs;
        else {
            *error = "Floral FUSE mode must be none or lxcfs";
            return false;
        }

        if (config->explicit_mode && config->mode != mode) {
            *error = "conflicting Floral FUSE boot modes";
            return false;
        }
        config->mode = mode;
        config->explicit_mode = true;
    }
    return true;
}

std::vector<std::string> BootArguments(const BootConfig& config) {
    return {std::string("androidboot.floral_fuse=") + ModeName(config.mode)};
}

}  // namespace floral::fuse
