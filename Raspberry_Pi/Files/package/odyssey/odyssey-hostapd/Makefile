#
# Copyright (C) 2016 Adrien RICCIARDI
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odyssey-hostapd
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/odyssey-hostapd
  SECTION:=base
  CATEGORY:=Odyssey
  TITLE:=Odyssey WiFi access point
  VERSION:=$(PKG_RELEASE)-$(REVISION)
endef

define Package/odyssey-hostapd/description
 Odyssey WiFi access point daemon.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/odyssey-hostapd/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,odyssey-hostapd))
