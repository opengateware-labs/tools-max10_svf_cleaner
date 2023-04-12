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

auto getCurrentDateTime() -> std::string {
    auto now = std::chrono::system_clock::now();
    std::time_t const time = std::chrono::system_clock::to_time_t(now);

    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%a %b %d %T %Y");

    return ss.str();
}

/**
 * Rewrite the Device line to remove the sof path
 * @param[in/out] svf_data Serial Vector File as String
 */
void extract_device_name(std::string &svf_data) {
    std::regex const device_info_regex("(!Device #1:) (.*) - (.*)");
    std::smatch match;

    if (regex_search(svf_data, match, device_info_regex)) {
        std::string const device_info = match[1].str() + " " + match[2].str() + " - " + getCurrentDateTime();
        svf_data = regex_replace(svf_data, device_info_regex, device_info);
    }
}

/**
 * @brief Remove the MAX 10 DSM Clear Commands
 * @param[in/out] svf_data Serial Vector File as String
 */
void remove_dsm_clear(std::string &svf_data) {
    // Define the string to search and replace
    std::string const search_string = "!\n!Max 10 DSM Clear\n!\nSIR 10 TDI (203);\nRUNTEST 128 TCK;\nSDR 23 TDI (000000);\nSIR 10 TDI (3F2);\nRUNTEST 8750003 TCK;";
    std::string const replace_string = "!\n!Max 10 DSM Clear (REMOVED)";

    // Find the position of the search string in the input string
    size_t pos = svf_data.find(search_string);

    // Replace all occurrences of the search string with the replace string
    while (pos != std::string::npos) {
        svf_data.replace(pos, search_string.length(), replace_string);
        pos = svf_data.find(search_string, pos + replace_string.length());
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
    if (num_args != 3) {
        std::cerr << "Usage: " << args[0] << " <input.svf> <output.svf>" << std::endl;
        return EXIT_FAILURE;
    }

    // Open the input file for reading
    std::ifstream infile(args[1], std::ios::binary);

    // Check if the file was opened successfully
    if (!infile.is_open()) {
        std::cerr << "Error: Could not open file." << std::endl;
        return EXIT_FAILURE;
    }

    // Read the contents of the file into a string
    std::string svf_data((std::istreambuf_iterator<char>(infile)), std::istreambuf_iterator<char>());

    // Close the input file
    infile.close();

    // Replace the specified string
    remove_dsm_clear(svf_data);
    extract_device_name(svf_data);

    // Open the output file for writing
    std::ofstream outfile(args[2]);

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
