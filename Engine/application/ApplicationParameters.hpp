#pragma once

#include <string>
#include <spdlog/spdlog.h>


struct ApplicationParameters {
    float near_plane;
    float far_plane;

    std::string log_location;
    spdlog::level::level_enum log_level;

    std::string window_name;

    int64_t max_fps;

    float daylength;
};

class ApplicationParametersReader {
public:
    static ApplicationParameters read(const std::string& filepath);
};

class ApplicationParametersWriter {
public:
    static void write(const ApplicationParameters& parameters, const std::string& filepath);
};