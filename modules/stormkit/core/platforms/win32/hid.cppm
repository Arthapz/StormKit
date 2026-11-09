module;

#include <stormkit/core/platform_macro.hpp>

#include <SDKDDKVer.h>
#include <winapifamily.h>

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

#include <hidsdi.h>

#include <hidpi.h>

export module stormkit.platform.win32.hid;

export namespace win32 {
    using ::HIDD_ATTRIBUTES;
    using ::HIDP_BUTTON_ARRAY_DATA;
    using ::HIDP_BUTTON_CAPS;
    using ::HIDP_CAPS;
    using ::HIDP_DATA;
    using ::HIDP_EXTENDED_ATTRIBUTES;
    using ::HIDP_REPORT_TYPE;
    using ::HIDP_UNKNOWN_TOKEN;
    using ::HIDP_VALUE_CAPS;
    using ::USAGE_AND_PAGE;

    using ::HidD_GetAttributes;
    using ::HidD_GetHidGuid;
    using ::HidD_GetIndexedString;
    using ::HidD_GetInputReport;
    using ::HidD_GetManufacturerString;
    using ::HidD_GetNumInputBuffers;
    using ::HidD_GetPhysicalDescriptor;
    using ::HidD_GetPreparsedData;
    using ::HidD_GetProductString;
    using ::HidD_GetSerialNumberString;
    using ::HidD_SetFeature;
    using ::HidD_SetNumInputBuffers;
    using ::HidD_SetOutputReport;
    using ::HidP_GetButtonArray;
    using ::HidP_GetButtonCaps;
    using ::HidP_GetCaps;
    using ::HidP_GetData;
    using ::HidP_GetExtendedAttributes;
    using ::HidP_GetLinkCollectionNodes;
    using ::HidP_GetScaledUsageValue;
    using ::HidP_GetSpecificButtonCaps;
    using ::HidP_GetSpecificValueCaps;
    using ::HidP_GetUsages;
    using ::HidP_GetUsagesEx;
    using ::HidP_GetUsageValue;
    using ::HidP_GetUsageValueArray;
    using ::HidP_GetValueCaps;
    using ::HidP_GetVersion;
    using ::HidP_InitializeReportForID;
    using ::HidP_MaxDataListLength;
    using ::HidP_MaxUsageListLength;
    using ::HidP_SetButtonArray;
    using ::HidP_SetData;
    using ::HidP_SetScaledUsageValue;
    using ::HidP_SetUsages;
    using ::HidP_SetUsageValue;
    using ::HidP_SetUsageValueArray;
    using ::HidP_TranslateUsagesToI8042ScanCodes;
    using ::HidP_UnsetUsages;
    using ::HidP_UsageAndPageListDifference;
    using ::HidP_UsageListDifference;

#pragma push_macro("HidP_GetButtons")
#undef HidP_GetButtons
    auto HidP_GetButtons(HIDP_REPORT_TYPE     report_type,
                         USAGE                usage,
                         USHORT               link_collection,
                         PUSAGE               usage_list,
                         PULONG               usage_length,
                         PHIDP_PREPARSED_DATA preparsed_data,
                         PCHAR                report,
                         ULONG                report_length) noexcept -> NTSTATUS;
#pragma push_macro("HidP_GetButtonsEx")
#undef HidP_GetButtonsEx
    auto HidP_GetButtonsEx(HIDP_REPORT_TYPE     report_type,
                           USHORT               link_collection,
                           PUSAGE_AND_PAGE      button_list,
                           PULONG               usage_length,
                           PHIDP_PREPARSED_DATA preparsed_data,
                           PCHAR                report,
                           ULONG                report_length) noexcept -> NTSTATUS;
#pragma push_macro("HidP_SetButtons")
#undef HidP_SetButtons
    auto HidP_SetButtons(HIDP_REPORT_TYPE     report_type,
                         USAGE                usage,
                         USHORT               link_collection,
                         PUSAGE               usage_list,
                         PULONG               usage_length,
                         PHIDP_PREPARSED_DATA preparsed_data,
                         PCHAR                report,
                         ULONG                report_length) noexcept -> NTSTATUS;
#pragma push_macro("HidP_UnsetButtons")
#undef HidP_UnsetButtons
    auto HidP_UnsetButtons(HIDP_REPORT_TYPE     report_type,
                           USAGE                usage,
                           USHORT               link_collection,
                           PUSAGE               usage_list,
                           PULONG               usage_length,
                           PHIDP_PREPARSED_DATA preparsed_data,
                           PCHAR                report,
                           ULONG                report_length) noexcept -> NTSTATUS;
} // namespace win32

////////////////////////////////////////////////////////////////////
///                      IMPLEMENTATION                          ///
////////////////////////////////////////////////////////////////////

namespace win32 {
    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto HidP_GetButtons(HIDP_REPORT_TYPE     report_type,
                                USAGE                usage,
                                USHORT               link_collection,
                                PUSAGE               usage_list,
                                PULONG               usage_length,
                                PHIDP_PREPARSED_DATA preparsed_data,
                                PCHAR                report,
                                ULONG                report_length) noexcept -> NTSTATUS {
#pragma pop_macro("HidP_GetButtons")
        return HidP_GetButtons(report_type,
                               usage,
                               link_collection,
                               usage_list,
                               usage_length,
                               preparsed_data,
                               report,
                               report_length);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto HidP_GetButtonsEx(HIDP_REPORT_TYPE     report_type,
                                  USHORT               link_collection,
                                  PUSAGE_AND_PAGE      button_list,
                                  PULONG               usage_length,
                                  PHIDP_PREPARSED_DATA preparsed_data,
                                  PCHAR                report,
                                  ULONG                report_length) noexcept -> NTSTATUS {
#pragma pop_macro("HidP_GetButtonsEx")
        return HidP_GetButtonsEx(report_type, link_collection, button_list, usage_length, preparsed_data, report, report_length);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto HidP_SetButtons(HIDP_REPORT_TYPE     report_type,
                                USAGE                usage,
                                USHORT               link_collection,
                                PUSAGE               usage_list,
                                PULONG               usage_length,
                                PHIDP_PREPARSED_DATA preparsed_data,
                                PCHAR                report,
                                ULONG                report_length) noexcept -> NTSTATUS {
#pragma pop_macro("HidP_SetButtons")
        return HidP_SetButtons(report_type,
                               usage,
                               link_collection,
                               usage_list,
                               usage_length,
                               preparsed_data,
                               report,
                               report_length);
    }

    /////////////////////////////////////
    /////////////////////////////////////
    STORMKIT_FORCE_INLINE
    inline auto HidP_UnsetButtons(HIDP_REPORT_TYPE     report_type,
                                  USAGE                usage,
                                  USHORT               link_collection,
                                  PUSAGE               usage_list,
                                  PULONG               usage_length,
                                  PHIDP_PREPARSED_DATA preparsed_data,
                                  PCHAR                report,
                                  ULONG                report_length) noexcept -> NTSTATUS {
#pragma pop_macro("HidP_UnsetButtons")
        return HidP_UnsetButtons(report_type,
                                 usage,
                                 link_collection,
                                 usage_list,
                                 usage_length,
                                 preparsed_data,
                                 report,
                                 report_length);
    }
} // namespace win32
