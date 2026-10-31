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

#include <string>
#include <vector>

namespace floral::fuse {

enum class Mode { None, Lxcfs };

struct BootConfig {
    Mode mode = Mode::None;
    bool explicit_mode = false;
};

inline constexpr char kLxcfsRoot[] = "/run/floral-lxcfs";
inline constexpr char kLxcfsFilesystem[] = "fuse.floral-lxcfs";

const char* ModeName(Mode mode);
bool ParseBootArguments(const std::vector<std::string>& arguments,
                        BootConfig* config, std::string* error);
std::vector<std::string> BootArguments(const BootConfig& config);
bool ResolveBootMode(BootConfig* config, std::string* error);
bool VerifyLxcfsSource(std::string* error);
int BindLxcfsViews();

}  // namespace floral::fuse
