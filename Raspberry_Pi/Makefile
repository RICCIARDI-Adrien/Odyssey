all:
	@# On initial checkout, .config does not exist, so use our .config
	@if [ ! -e OpenWRT/.config ]; \
	then \
		cp OpenWRT_Configuration_File OpenWRT/.config; \
		echo "Forced non-existing .config file with preconfigured one."; \
	# If OpenWRT .config file is newer than our (i.e. it has been modified by using menuconfig), replace our file by the new one to keep modifications \
	elif [ OpenWRT/.config -nt OpenWRT_Configuration_File ]; \
	then \
		cp OpenWRT/.config OpenWRT_Configuration_File; \
		echo "Updated configuration file."; \
	fi

	@# Create links to make Odyssey packages available into OpenWRT build system
	@ln -sf ../../OpenWRT_Odyssey_Packages OpenWRT/package/odyssey

	@# Build image
	cd OpenWRT && $(MAKE)
