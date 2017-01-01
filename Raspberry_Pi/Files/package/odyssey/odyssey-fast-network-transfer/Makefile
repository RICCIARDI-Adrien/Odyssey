#
# Copyright (C) 2016 Adrien RICCIARDI
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odyssey-fast-network-transfer
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/odyssey-fast-network-transfer
  SECTION:=base
  CATEGORY:=Odyssey
  TITLE:=Odyssey video network transfer server
  VERSION:=$(PKG_RELEASE)-$(REVISION)
endef

define Package/odyssey-fast-network-transfer/description
 Odyssey video network transfer server.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
	$(CP) src/* $(PKG_BUILD_DIR)
endef

TARGET_CFLAGS += -W -Wall -Werror
SOURCES = $(PKG_BUILD_DIR)/Main.c $(PKG_BUILD_DIR)/Network.c

define Build/Compile
	$(TARGET_CC) $(TARGET_CFLAGS) $(SOURCES) -o $(PKG_BUILD_DIR)/fast-network-transfer
endef

define Package/odyssey-fast-network-transfer/install
	# Install binary
	$(INSTALL_DIR) $(1)/usr/sbin/
	$(INSTALL_BIN) $(PKG_BUILD_DIR)/fast-network-transfer $(1)/usr/sbin/
endef

$(eval $(call BuildPackage,odyssey-fast-network-transfer))
