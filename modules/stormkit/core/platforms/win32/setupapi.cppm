// Copyright (C) 2026 Arthur LAURENT <arthur.laurent4@gmail.com>
// This file is subject to the license terms in the LICENSE file
// found in the top-level of this distribution

module;

#include <stormkit/core/platform_macro.hpp>

#include <SDKDDKVer.h>
#include <winapifamily.h>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <SetupAPI.h>

export module stormkit.platform.win32.setupapi;

export namespace win32 {
    using ::SP_CLASSIMAGELIST_DATA;
    using ::SP_CLASSINSTALL_HEADER;
    using ::SP_DETECTDEVICE_PARAMS;
    using ::SP_DEVICE_INTERFACE_DATA;
    using ::SP_DEVICE_INTERFACE_DETAIL_DATA_A;
    using ::SP_DEVICE_INTERFACE_DETAIL_DATA_W;
    using ::SP_DEVINFO_DATA;
    using ::SP_DEVINFO_LIST_DETAIL_DATA_A;
    using ::SP_DEVINFO_LIST_DETAIL_DATA_W;
    using ::SP_DEVINSTALL_PARAMS_A;
    using ::SP_DEVINSTALL_PARAMS_W;
    using ::SP_DRVINFO_DATA_V1_A;
    using ::SP_DRVINFO_DATA_V1_W;
    using ::SP_DRVINFO_DATA_V2_A;
    using ::SP_DRVINFO_DATA_V2_W;
    using ::SP_DRVINFO_DETAIL_DATA_A;
    using ::SP_DRVINFO_DETAIL_DATA_W;
    using ::SP_DRVINSTALL_PARAMS;
    using ::SP_NEWDEVICEWIZARD_DATA;
    using ::SP_POWERMESSAGEWAKE_PARAMS_A;
    using ::SP_POWERMESSAGEWAKE_PARAMS_W;
    using ::SP_PROPCHANGE_PARAMS;
    using ::SP_PROPSHEETPAGE_REQUEST;
    using ::SP_REMOVEDEVICE_PARAMS;
    using ::SP_SELECTDEVICE_PARAMS_A;
    using ::SP_SELECTDEVICE_PARAMS_W;
    using ::SP_TROUBLESHOOTER_PARAMS_A;
    using ::SP_TROUBLESHOOTER_PARAMS_W;

