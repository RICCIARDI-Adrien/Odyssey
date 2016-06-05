#
# Copyright (C) 2016 Adrien RICCIARDI
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odyssey-raspicam
PKG_RELEASE:=1

PKG_BUILD_PARALLEL:=0

include $(INCLUDE_DIR)/package.mk

define Package/odyssey-raspicam
  SECTION:=base
  CATEGORY:=Odyssey
  TITLE:=Raspberry Pi camera module utilities (raspivid, raspistill...)
  VERSION:=$(PKG_RELEASE)-$(REVISION)
endef

define Package/odyssey-raspicam/description
 Raspberry Pi camera module utilities (raspivid, raspistill...).
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) src/* $(PKG_BUILD_DIR)
endef

define Build/Configure
	cd $(PKG_BUILD_DIR) && IN_OPENWRT=1 \
	AR="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)ar" \
	AS="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC) -c $(TARGET_CFLAGS)" \
	LD="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)ld" \
	NM="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)nm" \
	CC="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC)" \
	GCC="$(TOOLCHAIN_DIR)/bin/$(TARGET_CC)" \
	CXX="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)g++" \
	RANLIB="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)ranlib" \
	STRIP="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)strip" \
	OBJCOPY="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)objcopy" \
	OBJDUMP="$(TOOLCHAIN_DIR)/bin/$(TARGET_CROSS)objdump" \
	TARGET_CPPFLAGS="$(TARGET_CPPFLAGS)" \
	TARGET_CFLAGS="$(TARGET_CFLAGS)" \
	TARGET_LDFLAGS="$(TARGET_LDFLAGS)" \
	cmake . $(CMAKE_GLOBAL_FLAGS) -DCMAKE_BUILD_TYPE=Release
endef

define Build/Compile
	$(MAKE) -C $(PKG_BUILD_DIR)
endef

define Package/odyssey-raspicam/install
	# Install only needed libraries
	$(INSTALL_DIR) $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libbcm_host.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libmmal_core.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libmmal_util.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libmmal_vc_client.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libvchiq_arm.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libvcos.so $(1)/usr/lib/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/lib/libvcsm.so $(1)/usr/lib/
	
	# Install only needed binaries
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/build/bin/raspivid $(1)/usr/sbin/
	
	# Install init script
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,odyssey-raspicam))
