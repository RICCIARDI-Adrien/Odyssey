all:
	@# On initial checkout, .config does not exist, so use our .config
	@if [ ! -e OpenWRT/.config ]; \
	then \
		cp Files/OpenWRT_Configuration_File OpenWRT/.config; \
		echo "Forced non-existing .config file with preconfigured one."; \
	# If OpenWRT .config file is newer than our (i.e. it has been modified by using menuconfig), replace our file by the new one to keep modifications \
	elif [ OpenWRT/.config -nt Files/OpenWRT_Configuration_File ]; \
	then \
		cp OpenWRT/.config Files/OpenWRT_Configuration_File; \
		echo "Updated configuration file."; \
	fi

	@# Create links to make Odyssey packages available into OpenWRT build system
	@ln -sf ../../Files/package/odyssey OpenWRT/package/odyssey

	@# Build image
	cd OpenWRT && $(MAKE)
