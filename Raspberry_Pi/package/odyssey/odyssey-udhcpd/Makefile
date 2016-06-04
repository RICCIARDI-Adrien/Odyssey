#
# Copyright (C) 2016 Adrien RICCIARDI
#

include $(TOPDIR)/rules.mk

PKG_NAME:=odyssey-udhcpd
PKG_RELEASE:=1

include $(INCLUDE_DIR)/package.mk

define Package/odyssey-udhcpd
  SECTION:=base
  CATEGORY:=Odyssey
  TITLE:=Odyssey DHCP server
  VERSION:=$(PKG_RELEASE)-$(REVISION)
endef

define Package/odyssey-udhcpd/description
 Odyssey DHCP server daemon.
endef

define Build/Prepare
	mkdir -p $(PKG_BUILD_DIR)
endef

define Build/Compile/Default

endef
Build/Compile = $(Build/Compile/Default)

define Package/odyssey-udhcpd/install
	$(CP) ./files/* $(1)/
endef

$(eval $(call BuildPackage,odyssey-udhcpd))
