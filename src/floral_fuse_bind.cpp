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

#include <android-base/logging.h>
#include <android-base/properties.h>
#include <string_view>
#include <cstdio>

int main(int argc, char** argv) {
    android::base::InitLogging(argv, &android::base::KernelLogger);
    const bool verify_only = argc == 2 && std::string_view(argv[1]) == "--verify-only";
    if (argc != 1 && !verify_only) {
        LOG(ERROR) << "Usage: floral_fuse_bind [--verify-only]";
        return 1;
    }
    std::vector<std::string> arguments;
    for (const char* key : {"floral_fuse"}) {
        const auto value = android::base::GetProperty(std::string("ro.boot.") + key, "");
        if (!value.empty()) arguments.push_back(std::string("androidboot.") + key + "=" + value);
    }
    floral::fuse::BootConfig config;
    std::string error;
    if (!floral::fuse::ParseBootArguments(arguments, &config, &error) || !config.explicit_mode) {
        LOG(ERROR) << "Missing/invalid init-selected Floral FUSE mode: " << error;
        fprintf(stderr, "Missing/invalid init-selected Floral FUSE mode: %s\n", error.c_str());
        return 1;
    }
    // This validates a fixed choice, never performs legacy mode selection again.
    if (!floral::fuse::ResolveBootMode(&config, &error)) {
        LOG(ERROR) << error;
        fprintf(stderr, "%s\n", error.c_str());
        return 1;
    }
    if (config.mode == floral::fuse::Mode::None || verify_only) return 0;
    return floral::fuse::BindLxcfsViews();
}