    using ::SetupDiAskForOEMDisk;
    using ::SetupDiBuildClassInfoList;
    using ::SetupDiBuildClassInfoListExA;
    using ::SetupDiBuildClassInfoListExW;
    using ::SetupDiBuildDriverInfoList;
    using ::SetupDiCallClassInstaller;
    using ::SetupDiCancelDriverInfoSearch;
    using ::SetupDiChangeState;
    using ::SetupDiClassGuidsFromNameA;
    using ::SetupDiClassGuidsFromNameExA;
    using ::SetupDiClassGuidsFromNameExW;
    using ::SetupDiClassGuidsFromNameW;
    using ::SetupDiClassNameFromGuidA;
    using ::SetupDiClassNameFromGuidExA;
    using ::SetupDiClassNameFromGuidExW;
    using ::SetupDiClassNameFromGuidW;
    using ::SetupDiCreateDeviceInfoA;
    using ::SetupDiCreateDeviceInfoList;
    using ::SetupDiCreateDeviceInfoListExA;
    using ::SetupDiCreateDeviceInfoListExW;
    using ::SetupDiCreateDeviceInfoW;
    using ::SetupDiCreateDeviceInterfaceA;
    using ::SetupDiCreateDeviceInterfaceRegKeyA;
    using ::SetupDiCreateDeviceInterfaceRegKeyW;
    using ::SetupDiCreateDeviceInterfaceW;
    using ::SetupDiCreateDevRegKeyA;
    using ::SetupDiCreateDevRegKeyW;
    using ::SetupDiDeleteDeviceInfo;
    using ::SetupDiDeleteDeviceInterfaceData;
    using ::SetupDiDeleteDeviceInterfaceRegKey;
    using ::SetupDiDeleteDevRegKey;
    using ::SetupDiDestroyClassImageList;
    using ::SetupDiDestroyDeviceInfoList;
    using ::SetupDiDestroyDriverInfoList;
    using ::SetupDiDrawMiniIcon;
    using ::SetupDiEnumDeviceInfo;
    using ::SetupDiEnumDeviceInterfaces;
    using ::SetupDiEnumDriverInfoA;
    using ::SetupDiEnumDriverInfoW;
    using ::SetupDiGetActualModelsSectionA;
    using ::SetupDiGetActualModelsSectionW;
    using ::SetupDiGetActualSectionToInstallA;
    using ::SetupDiGetActualSectionToInstallExA;
    using ::SetupDiGetActualSectionToInstallExW;
    using ::SetupDiGetActualSectionToInstallW;
    using ::SetupDiGetClassBitmapIndex;
    using ::SetupDiGetClassDescriptionA;
    using ::SetupDiGetClassDescriptionExA;
    using ::SetupDiGetClassDescriptionExW;
    using ::SetupDiGetClassDescriptionW;
    using ::SetupDiGetClassDevPropertySheetsA;
    using ::SetupDiGetClassDevPropertySheetsW;
    using ::SetupDiGetClassDevsA;
    using ::SetupDiGetClassDevsExA;
    using ::SetupDiGetClassDevsExW;
    using ::SetupDiGetClassDevsW;
    using ::SetupDiGetClassImageIndex;
    using ::SetupDiGetClassImageList;
    using ::SetupDiGetClassImageListExA;
    using ::SetupDiGetClassImageListExW;
    using ::SetupDiGetClassInstallParamsA;
    using ::SetupDiGetClassInstallParamsW;
    using ::SetupDiGetClassPropertyExW;
    using ::SetupDiGetClassPropertyKeys;
    using ::SetupDiGetClassPropertyKeysExW;
    using ::SetupDiGetClassPropertyW;
    using ::SetupDiGetClassRegistryPropertyA;
    using ::SetupDiGetClassRegistryPropertyW;
    using ::SetupDiGetCustomDevicePropertyA;
    using ::SetupDiGetCustomDevicePropertyW;
    using ::SetupDiGetDeviceInfoListClass;
    using ::SetupDiGetDeviceInfoListDetailA;
    using ::SetupDiGetDeviceInfoListDetailW;
    using ::SetupDiGetDeviceInstallParamsA;
    using ::SetupDiGetDeviceInstallParamsW;
    using ::SetupDiGetDeviceInstanceIdA;
    using ::SetupDiGetDeviceInstanceIdW;
    using ::SetupDiGetDeviceInterfaceAlias;
    using ::SetupDiGetDeviceInterfaceDetailA;
    using ::SetupDiGetDeviceInterfaceDetailW;
    using ::SetupDiGetDeviceInterfacePropertyKeys;
    using ::SetupDiGetDeviceInterfacePropertyW;
    using ::SetupDiGetDevicePropertyKeys;
    using ::SetupDiGetDevicePropertyW;
    using ::SetupDiGetDeviceRegistryPropertyA;
    using ::SetupDiGetDeviceRegistryPropertyW;
    using ::SetupDiGetDriverInfoDetailA;
    using ::SetupDiGetDriverInfoDetailW;
    using ::SetupDiGetDriverInstallParamsA;
    using ::SetupDiGetDriverInstallParamsW;
    using ::SetupDiGetHwProfileFriendlyNameA;
    using ::SetupDiGetHwProfileFriendlyNameExA;
    using ::SetupDiGetHwProfileFriendlyNameExW;
    using ::SetupDiGetHwProfileFriendlyNameW;
    using ::SetupDiGetHwProfileList;
    using ::SetupDiGetHwProfileListExA;
    using ::SetupDiGetHwProfileListExW;
    using ::SetupDiGetINFClassA;
    using ::SetupDiGetINFClassW;
    using ::SetupDiGetSelectedDevice;
    using ::SetupDiGetSelectedDriverA;
    using ::SetupDiGetSelectedDriverW;
    using ::SetupDiInstallClassA;
    using ::SetupDiInstallClassExA;
    using ::SetupDiInstallClassExW;
    using ::SetupDiInstallClassW;
    using ::SetupDiInstallDevice;
    using ::SetupDiInstallDeviceInterfaces;
    using ::SetupDiInstallDriverFiles;
    using ::SetupDiLoadClassIcon;
    using ::SetupDiLoadDeviceIcon;
    using ::SetupDiOpenClassRegKey;
    using ::SetupDiOpenClassRegKeyExA;
    using ::SetupDiOpenClassRegKeyExW;
    using ::SetupDiOpenDeviceInfoA;
    using ::SetupDiOpenDeviceInfoW;
    using ::SetupDiOpenDeviceInterfaceA;
    using ::SetupDiOpenDeviceInterfaceRegKey;
    using ::SetupDiOpenDeviceInterfaceW;
    using ::SetupDiOpenDevRegKey;
    using ::SetupDiRegisterCoDeviceInstallers;
    using ::SetupDiRegisterDeviceInfo;
    using ::SetupDiRemoveDevice;
    using ::SetupDiRemoveDeviceInterface;
    using ::SetupDiRestartDevices;
    using ::SetupDiSelectBestCompatDrv;
    using ::SetupDiSelectDevice;
    using ::SetupDiSelectOEMDrv;
    using ::SetupDiSetClassInstallParamsA;
    using ::SetupDiSetClassInstallParamsW;
    using ::SetupDiSetClassPropertyExW;
    using ::SetupDiSetClassPropertyW;
    using ::SetupDiSetClassRegistryPropertyA;
    using ::SetupDiSetClassRegistryPropertyW;
    using ::SetupDiSetDeviceInstallParamsA;
    using ::SetupDiSetDeviceInstallParamsW;
    using ::SetupDiSetDeviceInterfaceDefault;
    using ::SetupDiSetDeviceInterfacePropertyW;
    using ::SetupDiSetDevicePropertyW;
    using ::SetupDiSetDeviceRegistryPropertyA;
    using ::SetupDiSetDeviceRegistryPropertyW;
    using ::SetupDiSetDriverInstallParamsA;
    using ::SetupDiSetDriverInstallParamsW;
    using ::SetupDiSetSelectedDevice;
    using ::SetupDiSetSelectedDriverA;
    using ::SetupDiSetSelectedDriverW;
    using ::SetupDiUnremoveDevice;
    using ::SetupGetInfDriverStoreLocationA;
    using ::SetupGetInfDriverStoreLocationW;
    using ::SetupGetInfPublishedNameA;
    using ::SetupGetInfPublishedNameW;
    using ::SetupGetNonInteractiveMode;
    using ::SetupGetThreadLogToken;
    using ::SetupSetNonInteractiveMode;
    using ::SetupSetThreadLogToken;
    using ::SetupWriteTextLog;
    using ::SetupWriteTextLogError;
    using ::SetupWriteTextLogInfLine;

#pragma push_macro("DIGCF_DEFAULT")
#undef DIGCF_DEFAULT
    inline constexpr auto DIGCF_DEFAULT =
#pragma pop_macro("DIGCF_DEFAULT")
      DIGCF_DEFAULT;
#pragma push_macro("DIGCF_PRESENT")
#undef DIGCF_PRESENT
    inline constexpr auto DIGCF_PRESENT =
#pragma pop_macro("DIGCF_PRESENT")
      DIGCF_PRESENT;
#pragma push_macro("DIGCF_ALLCLASSES")
#undef DIGCF_ALLCLASSES
    inline constexpr auto DIGCF_ALLCLASSES =
#pragma pop_macro("DIGCF_ALLCLASSES")
      DIGCF_ALLCLASSES;
#pragma push_macro("DIGCF_PROFILE")
#undef DIGCF_PROFILE
    inline constexpr auto DIGCF_PROFILE =
#pragma pop_macro("DIGCF_PROFILE")
      DIGCF_PROFILE;
#pragma push_macro("DIGCF_DEVICEINTERFACE")
#undef DIGCF_DEVICEINTERFACE
    inline constexpr auto DIGCF_DEVICEINTERFACE =
#pragma pop_macro("DIGCF_DEVICEINTERFACE")
      DIGCF_DEVICEINTERFACE;

#pragma push_macro("DIBCI_NOINSTALLCLASS")
#undef DIBCI_NOINSTALLCLASS
    inline constexpr auto DIBCI_NOINSTALLCLASS =
#pragma pop_macro("DIBCI_NOINSTALLCLASS")
      DIBCI_NOINSTALLCLASS;
#pragma push_macro("DIBCI_NODISPLAYCLASS")
#undef DIBCI_NODISPLAYCLASS
    inline constexpr auto DIBCI_NODISPLAYCLASS =
#pragma pop_macro("DIBCI_NODISPLAYCLASS")
      DIBCI_NODISPLAYCLASS;

#pragma push_macro("DIOCR_INSTALLER")
#undef DIOCR_INSTALLER
    inline constexpr auto DIOCR_INSTALLER =
#pragma pop_macro("DIOCR_INSTALLER")
      DIOCR_INSTALLER;
#pragma push_macro("DIOCR_INTERFACE")
#undef DIOCR_INTERFACE
    inline constexpr auto DIOCR_INTERFACE =
#pragma pop_macro("DIOCR_INTERFACE")
      DIOCR_INTERFACE;

#pragma push_macro("DIREG_DEV")
#undef DIREG_DEV
    inline constexpr auto DIREG_DEV =
#pragma pop_macro("DIREG_DEV")
      DIREG_DEV;
#pragma push_macro("DIREG_DRV")
#undef DIREG_DRV
    inline constexpr auto DIREG_DRV =
#pragma pop_macro("DIREG_DRV")
      DIREG_DRV;
#pragma push_macro("DIREG_BOTH")
#undef DIREG_BOTH
    inline constexpr auto DIREG_BOTH =
#pragma pop_macro("DIREG_BOTH")
      DIREG_BOTH;

#pragma push_macro("DICLASSPROP_INSTALLER")
#undef DICLASSPROP_INSTALLER
    inline constexpr auto DICLASSPROP_INSTALLER =
#pragma pop_macro("DICLASSPROP_INSTALLER")
      DICLASSPROP_INSTALLER;
#pragma push_macro("DICLASSPROP_INTERFACE")
#undef DICLASSPROP_INTERFACE
    inline constexpr auto DICLASSPROP_INTERFACE =
#pragma pop_macro("DICLASSPROP_INTERFACE")
      DICLASSPROP_INTERFACE;

#pragma push_macro("SPDRP_DEVICEDESC")
#undef SPDRP_DEVICEDESC
    inline constexpr auto SPDRP_DEVICEDESC =
#pragma pop_macro("SPDRP_DEVICEDESC")
      SPDRP_DEVICEDESC;
#pragma push_macro("SPDRP_HARDWAREID")
#undef SPDRP_HARDWAREID
    inline constexpr auto SPDRP_HARDWAREID =
#pragma pop_macro("SPDRP_HARDWAREID")
      SPDRP_HARDWAREID;
#pragma push_macro("SPDRP_COMPATIBLEIDS")
#undef SPDRP_COMPATIBLEIDS
    inline constexpr auto SPDRP_COMPATIBLEIDS =
#pragma pop_macro("SPDRP_COMPATIBLEIDS")
      SPDRP_COMPATIBLEIDS;
#pragma push_macro("SPDRP_UNUSED0")
#undef SPDRP_UNUSED0
    inline constexpr auto SPDRP_UNUSED0 =
#pragma pop_macro("SPDRP_UNUSED0")
      SPDRP_UNUSED0;
#pragma push_macro("SPDRP_SERVICE")
#undef SPDRP_SERVICE
    inline constexpr auto SPDRP_SERVICE =
#pragma pop_macro("SPDRP_SERVICE")
      SPDRP_SERVICE;
#pragma push_macro("SPDRP_UNUSED1")
#undef SPDRP_UNUSED1
    inline constexpr auto SPDRP_UNUSED1 =
#pragma pop_macro("SPDRP_UNUSED1")
      SPDRP_UNUSED1;
#pragma push_macro("SPDRP_UNUSED2")
#undef SPDRP_UNUSED2
    inline constexpr auto SPDRP_UNUSED2 =
#pragma pop_macro("SPDRP_UNUSED2")
      SPDRP_UNUSED2;
#pragma push_macro("SPDRP_CLASS")
#undef SPDRP_CLASS
    inline constexpr auto SPDRP_CLASS =
#pragma pop_macro("SPDRP_CLASS")
      SPDRP_CLASS;
#pragma push_macro("SPDRP_CLASSGUID")
#undef SPDRP_CLASSGUID
    inline constexpr auto SPDRP_CLASSGUID =
#pragma pop_macro("SPDRP_CLASSGUID")
      SPDRP_CLASSGUID;
#pragma push_macro("SPDRP_DRIVER")
#undef SPDRP_DRIVER
    inline constexpr auto SPDRP_DRIVER =
#pragma pop_macro("SPDRP_DRIVER")
      SPDRP_DRIVER;
#pragma push_macro("SPDRP_CONFIGFLAGS")
#undef SPDRP_CONFIGFLAGS
    inline constexpr auto SPDRP_CONFIGFLAGS =
#pragma pop_macro("SPDRP_CONFIGFLAGS")
      SPDRP_CONFIGFLAGS;
#pragma push_macro("SPDRP_MFG")
#undef SPDRP_MFG
    inline constexpr auto SPDRP_MFG =
#pragma pop_macro("SPDRP_MFG")
      SPDRP_MFG;
#pragma push_macro("SPDRP_FRIENDLYNAME")
#undef SPDRP_FRIENDLYNAME
    inline constexpr auto SPDRP_FRIENDLYNAME =
#pragma pop_macro("SPDRP_FRIENDLYNAME")
      SPDRP_FRIENDLYNAME;
#pragma push_macro("SPDRP_LOCATION_INFORMATION")
#undef SPDRP_LOCATION_INFORMATION
    inline constexpr auto SPDRP_LOCATION_INFORMATION =
#pragma pop_macro("SPDRP_LOCATION_INFORMATION")
      SPDRP_LOCATION_INFORMATION;
#pragma push_macro("SPDRP_PHYSICAL_DEVICE_OBJECT_NAME")
#undef SPDRP_PHYSICAL_DEVICE_OBJECT_NAME
    inline constexpr auto SPDRP_PHYSICAL_DEVICE_OBJECT_NAME =
#pragma pop_macro("SPDRP_PHYSICAL_DEVICE_OBJECT_NAME")
      SPDRP_PHYSICAL_DEVICE_OBJECT_NAME;
#pragma push_macro("SPDRP_CAPABILITIES")
#undef SPDRP_CAPABILITIES
    inline constexpr auto SPDRP_CAPABILITIES =
#pragma pop_macro("SPDRP_CAPABILITIES")
      SPDRP_CAPABILITIES;
#pragma push_macro("SPDRP_UI_NUMBER")
#undef SPDRP_UI_NUMBER
    inline constexpr auto SPDRP_UI_NUMBER =
#pragma pop_macro("SPDRP_UI_NUMBER")
      SPDRP_UI_NUMBER;
#pragma push_macro("SPDRP_UPPERFILTERS")
#undef SPDRP_UPPERFILTERS
    inline constexpr auto SPDRP_UPPERFILTERS =
#pragma pop_macro("SPDRP_UPPERFILTERS")
      SPDRP_UPPERFILTERS;
#pragma push_macro("SPDRP_LOWERFILTERS")
#undef SPDRP_LOWERFILTERS
    inline constexpr auto SPDRP_LOWERFILTERS =
#pragma pop_macro("SPDRP_LOWERFILTERS")
      SPDRP_LOWERFILTERS;
#pragma push_macro("SPDRP_BUSTYPEGUID")
#undef SPDRP_BUSTYPEGUID
    inline constexpr auto SPDRP_BUSTYPEGUID =
#pragma pop_macro("SPDRP_BUSTYPEGUID")
      SPDRP_BUSTYPEGUID;
#pragma push_macro("SPDRP_LEGACYBUSTYPE")
#undef SPDRP_LEGACYBUSTYPE
    inline constexpr auto SPDRP_LEGACYBUSTYPE =
#pragma pop_macro("SPDRP_LEGACYBUSTYPE")
      SPDRP_LEGACYBUSTYPE;
#pragma push_macro("SPDRP_BUSNUMBER")
#undef SPDRP_BUSNUMBER
    inline constexpr auto SPDRP_BUSNUMBER =
#pragma pop_macro("SPDRP_BUSNUMBER")
      SPDRP_BUSNUMBER;
#pragma push_macro("SPDRP_ENUMERATOR_NAME")
#undef SPDRP_ENUMERATOR_NAME
    inline constexpr auto SPDRP_ENUMERATOR_NAME =
#pragma pop_macro("SPDRP_ENUMERATOR_NAME")
      SPDRP_ENUMERATOR_NAME;
#pragma push_macro("SPDRP_SECURITY")
#undef SPDRP_SECURITY
    inline constexpr auto SPDRP_SECURITY =
#pragma pop_macro("SPDRP_SECURITY")
      SPDRP_SECURITY;
#pragma push_macro("SPDRP_SECURITY_SDS")
#undef SPDRP_SECURITY_SDS
    inline constexpr auto SPDRP_SECURITY_SDS =
#pragma pop_macro("SPDRP_SECURITY_SDS")
      SPDRP_SECURITY_SDS;
#pragma push_macro("SPDRP_DEVTYPE")
#undef SPDRP_DEVTYPE
    inline constexpr auto SPDRP_DEVTYPE =
#pragma pop_macro("SPDRP_DEVTYPE")
      SPDRP_DEVTYPE;
#pragma push_macro("SPDRP_EXCLUSIVE")
#undef SPDRP_EXCLUSIVE
    inline constexpr auto SPDRP_EXCLUSIVE =
#pragma pop_macro("SPDRP_EXCLUSIVE")
      SPDRP_EXCLUSIVE;
#pragma push_macro("SPDRP_CHARACTERISTICS")
#undef SPDRP_CHARACTERISTICS
    inline constexpr auto SPDRP_CHARACTERISTICS =
#pragma pop_macro("SPDRP_CHARACTERISTICS")
      SPDRP_CHARACTERISTICS;
#pragma push_macro("SPDRP_ADDRESS")
#undef SPDRP_ADDRESS
    inline constexpr auto SPDRP_ADDRESS =
#pragma pop_macro("SPDRP_ADDRESS")
      SPDRP_ADDRESS;
#pragma push_macro("SPDRP_UI_NUMBER_DESC_FORMAT")
#undef SPDRP_UI_NUMBER_DESC_FORMAT
    inline constexpr auto SPDRP_UI_NUMBER_DESC_FORMAT =
#pragma pop_macro("SPDRP_UI_NUMBER_DESC_FORMAT")
      SPDRP_UI_NUMBER_DESC_FORMAT;
#pragma push_macro("SPDRP_DEVICE_POWER_DATA")
#undef SPDRP_DEVICE_POWER_DATA
    inline constexpr auto SPDRP_DEVICE_POWER_DATA =
#pragma pop_macro("SPDRP_DEVICE_POWER_DATA")
      SPDRP_DEVICE_POWER_DATA;
#pragma push_macro("SPDRP_REMOVAL_POLICY")
#undef SPDRP_REMOVAL_POLICY
    inline constexpr auto SPDRP_REMOVAL_POLICY =
#pragma pop_macro("SPDRP_REMOVAL_POLICY")
      SPDRP_REMOVAL_POLICY;
#pragma push_macro("SPDRP_REMOVAL_POLICY_HW_DEFAULT")
#undef SPDRP_REMOVAL_POLICY_HW_DEFAULT
    inline constexpr auto SPDRP_REMOVAL_POLICY_HW_DEFAULT =
#pragma pop_macro("SPDRP_REMOVAL_POLICY_HW_DEFAULT")
      SPDRP_REMOVAL_POLICY_HW_DEFAULT;
#pragma push_macro("SPDRP_REMOVAL_POLICY_OVERRIDE")
#undef SPDRP_REMOVAL_POLICY_OVERRIDE
    inline constexpr auto SPDRP_REMOVAL_POLICY_OVERRIDE =
#pragma pop_macro("SPDRP_REMOVAL_POLICY_OVERRIDE")
      SPDRP_REMOVAL_POLICY_OVERRIDE;
#pragma push_macro("SPDRP_INSTALL_STATE")
#undef SPDRP_INSTALL_STATE
    inline constexpr auto SPDRP_INSTALL_STATE =
#pragma pop_macro("SPDRP_INSTALL_STATE")
      SPDRP_INSTALL_STATE;
#pragma push_macro("SPDRP_LOCATION_PATHS")
#undef SPDRP_LOCATION_PATHS
    inline constexpr auto SPDRP_LOCATION_PATHS =
#pragma pop_macro("SPDRP_LOCATION_PATHS")
      SPDRP_LOCATION_PATHS;
#pragma push_macro("SPDRP_BASE_CONTAINERID")
#undef SPDRP_BASE_CONTAINERID
    inline constexpr auto SPDRP_BASE_CONTAINERID =
#pragma pop_macro("SPDRP_BASE_CONTAINERID")
      SPDRP_BASE_CONTAINERID;
#pragma push_macro("SPDRP_MAXIMUM_PROPERTY")
#undef SPDRP_MAXIMUM_PROPERTY
    inline constexpr auto SPDRP_MAXIMUM_PROPERTY =
#pragma pop_macro("SPDRP_MAXIMUM_PROPERTY")
      SPDRP_MAXIMUM_PROPERTY;

#pragma push_macro("SPCRP_UPPERFILTERS")
#undef SPCRP_UPPERFILTERS
    inline constexpr auto SPCRP_UPPERFILTERS =
#pragma pop_macro("SPCRP_UPPERFILTERS")
      SPCRP_UPPERFILTERS;
#pragma push_macro("SPCRP_LOWERFILTERS")
#undef SPCRP_LOWERFILTERS
    inline constexpr auto SPCRP_LOWERFILTERS =
#pragma pop_macro("SPCRP_LOWERFILTERS")
      SPCRP_LOWERFILTERS;
#pragma push_macro("SPCRP_SECURITY")
#undef SPCRP_SECURITY
    inline constexpr auto SPCRP_SECURITY =
#pragma pop_macro("SPCRP_SECURITY")
      SPCRP_SECURITY;
#pragma push_macro("SPCRP_SECURITY_SDS")
#undef SPCRP_SECURITY_SDS
    inline constexpr auto SPCRP_SECURITY_SDS =
#pragma pop_macro("SPCRP_SECURITY_SDS")
      SPCRP_SECURITY_SDS;
#pragma push_macro("SPCRP_DEVTYPE")
#undef SPCRP_DEVTYPE
    inline constexpr auto SPCRP_DEVTYPE =
#pragma pop_macro("SPCRP_DEVTYPE")
      SPCRP_DEVTYPE;
#pragma push_macro("SPCRP_EXCLUSIVE")
#undef SPCRP_EXCLUSIVE
    inline constexpr auto SPCRP_EXCLUSIVE =
#pragma pop_macro("SPCRP_EXCLUSIVE")
      SPCRP_EXCLUSIVE;
#pragma push_macro("SPCRP_CHARACTERISTICS")
#undef SPCRP_CHARACTERISTICS
    inline constexpr auto SPCRP_CHARACTERISTICS =
#pragma pop_macro("SPCRP_CHARACTERISTICS")
      SPCRP_CHARACTERISTICS;
#pragma push_macro("SPCRP_MAXIMUM_PROPERTY")
#undef SPCRP_MAXIMUM_PROPERTY
    inline constexpr auto SPCRP_MAXIMUM_PROPERTY =
#pragma pop_macro("SPCRP_MAXIMUM_PROPERTY")
      SPCRP_MAXIMUM_PROPERTY;

#pragma push_macro("DMI_MASK")
#undef DMI_MASK
    inline constexpr auto DMI_MASK =
#pragma pop_macro("DMI_MASK")
      DMI_MASK;
#pragma push_macro("DMI_BKCOLOR")
#undef DMI_BKCOLOR
    inline constexpr auto DMI_BKCOLOR =
#pragma pop_macro("DMI_BKCOLOR")
      DMI_BKCOLOR;
#pragma push_macro("DMI_USERECT")
#undef DMI_USERECT
    inline constexpr auto DMI_USERECT =
#pragma pop_macro("DMI_USERECT")
      DMI_USERECT;

#pragma push_macro("DIGCDP_FLAG_BASIC")
#undef DIGCDP_FLAG_BASIC
    inline constexpr auto DIGCDP_FLAG_BASIC =
#pragma pop_macro("DIGCDP_FLAG_BASIC")
      DIGCDP_FLAG_BASIC;
#pragma push_macro("DIGCDP_FLAG_ADVANCED")
#undef DIGCDP_FLAG_ADVANCED
    inline constexpr auto DIGCDP_FLAG_ADVANCED =
#pragma pop_macro("DIGCDP_FLAG_ADVANCED")
      DIGCDP_FLAG_ADVANCED;

#pragma push_macro("IDI_RESOURCEFIRST")
#undef IDI_RESOURCEFIRST
    inline constexpr auto IDI_RESOURCEFIRST =
#pragma pop_macro("IDI_RESOURCEFIRST")
      IDI_RESOURCEFIRST;
#pragma push_macro("IDI_RESOURCE")
#undef IDI_RESOURCE
    inline constexpr auto IDI_RESOURCE =
#pragma pop_macro("IDI_RESOURCE")
      IDI_RESOURCE;
#pragma push_macro("IDI_RESOURCELAST")
#undef IDI_RESOURCELAST
    inline constexpr auto IDI_RESOURCELAST =
#pragma pop_macro("IDI_RESOURCELAST")
      IDI_RESOURCELAST;
#pragma push_macro("IDI_RESOURCEOVERLAYFIRST")
#undef IDI_RESOURCEOVERLAYFIRST
    inline constexpr auto IDI_RESOURCEOVERLAYFIRST =
#pragma pop_macro("IDI_RESOURCEOVERLAYFIRST")
      IDI_RESOURCEOVERLAYFIRST;
#pragma push_macro("IDI_RESOURCEOVERLAYLAST")
#undef IDI_RESOURCEOVERLAYLAST
    inline constexpr auto IDI_RESOURCEOVERLAYLAST =
#pragma pop_macro("IDI_RESOURCEOVERLAYLAST")
      IDI_RESOURCEOVERLAYLAST;
#pragma push_macro("IDI_CONFLICT")
#undef IDI_CONFLICT
    inline constexpr auto IDI_CONFLICT =
#pragma pop_macro("IDI_CONFLICT")
      IDI_CONFLICT;
#pragma push_macro("IDI_CLASSICON_OVERLAYFIRST")
#undef IDI_CLASSICON_OVERLAYFIRST
    inline constexpr auto IDI_CLASSICON_OVERLAYFIRST =
#pragma pop_macro("IDI_CLASSICON_OVERLAYFIRST")
      IDI_CLASSICON_OVERLAYFIRST;
#pragma push_macro("IDI_CLASSICON_OVERLAYLAST")
#undef IDI_CLASSICON_OVERLAYLAST
    inline constexpr auto IDI_CLASSICON_OVERLAYLAST =
#pragma pop_macro("IDI_CLASSICON_OVERLAYLAST")
      IDI_CLASSICON_OVERLAYLAST;
#pragma push_macro("IDI_PROBLEM_OVL")
#undef IDI_PROBLEM_OVL
    inline constexpr auto IDI_PROBLEM_OVL =
#pragma pop_macro("IDI_PROBLEM_OVL")
      IDI_PROBLEM_OVL;
#pragma push_macro("IDI_DISABLED_OVL")
#undef IDI_DISABLED_OVL
    inline constexpr auto IDI_DISABLED_OVL =
#pragma pop_macro("IDI_DISABLED_OVL")
      IDI_DISABLED_OVL;
#pragma push_macro("IDI_FORCED_OVL")
#undef IDI_FORCED_OVL
    inline constexpr auto IDI_FORCED_OVL =
#pragma pop_macro("IDI_FORCED_OVL")
      IDI_FORCED_OVL;

#pragma push_macro("SPWPT_SELECTDEVICE")
#undef SPWPT_SELECTDEVICE
    inline constexpr auto SPWPT_SELECTDEVICE =
#pragma pop_macro("SPWPT_SELECTDEVICE")
      SPWPT_SELECTDEVICE;

#pragma push_macro("SPWP_USE_DEVINFO_DATA")
#undef SPWP_USE_DEVINFO_DATA
    inline constexpr auto SPWP_USE_DEVINFO_DATA =
#pragma pop_macro("SPWP_USE_DEVINFO_DATA")
      SPWP_USE_DEVINFO_DATA;

#pragma push_macro("SIGNERSCORE_UNKNOWN")
#undef SIGNERSCORE_UNKNOWN
    inline constexpr auto SIGNERSCORE_UNKNOWN =
#pragma pop_macro("SIGNERSCORE_UNKNOWN")
      SIGNERSCORE_UNKNOWN;
#pragma push_macro("SIGNERSCORE_W9X_SUSPECT")
#undef SIGNERSCORE_W9X_SUSPECT
    inline constexpr auto SIGNERSCORE_W9X_SUSPECT =
#pragma pop_macro("SIGNERSCORE_W9X_SUSPECT")
      SIGNERSCORE_W9X_SUSPECT;
#pragma push_macro("SIGNERSCORE_UNSIGNED")
#undef SIGNERSCORE_UNSIGNED
    inline constexpr auto SIGNERSCORE_UNSIGNED =
#pragma pop_macro("SIGNERSCORE_UNSIGNED")
      SIGNERSCORE_UNSIGNED;
#pragma push_macro("SIGNERSCORE_AUTHENTICODE")
#undef SIGNERSCORE_AUTHENTICODE
    inline constexpr auto SIGNERSCORE_AUTHENTICODE =
#pragma pop_macro("SIGNERSCORE_AUTHENTICODE")
      SIGNERSCORE_AUTHENTICODE;
#pragma push_macro("SIGNERSCORE_WHQL")
#undef SIGNERSCORE_WHQL
    inline constexpr auto SIGNERSCORE_WHQL =
#pragma pop_macro("SIGNERSCORE_WHQL")
      SIGNERSCORE_WHQL;
#pragma push_macro("SIGNERSCORE_UNCLASSIFIED")
#undef SIGNERSCORE_UNCLASSIFIED
    inline constexpr auto SIGNERSCORE_UNCLASSIFIED =
#pragma pop_macro("SIGNERSCORE_UNCLASSIFIED")
      SIGNERSCORE_UNCLASSIFIED;
#pragma push_macro("SIGNERSCORE_INBOX")
#undef SIGNERSCORE_INBOX
    inline constexpr auto SIGNERSCORE_INBOX =
#pragma pop_macro("SIGNERSCORE_INBOX")
      SIGNERSCORE_INBOX;
#pragma push_macro("SIGNERSCORE_LOGO_STANDARD")
#undef SIGNERSCORE_LOGO_STANDARD
    inline constexpr auto SIGNERSCORE_LOGO_STANDARD =
#pragma pop_macro("SIGNERSCORE_LOGO_STANDARD")
      SIGNERSCORE_LOGO_STANDARD;
#pragma push_macro("SIGNERSCORE_LOGO_PREMIUM")
#undef SIGNERSCORE_LOGO_PREMIUM
    inline constexpr auto SIGNERSCORE_LOGO_PREMIUM =
#pragma pop_macro("SIGNERSCORE_LOGO_PREMIUM")
      SIGNERSCORE_LOGO_PREMIUM;

#pragma push_macro("SIGNERSCORE_MASK")
#undef SIGNERSCORE_MASK
    inline constexpr auto SIGNERSCORE_MASK =
#pragma pop_macro("SIGNERSCORE_MASK")
      SIGNERSCORE_MASK;
#pragma push_macro("SIGNERSCORE_SIGNED_MASK")
#undef SIGNERSCORE_SIGNED_MASK
    inline constexpr auto SIGNERSCORE_SIGNED_MASK =
#pragma pop_macro("SIGNERSCORE_SIGNED_MASK")
      SIGNERSCORE_SIGNED_MASK;

#pragma push_macro("DICUSTOMDEVPROP_MERGE_MULTISZ")
#undef DICUSTOMDEVPROP_MERGE_MULTISZ
    inline constexpr auto DICUSTOMDEVPROP_MERGE_MULTISZ =
#pragma pop_macro("DICUSTOMDEVPROP_MERGE_MULTISZ")
      DICUSTOMDEVPROP_MERGE_MULTISZ;

#pragma push_macro("SCWMI_CLOBBER_SECURITY")
#undef SCWMI_CLOBBER_SECURITY
    inline constexpr auto SCWMI_CLOBBER_SECURITY =
#pragma pop_macro("SCWMI_CLOBBER_SECURITY")
      SCWMI_CLOBBER_SECURITY;
} // namespace win32
