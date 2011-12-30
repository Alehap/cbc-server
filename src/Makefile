# Makefile for CMAKE
# Author: Pan, Shi Zhu
# Create: 5 Aug 2010

build_target=build
transfer_targets=clean depend edit_cache install rebuild_cache help list_install_components
cmake_command=cmake ..

all: $(build_target)
	cd $(build_target);$(cmake_command)
	cd $(build_target);$(MAKE)

$(transfer_targets): $(build_target)
	cd $(build_target);$(cmake_command)
	cd $(build_target);$(MAKE) $@

$(build_target):
	mkdir -p $@

.PHONY: $(transfer_targets) all distclean

distclean:
	$(RM) -r build
	$(RM) tags

