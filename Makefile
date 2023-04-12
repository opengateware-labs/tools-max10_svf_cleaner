################################################################################
# SPDX-License-Identifier: CC0-1.0
# SPDX-FileType: OTHER
# SPDX-FileCopyrightText: (c) 2022, OpenGateware authors and contributors
################################################################################
# Build Options
################################################################################
SHELL         = /bin/bash -o pipefail
COPYRIGHT     = (c) 2022, OpenGateware authors and contributors
BUG_REPORT    = sig-tools@lists.opengateware.org
################################################################################
# Base Directories
################################################################################
TOP_DIR         = .
SRC_DIR         = $(TOP_DIR)/src
LINUX_BUILD_DIR = $(TOP_DIR)/cmake-release-linux
MSYS_BUILD_DIR  = $(TOP_DIR)/cmake-release-msys
MINGW_BUILD_DIR = $(TOP_DIR)/cmake-release-mingw
MAKEFLAGS      += "-j $(shell nproc)"
################################################################################
NAME = raetro/gateman-builder
VERSION = `date -u +"%Y.%m"`
BUILD_DATE = `date -u +"%Y-%m-%dT%H:%M:%SZ"`
VCS_REF = `git rev-parse --short HEAD`
################################################################################
## Makefile Options
################################################################################
.PHONY: default help mingw msys2 linux install clean docs clean-docs check format format-report format-fix release-linux release-windows release-mingw docker docker-push docker-release docker-run

default: help

linux:
	@cmake -G "Unix Makefiles" -B $(LINUX_BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	@cmake --build $(LINUX_BUILD_DIR) -- $(MAKEFLAGS)

msys2:
	@cmake -G "MinGW Makefiles" -B $(MSYS_BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	@cmake --build $(MSYS_BUILD_DIR) -- $(MAKEFLAGS)

mingw:
	@cmake -DCMAKE_TOOLCHAIN_FILE=./pkg/mingw-w64-x86_64.cmake -B $(MINGW_BUILD_DIR) -DCMAKE_BUILD_TYPE=Release
	@cmake --build $(MINGW_BUILD_DIR) -- $(MAKEFLAGS)

release-linux: clean-linux linux
	@cd $(LINUX_BUILD_DIR) && cpack
	@echo "Release Package for Linux Created"

release-msys2: clean-msys msys2
	@cd $(MSYS_BUILD_DIR) && cpack
	@echo "Release Package for MSYS Created"

release-mingw: clean-mingw mingw
	@cd $(MINGW_BUILD_DIR) && cpack
	@echo "Release Package for MinGW Created"

install-linux: release-linux
	@sudo dpkg --install $(LINUX_BUILD_DIR)/*.deb

clean-linux:
	@echo -e "\033[0;33mCleaning Linux build...\033[0m"
	@rm -rf $(LINUX_BUILD_DIR)

clean-msys:
	@echo -e "\033[0;33mCleaning MSYS build...\033[0m"
	@rm -rf $(MSYS_BUILD_DIR)

clean-mingw:
	@echo -e "\033[0;33mCleaning MinGW build...\033[0m"
	@rm -rf $(MINGW_BUILD_DIR)

clean-all:
	@echo -e "\033[0;33mCleaning all builds...\033[0m"
	@rm -rf $(LINUX_BUILD_DIR)
	@rm -rf $(MSYS_BUILD_DIR)
	@rm -rf $(MINGW_BUILD_DIR)

dist: release-linux release-mingw
	$(Q)doxygen docs/Doxyfile-prj.cfg

docs: clean-docs
	$(Q)doxygen docs/Doxyfile-prj.cfg

clean-docs:
	@echo -e "\033[0;33mCleaning documentation...\033[0m"
	$(Q)$(RM) -rf docs/html/ docs/mkdocs/ docs/xml/ docs/vuepress/

check:
	$(Q)cppcheck --enable=all -f -j8 --inconclusive --std=c++14 --output-file=report_cppcheck.log ./$(SRC_DIR)

format:
	@echo "Formatting Code Style..."
	$(Q)find $(SRC_DIR) \( -name '*.cpp' -o -name '*.hpp' -o -name '*.c' -o -name '*.h' \) -exec clang-format-11 --dry-run -style=file -i {} \;

format-report:
	@echo "Formatting Code Style..."
	$(Q)find $(SRC_DIR) \( -name '*.cpp' -o -name '*.hpp' -o -name '*.c' -o -name '*.h' \) -exec clang-format-11 --dry-run -style=file -i {} \; 2> report_clang-format.log

format-fix:
	@echo "Formatting Code Style..."
	$(Q)find $(SRC_DIR) \( -name '*.cpp' -o -name '*.hpp' -o -name '*.c' -o -name '*.h' \) -exec clang-format-11 -style=file -i {} \;

docker:
	$(Q)docker build --build-arg BUILD_VERSION=$(VERSION) --build-arg BUILD_DATE=$(BUILD_DATE) -t $(NAME):$(VERSION) --force-rm --compress pkg/docker
	$(Q)docker tag $(NAME):$(VERSION) $(NAME):latest

docker-push:
	$(Q)docker push $(NAME):$(VERSION)
	$(Q)docker push $(NAME):latest

docker-release: docker docker-push

docker-run:
	$(Q)docker run -it --rm -v $(PWD)/:/build $(NAME):latest /bin/bash

help:
	@echo "════════════════════════════════════════════════════════════════════════════════"
	@echo " OpenGateware.org - Gateman Help Menu "
	@echo "════════════════════════════════════════════════════════════════════════════════"
	@echo "1. Build:                                                                      "
	@echo "   1. linux             - build for debian/ubuntu distro                       "
	@echo "   2. msys2             - build for windows using MSYS2                        "
	@echo "   3. mingw             - build for windows using linux using mingw            "
	@echo ""
	@echo "2. Release:                                                                    "
	@echo "   1. release-linux     - create and pack a release for linux (.deb/.zip/.tar.gz)"
	@echo "   2. release-msys2     - create and pack a release for windows [msys2] (zip)  "
	@echo "   3. release-mingw     - create and pack a release for windows [mingw] (zip)  "	@echo ""
	@echo ""
	@echo "3. Clean  :                                                                    "
	@echo "   1. clean-linux       - clean linux build                                    "
	@echo "   2. clean-msys2       - clean msys2 build                                    "
	@echo "   3. clean-mingw       - clean mingw build                                    "
	@echo "   4. clean-all         - clean all builds                                     "
	@echo ""
	@echo "4. Others                                                                      "
	@echo "   2. docs              - generate documentation                               "
	@echo "   3. clean-docs        - clean generated documentation                        "
	@echo "   4. check             - run cppcheck for code analysis and generate a report "
	@echo "   5. format            - run clang-format and display errors on the terminal (dry-run) "
	@echo "   6. format-report     - run clang-format and generate a report (dry-run)     "
	@echo "   7. format-fix        - run clang-format and format the code                 "
