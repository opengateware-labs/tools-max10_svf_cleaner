/*******************************************************************************
 * SPDX-License-Identifier: MPL-2.0
 * SPDX-FileType: SOURCE
 * SPDX-FileCopyrightText: (c) 2023, OpenGateware authors and contributors
 *******************************************************************************
 *
 * Copyright (c) 2023, Marcus Andrade <marcus@opengateware.org>
 *
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this file,
 * You can obtain one at https://mozilla.org/MPL/2.0/.
 *
 ******************************************************************************/

#include <chrono>
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <regex>
#include <sstream>
#include <string>
#include <utility>

/**
 * @brief Split a filename into its base name and extension.
 * @param[in] filename The filename to split.
 * @return A tuple containing the base name and extension of the filename.
 */
auto split_filename(const std::string &filename) -> std::pair<std::string, std::string> {
    size_t const dot_pos = filename.find_last_of('.');
    if (dot_pos == std::string::npos) {
        // if the dot is not found, return the whole filename as the first element
        return std::make_pair(filename, "");
    } else {
        // split the filename into two substrings based on the dot position
        std::string const name = filename.substr(0, dot_pos);
        std::string const ext = filename.substr(dot_pos + 1);
        return std::make_pair(name, ext);
    }
}

/**
 * @brief Get the Current Date and Time
 * @return a string in the following format: Tue Apr 12 23:36:31 2023.
 */
auto getCurrentDateTime() -> std::string {
    auto now = std::chrono::system_clock::now();
    std::time_t const time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%a %b %d %T %Y");

    return ss.str();
}

/**
 * @brief Rewrite the Device line to remove the sof path
 * @param[in/out] svf_data Serial Vector File as String
 */
void clear_sof_path(std::string &svf_data) {
    std::regex const device_info_regex("(!Device #1:) (.*) - (.*)");
    std::smatch match;

    if (regex_search(svf_data, match, device_info_regex)) {
        std::string const device_info = match[1].str() + " " + match[2].str() + " - " + getCurrentDateTime();
        svf_data = regex_replace(svf_data, device_info_regex, device_info);
    }
}

/**
 * @brief CLI main application
 * @param[in] num_args number of arguments being passed into the program from the command line
 * @param[in] args array of arguments
 * @return 0 on clean exit, 1 otherwise.
 */
auto main(int num_args, char **args) -> int {
    // Check if the number of parameters are correct
    if (num_args < 2) {
        std::cerr << "Usage: " << args[0] << " input_filename [output_filename]" << std::endl;
        return EXIT_FAILURE;
    }

    // Open the input file for reading
    std::string const input_filename = args[1];
    std::ifstream infile(input_filename, std::ios::binary);

    // Check if the file was opened successfully
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        return EXIT_FAILURE;
    }

    // Read the input file line by line into a string
    std::string svf_data;
    std::string line;
    bool skip_lines = false; // flag to skip lines
    while (getline(infile, line)) {
        // check if the current line matches the starting string
        if (line == "!Max 10 DSM Clear") {
            skip_lines = true; // set the flag to skip lines
        }
        // check if the current line matches the ending string
        if (line == "!Max 10 Disable ISP") {
            skip_lines = false; // unset the flag to stop skipping lines
        }
        // if the flag is set, skip the current line
        if (skip_lines) {
            continue;
        }
        svf_data += line + "\n";
    }

    // Close the input file
    infile.close();

    // Clear the .sof path reference
    clear_sof_path(svf_data);

    // Handle output filename
    std::string output_filename;
    if (num_args >= 3) {
        output_filename = args[2];
    } else {
        // if the output filename is not provided, set it to input_filename_nodsm.ext
        std::pair<std::string, std::string> const file_ext = split_filename(input_filename);
        output_filename = file_ext.first + "_nodsm." + file_ext.second;
    }

    // Open the output file for writing
    std::ofstream outfile(output_filename);

    // Check if the output file was opened successfully
    if (!outfile.is_open()) {
        std::cerr << "Error: Could not open output file." << std::endl;
        return EXIT_FAILURE;
    }

    // Write the contents of the input file to the output file
    outfile << svf_data;

    // Close the output file
    outfile.close();

    return EXIT_SUCCESS;
}
