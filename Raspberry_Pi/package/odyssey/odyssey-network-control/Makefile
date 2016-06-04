#
# Copyright (C) 2016 Adrien RICCIARDI
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odyssey-network-control
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/odyssey-network-control
  SECTION:=base
  CATEGORY:=Odyssey
  TITLE:=Odyssey remote control server
  VERSION:=$(PKG_RELEASE)-$(REVISION)
endef

define Package/odyssey-network-control/description
 Odyssey remote control server.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) src/* $(PKG_BUILD_DIR)
endef

TARGET_CFLAGS += -W -Wall

define Build/Compile
	$(eval SOURCES=$(shell find $(PKG_BUILD_DIR) -name "*.c"))
	$(TARGET_CC) $(TARGET_CFLAGS) $(SOURCES) -o $(PKG_BUILD_DIR)/network-control
endef

define Package/odyssey-network-control/install
	# Install binary
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/network-control $(1)/usr/sbin/
	
	# Install init script
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,odyssey-network-control))
