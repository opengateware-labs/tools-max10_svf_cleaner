# MAX10 SVF Cleaner

[![release](https://img.shields.io/github/release/opengateware-labs/tools-max10_svf_cleaner.svg)](https://github.com/opengateware-labs/tools-max10_svf_cleaner/releases)
[![license](https://img.shields.io/github/license/opengateware-labs/tools-max10_svf_cleaner.svg?label=License&color=yellow)](#legal-notices)
[![stars](https://img.shields.io/github/stars/opengateware-labs/tools-max10_svf_cleaner.svg?label=Project%20Stars)](https://github.com/opengateware-labs/tools-max10_svf_cleaner/stargazers)
[![issues](https://img.shields.io/github/issues/opengateware-labs/tools-max10_svf_cleaner.svg?label=Issues&color=red)](https://github.com/opengateware-labs/tools-max10_svf_cleaner/issues)

## Overview

MAX10 SVF Cleaner is a tool that solves a problem related to Intel® MAX® 10 devices.
The issue is that the security key stored in the Initialization Configuration Bits (ICB) settings is erased every time the device is programmed using an Intel Quartus Prime software generated .svf file.
The cause of the issue is the lack of flow control syntax in the Serial Vector Format (.svf), which results in the file containing a "MAX 10 DSM Clear" operation that erases all ICB settings in the internal memory.

## Usage

This tool removes the lines in the .svf file that cause the erasure of the security key and creates a new one.
By doing so, it's possible to preserve the security key stored in the ICB settings and prevent it from being erased every time the device is programmed.

To use MAX10 SVF Cleaner, simply run:

```bash
max10_svf_cleaner <input.svf> <output.svf>
```

## Building

### Unix (Linux and MacOS)

```bash
cmake -G "Unix Makefiles" -B cmake-nix -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-nix
## Create Package (Optional)
cd cmake-nix && cpack
```

### Windows using MingW

```bash
cmake -DCMAKE_TOOLCHAIN_FILE=./pkg/mingw-w64-x86_64.cmake -B cmake-win -DCMAKE_BUILD_TYPE=Release
cmake --build cmake-win
## Create Package (Optional)
cd cmake-win && cpack
```

## Disclaimer

MAX10 SVF Cleaner is provided as-is and without warranty of any kind.
The use of this tool is at your own risk, and the author shall not be held liable for any damages resulting from its use.
It is recommended that you back up your original `.svf` files before using this tool.

## Legal Notices

This work is licensed under multiple licenses.

- All original source code is licensed under [Mozilla Public License 2.0] unless implicit indicated.
- All documentation is licensed under [Creative Commons Attribution Share Alike 4.0 International] Public License.
- Some configuration and data files are licensed under [Creative Commons Zero v1.0 Universal].

Open Gateware and any contributors reserve all others rights, whether under their respective copyrights, patents, or trademarks, whether by implication, estoppel or otherwise.

Individual files may contain the following SPDX license tags as a shorthand for the above copyright and warranty notices:

```text
SPDX-License-Identifier: MPL-2.0
SPDX-License-Identifier: CC-BY-SA-4.0
SPDX-License-Identifier: CC0-1.0
```

This eases machine processing of licensing information based on the SPDX License Identifiers that are available at <https://spdx.org/licenses/>.

The Open Gateware authors and contributors or any of its maintainers are in no way associated with or endorsed by Intel®, Altera®, AMD®, Xilinx®, Lattice®, Microsoft® or any other company not implicit indicated.
All other brands or product names are the property of their respective holders.

<!-- Licenses and Links -->
[Creative Commons Attribution Share Alike 4.0 International]:https://spdx.org/licenses/CC-BY-SA-4.0.html
[Creative Commons Zero v1.0 Universal]:https://spdx.org/licenses/CC0-1.0.html
[Mozilla Public License 2.0]:https://spdx.org/licenses/MPL-2.0.html
