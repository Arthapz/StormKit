

module;

#include <stormkit/core/platform_macro.hpp>

#include <SDKDDKVer.h>
#include <winapifamily.h>

#include <Windows.h>

#include <cfgmgr32.h>

export module stormkit.platform.win32.cfgmgr32;

export namespace win32 {
    using ::BUSNUMBER_DES;
    using ::BUSNUMBER_RANGE;
    using ::BUSNUMBER_RESOURCE;
    using ::CM_NOTIFY_ACTION;
    using ::CM_NOTIFY_EVENT_DATA;
    using ::CM_NOTIFY_FILTER;
    using ::CONFLICT_DETAILS_A;
    using ::CONFLICT_DETAILS_W;
    using ::CS_DES;
    using ::CS_RESOURCE;
    using ::DMA_DES;
    using ::DMA_RANGE;
    using ::DMA_RESOURCE;
    using ::IO_DES;
    using ::IO_RANGE;
    using ::IO_RESOURCE;
    using ::IRQ_DES_32;
    using ::IRQ_DES_64;
    using ::IRQ_RANGE;
    using ::IRQ_RESOURCE_32;
    using ::IRQ_RESOURCE_64;
    using ::MEM_DES;
    using ::MEM_RANGE;
    using ::MEM_RESOURCE;
    using ::MFCARD_DES;
    using ::MFCARD_RESOURCE;
    using ::PCCARD_DES;
    using ::PCCARD_RESOURCE;

    using ::CM_Add_Empty_Log_Conf;
    using ::CM_Add_Empty_Log_Conf_Ex;
    using ::CM_Add_ID_ExA;
    using ::CM_Add_ID_ExW;
    using ::CM_Add_IDA;
    using ::CM_Add_IDW;
    using ::CM_Add_Range;
    using ::CM_Add_Res_Des;
    using ::CM_Add_Res_Des_Ex;
    using ::CM_Connect_MachineA;
    using ::CM_Connect_MachineW;
    using ::CM_Create_DevNode_ExA;
    using ::CM_Create_DevNode_ExW;
    using ::CM_Create_DevNodeA;
    using ::CM_Create_DevNodeW;
    using ::CM_Create_Range_List;
    using ::CM_Delete_Class_Key;
    using ::CM_Delete_Class_Key_Ex;
    using ::CM_Delete_Device_Interface_Key_ExA;
    using ::CM_Delete_Device_Interface_Key_ExW;
    using ::CM_Delete_Device_Interface_KeyA;
    using ::CM_Delete_Device_Interface_KeyW;
    using ::CM_Delete_DevNode_Key;
    using ::CM_Delete_DevNode_Key_Ex;
    using ::CM_Delete_Range;
    using ::CM_Detect_Resource_Conflict;
    using ::CM_Detect_Resource_Conflict_Ex;
    using ::CM_Disable_DevNode;
    using ::CM_Disable_DevNode_Ex;
    using ::CM_Disconnect_Machine;
    using ::CM_Dup_Range_List;
    using ::CM_Enable_DevNode;
    using ::CM_Enable_DevNode_Ex;
    using ::CM_Enumerate_Classes;
    using ::CM_Enumerate_Classes_Ex;
    using ::CM_Enumerate_Enumerators_ExA;
    using ::CM_Enumerate_Enumerators_ExW;
    using ::CM_Enumerate_EnumeratorsA;
    using ::CM_Enumerate_EnumeratorsW;
    using ::CM_Find_Range;
    using ::CM_First_Range;
    using ::CM_Free_Log_Conf;
    using ::CM_Free_Log_Conf_Ex;
    using ::CM_Free_Log_Conf_Handle;
    using ::CM_Free_Range_List;
    using ::CM_Free_Res_Des;
    using ::CM_Free_Res_Des_Ex;
    using ::CM_Free_Res_Des_Handle;
    using ::CM_Free_Resource_Conflict_Handle;
    using ::CM_Get_Child;
    using ::CM_Get_Child_Ex;
    using ::CM_Get_Class_Key_Name_ExA;
    using ::CM_Get_Class_Key_Name_ExW;
    using ::CM_Get_Class_Key_NameA;
    using ::CM_Get_Class_Key_NameW;
    using ::CM_Get_Class_Name_ExA;
    using ::CM_Get_Class_Name_ExW;
    using ::CM_Get_Class_NameA;
    using ::CM_Get_Class_NameW;
    using ::CM_Get_Class_Property_ExW;
    using ::CM_Get_Class_Property_Keys;
    using ::CM_Get_Class_Property_Keys_Ex;
    using ::CM_Get_Class_PropertyW;
    using ::CM_Get_Class_Registry_PropertyA;
    using ::CM_Get_Class_Registry_PropertyW;
    using ::CM_Get_Depth;
    using ::CM_Get_Depth_Ex;
    using ::CM_Get_Device_ID_ExA;
    using ::CM_Get_Device_ID_ExW;
    using ::CM_Get_Device_ID_List_ExA;
    using ::CM_Get_Device_ID_List_ExW;
    using ::CM_Get_Device_ID_List_Size_ExA;
    using ::CM_Get_Device_ID_List_Size_ExW;
    using ::CM_Get_Device_ID_List_SizeA;
    using ::CM_Get_Device_ID_List_SizeW;
    using ::CM_Get_Device_ID_ListA;
    using ::CM_Get_Device_ID_ListW;
    using ::CM_Get_Device_ID_Size;
    using ::CM_Get_Device_ID_Size_Ex;
    using ::CM_Get_Device_IDA;
    using ::CM_Get_Device_IDW;
    using ::CM_Get_Device_Interface_Alias_ExA;
    using ::CM_Get_Device_Interface_Alias_ExW;
    using ::CM_Get_Device_Interface_AliasA;
    using ::CM_Get_Device_Interface_AliasW;
    using ::CM_Get_Device_Interface_List_ExA;
    using ::CM_Get_Device_Interface_List_ExW;
    using ::CM_Get_Device_Interface_List_Size_ExA;
    using ::CM_Get_Device_Interface_List_Size_ExW;
    using ::CM_Get_Device_Interface_List_SizeA;
    using ::CM_Get_Device_Interface_List_SizeW;
    using ::CM_Get_Device_Interface_ListA;
    using ::CM_Get_Device_Interface_ListW;
    using ::CM_Get_Device_Interface_Property_ExW;
    using ::CM_Get_Device_Interface_Property_Keys_ExW;
    using ::CM_Get_Device_Interface_Property_KeysW;
    using ::CM_Get_Device_Interface_PropertyW;
    using ::CM_Get_DevNode_Custom_Property_ExA;
    using ::CM_Get_DevNode_Custom_Property_ExW;
    using ::CM_Get_DevNode_Custom_PropertyA;
    using ::CM_Get_DevNode_Custom_PropertyW;
    using ::CM_Get_DevNode_Property_ExW;
    using ::CM_Get_DevNode_Property_Keys;
    using ::CM_Get_DevNode_Property_Keys_Ex;
    using ::CM_Get_DevNode_PropertyW;
    using ::CM_Get_DevNode_Registry_Property_ExA;
    using ::CM_Get_DevNode_Registry_Property_ExW;
    using ::CM_Get_DevNode_Registry_PropertyA;
    using ::CM_Get_DevNode_Registry_PropertyW;
    using ::CM_Get_DevNode_Status;
    using ::CM_Get_DevNode_Status_Ex;
    using ::CM_Get_First_Log_Conf;
    using ::CM_Get_First_Log_Conf_Ex;
    using ::CM_Get_Global_State;
    using ::CM_Get_Global_State_Ex;
    using ::CM_Get_Hardware_Profile_Info_ExA;
    using ::CM_Get_Hardware_Profile_Info_ExW;
    using ::CM_Get_Hardware_Profile_InfoA;
    using ::CM_Get_Hardware_Profile_InfoW;
    using ::CM_Get_HW_Prof_Flags_ExA;
    using ::CM_Get_HW_Prof_Flags_ExW;
    using ::CM_Get_HW_Prof_FlagsA;
    using ::CM_Get_HW_Prof_FlagsW;
    using ::CM_Get_Log_Conf_Priority;
    using ::CM_Get_Log_Conf_Priority_Ex;
    using ::CM_Get_Next_Log_Conf;
    using ::CM_Get_Next_Log_Conf_Ex;
    using ::CM_Get_Next_Res_Des;
    using ::CM_Get_Next_Res_Des_Ex;
    using ::CM_Get_Parent;
    using ::CM_Get_Parent_Ex;
    using ::CM_Get_Res_Des_Data;
    using ::CM_Get_Res_Des_Data_Ex;
    using ::CM_Get_Res_Des_Data_Size;
    using ::CM_Get_Res_Des_Data_Size_Ex;
    using ::CM_Get_Resource_Conflict_Count;
    using ::CM_Get_Resource_Conflict_DetailsA;
    using ::CM_Get_Resource_Conflict_DetailsW;
    using ::CM_Get_Sibling;
    using ::CM_Get_Sibling_Ex;
    using ::CM_Get_Version;
    using ::CM_Get_Version_Ex;
    using ::CM_Intersect_Range_List;
    using ::CM_Invert_Range_List;
    using ::CM_Is_Dock_Station_Present;
    using ::CM_Is_Dock_Station_Present_Ex;
    using ::CM_Is_Version_Available;
    using ::CM_Is_Version_Available_Ex;
    using ::CM_Locate_DevNode_ExA;
    using ::CM_Locate_DevNode_ExW;
    using ::CM_Locate_DevNodeA;
    using ::CM_Locate_DevNodeW;
    using ::CM_MapCrToWin32Err;
    using ::CM_Merge_Range_List;
    using ::CM_Modify_Res_Des;
    using ::CM_Modify_Res_Des_Ex;
    using ::CM_Move_DevNode;
    using ::CM_Move_DevNode_Ex;
    using ::CM_Next_Range;
    using ::CM_Open_Class_Key_ExA;
    using ::CM_Open_Class_Key_ExW;
    using ::CM_Open_Class_KeyA;
    using ::CM_Open_Class_KeyW;
    using ::CM_Open_Device_Interface_Key_ExA;
    using ::CM_Open_Device_Interface_Key_ExW;
    using ::CM_Open_Device_Interface_KeyA;
    using ::CM_Open_Device_Interface_KeyW;
    using ::CM_Open_DevNode_Key;
    using ::CM_Open_DevNode_Key_Ex;
    using ::CM_Query_And_Remove_SubTree_ExA;
    using ::CM_Query_And_Remove_SubTree_ExW;
    using ::CM_Query_And_Remove_SubTreeA;
    using ::CM_Query_And_Remove_SubTreeW;
    using ::CM_Query_Arbitrator_Free_Data;
    using ::CM_Query_Arbitrator_Free_Data_Ex;
    using ::CM_Query_Arbitrator_Free_Size;
    using ::CM_Query_Arbitrator_Free_Size_Ex;
    using ::CM_Query_Remove_SubTree;
    using ::CM_Query_Remove_SubTree_Ex;
    using ::CM_Query_Resource_Conflict_List;
    using ::CM_Reenumerate_DevNode;
    using ::CM_Reenumerate_DevNode_Ex;
    using ::CM_Register_Device_Driver;
    using ::CM_Register_Device_Driver_Ex;
    using ::CM_Register_Device_Interface_ExA;
    using ::CM_Register_Device_Interface_ExW;
    using ::CM_Register_Device_InterfaceA;
    using ::CM_Register_Device_InterfaceW;
    using ::CM_Register_Notification;
    using ::CM_Remove_SubTree;
    using ::CM_Remove_SubTree_Ex;
    using ::CM_Request_Device_Eject_ExA;
    using ::CM_Request_Device_Eject_ExW;
    using ::CM_Request_Device_EjectA;
    using ::CM_Request_Device_EjectW;
    using ::CM_Request_Eject_PC;
    using ::CM_Request_Eject_PC_Ex;
    using ::CM_Run_Detection;
    using ::CM_Run_Detection_Ex;
    using ::CM_Set_Class_Property_ExW;
    using ::CM_Set_Class_PropertyW;
    using ::CM_Set_Class_Registry_PropertyA;
    using ::CM_Set_Class_Registry_PropertyW;
    using ::CM_Set_Device_Interface_Property_ExW;
    using ::CM_Set_Device_Interface_PropertyW;
    using ::CM_Set_DevNode_Problem;
    using ::CM_Set_DevNode_Problem_Ex;
    using ::CM_Set_DevNode_Property_ExW;
    using ::CM_Set_DevNode_PropertyW;
    using ::CM_Set_DevNode_Registry_Property_ExA;
    using ::CM_Set_DevNode_Registry_Property_ExW;
    using ::CM_Set_DevNode_Registry_PropertyA;
    using ::CM_Set_DevNode_Registry_PropertyW;
    using ::CM_Set_HW_Prof;
    using ::CM_Set_HW_Prof_Ex;
    using ::CM_Set_HW_Prof_Flags_ExA;
    using ::CM_Set_HW_Prof_Flags_ExW;
    using ::CM_Set_HW_Prof_FlagsA;
    using ::CM_Set_HW_Prof_FlagsW;
    using ::CM_Setup_DevNode;
    using ::CM_Setup_DevNode_Ex;
    using ::CM_Test_Range_Available;
    using ::CM_Uninstall_DevNode;
    using ::CM_Uninstall_DevNode_Ex;
    using ::CM_Unregister_Device_Interface_ExA;
    using ::CM_Unregister_Device_Interface_ExW;
    using ::CM_Unregister_Device_InterfaceA;
    using ::CM_Unregister_Device_InterfaceW;
    using ::CM_Unregister_Notification;
    using ::CM_WaitNoPendingInstallEvents;

#pragma push_macro("CM_CDMASK_DEVINST")
#undef CM_CDMASK_DEVINST
    inline constexpr auto CM_CDMASK_DEVINST =
#pragma pop_macro("CM_CDMASK_DEVINST")
      CM_CDMASK_DEVINST;
#pragma push_macro("CM_CDMASK_RESDES")
#undef CM_CDMASK_RESDES
    inline constexpr auto CM_CDMASK_RESDES =
#pragma pop_macro("CM_CDMASK_RESDES")
      CM_CDMASK_RESDES;
#pragma push_macro("CM_CDMASK_FLAGS")
#undef CM_CDMASK_FLAGS
    inline constexpr auto CM_CDMASK_FLAGS =
#pragma pop_macro("CM_CDMASK_FLAGS")
      CM_CDMASK_FLAGS;
#pragma push_macro("CM_CDMASK_DESCRIPTION")
#undef CM_CDMASK_DESCRIPTION
    inline constexpr auto CM_CDMASK_DESCRIPTION =
#pragma pop_macro("CM_CDMASK_DESCRIPTION")
      CM_CDMASK_DESCRIPTION;
#pragma push_macro("CM_CDMASK_VALID")
#undef CM_CDMASK_VALID
    inline constexpr auto CM_CDMASK_VALID =
#pragma pop_macro("CM_CDMASK_VALID")
      CM_CDMASK_VALID;

#pragma push_macro("CM_CDFLAGS_DRIVER")
#undef CM_CDFLAGS_DRIVER
    inline constexpr auto CM_CDFLAGS_DRIVER =
#pragma pop_macro("CM_CDFLAGS_DRIVER")
      CM_CDFLAGS_DRIVER;
#pragma push_macro("CM_CDFLAGS_ROOT_OWNED")
#undef CM_CDFLAGS_ROOT_OWNED
    inline constexpr auto CM_CDFLAGS_ROOT_OWNED =
#pragma pop_macro("CM_CDFLAGS_ROOT_OWNED")
      CM_CDFLAGS_ROOT_OWNED;
#pragma push_macro("CM_CDFLAGS_RESERVED")
#undef CM_CDFLAGS_RESERVED
    inline constexpr auto CM_CDFLAGS_RESERVED =
#pragma pop_macro("CM_CDFLAGS_RESERVED")
      CM_CDFLAGS_RESERVED;

#pragma push_macro("mMD_MemoryType")
#undef mMD_MemoryType
    inline constexpr auto mMD_MemoryType =
#pragma pop_macro("mMD_MemoryType")
      mMD_MemoryType;
#pragma push_macro("fMD_MemoryType")
#undef fMD_MemoryType
    inline constexpr auto fMD_MemoryType =
#pragma pop_macro("fMD_MemoryType")
      fMD_MemoryType;
#pragma push_macro("fMD_ROM")
#undef fMD_ROM
    inline constexpr auto fMD_ROM =
#pragma pop_macro("fMD_ROM")
      fMD_ROM;
#pragma push_macro("fMD_RAM")
#undef fMD_RAM
    inline constexpr auto fMD_RAM =
#pragma pop_macro("fMD_RAM")
      fMD_RAM;

#pragma push_macro("mMD_32_24")
#undef mMD_32_24
    inline constexpr auto mMD_32_24 =
#pragma pop_macro("mMD_32_24")
      mMD_32_24;
#pragma push_macro("fMD_32_24")
#undef fMD_32_24
    inline constexpr auto fMD_32_24 =
#pragma pop_macro("fMD_32_24")
      fMD_32_24;
#pragma push_macro("fMD_24")
#undef fMD_24
    inline constexpr auto fMD_24 =
#pragma pop_macro("fMD_24")
      fMD_24;
#pragma push_macro("fMD_32")
#undef fMD_32
    inline constexpr auto fMD_32 =
#pragma pop_macro("fMD_32")
      fMD_32;

#pragma push_macro("mMD_Prefetchable")
#undef mMD_Prefetchable
    inline constexpr auto mMD_Prefetchable =
#pragma pop_macro("mMD_Prefetchable")
      mMD_Prefetchable;
#pragma push_macro("fMD_Prefetchable")
#undef fMD_Prefetchable
    inline constexpr auto fMD_Prefetchable =
#pragma pop_macro("fMD_Prefetchable")
      fMD_Prefetchable;
#pragma push_macro("fMD_Pref")
#undef fMD_Pref
    inline constexpr auto fMD_Pref =
#pragma pop_macro("fMD_Pref")
      fMD_Pref;
#pragma push_macro("fMD_PrefetchDisallowed")
#undef fMD_PrefetchDisallowed
    inline constexpr auto fMD_PrefetchDisallowed =
#pragma pop_macro("fMD_PrefetchDisallowed")
      fMD_PrefetchDisallowed;
#pragma push_macro("fMD_PrefetchAllowed")
#undef fMD_PrefetchAllowed
    inline constexpr auto fMD_PrefetchAllowed =
#pragma pop_macro("fMD_PrefetchAllowed")
      fMD_PrefetchAllowed;

#pragma push_macro("mMD_Readable")
#undef mMD_Readable
    inline constexpr auto mMD_Readable =
#pragma pop_macro("mMD_Readable")
      mMD_Readable;
#pragma push_macro("fMD_Readable")
#undef fMD_Readable
    inline constexpr auto fMD_Readable =
#pragma pop_macro("fMD_Readable")
      fMD_Readable;
#pragma push_macro("fMD_ReadAllowed")
#undef fMD_ReadAllowed
    inline constexpr auto fMD_ReadAllowed =
#pragma pop_macro("fMD_ReadAllowed")
      fMD_ReadAllowed;
#pragma push_macro("fMD_ReadDisallowed")
#undef fMD_ReadDisallowed
    inline constexpr auto fMD_ReadDisallowed =
#pragma pop_macro("fMD_ReadDisallowed")
      fMD_ReadDisallowed;

#pragma push_macro("mMD_CombinedWrite")
#undef mMD_CombinedWrite
    inline constexpr auto mMD_CombinedWrite =
#pragma pop_macro("mMD_CombinedWrite")
      mMD_CombinedWrite;
#pragma push_macro("fMD_CombinedWrite")
#undef fMD_CombinedWrite
    inline constexpr auto fMD_CombinedWrite =
#pragma pop_macro("fMD_CombinedWrite")
      fMD_CombinedWrite;
#pragma push_macro("fMD_CombinedWriteDisallowed")
#undef fMD_CombinedWriteDisallowed
    inline constexpr auto fMD_CombinedWriteDisallowed =
#pragma pop_macro("fMD_CombinedWriteDisallowed")
      fMD_CombinedWriteDisallowed;
#pragma push_macro("fMD_CombinedWriteAllowed")
#undef fMD_CombinedWriteAllowed
    inline constexpr auto fMD_CombinedWriteAllowed =
#pragma pop_macro("fMD_CombinedWriteAllowed")
      fMD_CombinedWriteAllowed;

#pragma push_macro("mMD_Cacheable")
#undef mMD_Cacheable
    inline constexpr auto mMD_Cacheable =
#pragma pop_macro("mMD_Cacheable")
      mMD_Cacheable;
#pragma push_macro("fMD_NonCacheable")
#undef fMD_NonCacheable
    inline constexpr auto fMD_NonCacheable =
#pragma pop_macro("fMD_NonCacheable")
      fMD_NonCacheable;
#pragma push_macro("fMD_Cacheable")
#undef fMD_Cacheable
    inline constexpr auto fMD_Cacheable =
#pragma pop_macro("fMD_Cacheable")
      fMD_Cacheable;

#pragma push_macro("fMD_WINDOW_DECODE")
#undef fMD_WINDOW_DECODE
    inline constexpr auto fMD_WINDOW_DECODE =
#pragma pop_macro("fMD_WINDOW_DECODE")
      fMD_WINDOW_DECODE;
#pragma push_macro("fMD_MEMORY_BAR")
#undef fMD_MEMORY_BAR
    inline constexpr auto fMD_MEMORY_BAR =
#pragma pop_macro("fMD_MEMORY_BAR")
      fMD_MEMORY_BAR;

#pragma push_macro("MType_Range")
#undef MType_Range
    inline constexpr auto MType_Range =
#pragma pop_macro("MType_Range")
      MType_Range;

#pragma push_macro("fIOD_PortType")
#undef fIOD_PortType
    inline constexpr auto fIOD_PortType =
#pragma pop_macro("fIOD_PortType")
      fIOD_PortType;
#pragma push_macro("fIOD_Memory")
#undef fIOD_Memory
    inline constexpr auto fIOD_Memory =
#pragma pop_macro("fIOD_Memory")
      fIOD_Memory;
#pragma push_macro("fIOD_IO")
#undef fIOD_IO
    inline constexpr auto fIOD_IO =
#pragma pop_macro("fIOD_IO")
      fIOD_IO;
#pragma push_macro("fIOD_DECODE")
#undef fIOD_DECODE
    inline constexpr auto fIOD_DECODE =
#pragma pop_macro("fIOD_DECODE")
      fIOD_DECODE;
#pragma push_macro("fIOD_10_BIT_DECODE")
#undef fIOD_10_BIT_DECODE
    inline constexpr auto fIOD_10_BIT_DECODE =
#pragma pop_macro("fIOD_10_BIT_DECODE")
      fIOD_10_BIT_DECODE;
#pragma push_macro("fIOD_12_BIT_DECODE")
#undef fIOD_12_BIT_DECODE
    inline constexpr auto fIOD_12_BIT_DECODE =
#pragma pop_macro("fIOD_12_BIT_DECODE")
      fIOD_12_BIT_DECODE;
#pragma push_macro("fIOD_16_BIT_DECODE")
#undef fIOD_16_BIT_DECODE
    inline constexpr auto fIOD_16_BIT_DECODE =
#pragma pop_macro("fIOD_16_BIT_DECODE")
      fIOD_16_BIT_DECODE;
#pragma push_macro("fIOD_POSITIVE_DECODE")
#undef fIOD_POSITIVE_DECODE
    inline constexpr auto fIOD_POSITIVE_DECODE =
#pragma pop_macro("fIOD_POSITIVE_DECODE")
      fIOD_POSITIVE_DECODE;
#pragma push_macro("fIOD_PASSIVE_DECODE")
#undef fIOD_PASSIVE_DECODE
    inline constexpr auto fIOD_PASSIVE_DECODE =
#pragma pop_macro("fIOD_PASSIVE_DECODE")
      fIOD_PASSIVE_DECODE;
#pragma push_macro("fIOD_WINDOW_DECODE")
#undef fIOD_WINDOW_DECODE
    inline constexpr auto fIOD_WINDOW_DECODE =
#pragma pop_macro("fIOD_WINDOW_DECODE")
      fIOD_WINDOW_DECODE;
#pragma push_macro("fIOD_PORT_BAR")
#undef fIOD_PORT_BAR
    inline constexpr auto fIOD_PORT_BAR =
#pragma pop_macro("fIOD_PORT_BAR")
      fIOD_PORT_BAR;

#pragma push_macro("IOA_Local")
#undef IOA_Local
    inline constexpr auto IOA_Local =
#pragma pop_macro("IOA_Local")
      IOA_Local;

#pragma push_macro("mDD_Width")
#undef mDD_Width
    inline constexpr auto mDD_Width =
#pragma pop_macro("mDD_Width")
      mDD_Width;
#pragma push_macro("fDD_BYTE")
#undef fDD_BYTE
    inline constexpr auto fDD_BYTE =
#pragma pop_macro("fDD_BYTE")
      fDD_BYTE;
#pragma push_macro("fDD_WORD")
#undef fDD_WORD
    inline constexpr auto fDD_WORD =
#pragma pop_macro("fDD_WORD")
      fDD_WORD;
#pragma push_macro("fDD_DWORD")
#undef fDD_DWORD
    inline constexpr auto fDD_DWORD =
#pragma pop_macro("fDD_DWORD")
      fDD_DWORD;
#pragma push_macro("fDD_BYTE_AND_WORD")
#undef fDD_BYTE_AND_WORD
    inline constexpr auto fDD_BYTE_AND_WORD =
#pragma pop_macro("fDD_BYTE_AND_WORD")
      fDD_BYTE_AND_WORD;

#pragma push_macro("mDD_BusMaster")
#undef mDD_BusMaster
    inline constexpr auto mDD_BusMaster =
#pragma pop_macro("mDD_BusMaster")
      mDD_BusMaster;
#pragma push_macro("fDD_NoBusMaster")
#undef fDD_NoBusMaster
    inline constexpr auto fDD_NoBusMaster =
#pragma pop_macro("fDD_NoBusMaster")
      fDD_NoBusMaster;
#pragma push_macro("fDD_BusMaster")
#undef fDD_BusMaster
    inline constexpr auto fDD_BusMaster =
#pragma pop_macro("fDD_BusMaster")
      fDD_BusMaster;

#pragma push_macro("mDD_Type")
#undef mDD_Type
    inline constexpr auto mDD_Type =
#pragma pop_macro("mDD_Type")
      mDD_Type;
#pragma push_macro("fDD_TypeStandard")
#undef fDD_TypeStandard
    inline constexpr auto fDD_TypeStandard =
#pragma pop_macro("fDD_TypeStandard")
      fDD_TypeStandard;
#pragma push_macro("fDD_TypeA")
#undef fDD_TypeA
    inline constexpr auto fDD_TypeA =
#pragma pop_macro("fDD_TypeA")
      fDD_TypeA;
#pragma push_macro("fDD_TypeB")
#undef fDD_TypeB
    inline constexpr auto fDD_TypeB =
#pragma pop_macro("fDD_TypeB")
      fDD_TypeB;
#pragma push_macro("fDD_TypeF")
#undef fDD_TypeF
    inline constexpr auto fDD_TypeF =
#pragma pop_macro("fDD_TypeF")
      fDD_TypeF;

#pragma push_macro("mIRQD_Share")
#undef mIRQD_Share
    inline constexpr auto mIRQD_Share =
#pragma pop_macro("mIRQD_Share")
      mIRQD_Share;
#pragma push_macro("fIRQD_Exclusive")
#undef fIRQD_Exclusive
    inline constexpr auto fIRQD_Exclusive =
#pragma pop_macro("fIRQD_Exclusive")
      fIRQD_Exclusive;
#pragma push_macro("fIRQD_Share")
#undef fIRQD_Share
    inline constexpr auto fIRQD_Share =
#pragma pop_macro("fIRQD_Share")
      fIRQD_Share;

#pragma push_macro("mIRQD_Edge_Level")
#undef mIRQD_Edge_Level
    inline constexpr auto mIRQD_Edge_Level =
#pragma pop_macro("mIRQD_Edge_Level")
      mIRQD_Edge_Level;
#pragma push_macro("fIRQD_Level")
#undef fIRQD_Level
    inline constexpr auto fIRQD_Level =
#pragma pop_macro("fIRQD_Level")
      fIRQD_Level;
#pragma push_macro("fIRQD_Edge")
#undef fIRQD_Edge
    inline constexpr auto fIRQD_Edge =
#pragma pop_macro("fIRQD_Edge")
      fIRQD_Edge;

#pragma push_macro("CM_RESDES_WIDTH_DEFAULT")
#undef CM_RESDES_WIDTH_DEFAULT
    inline constexpr auto CM_RESDES_WIDTH_DEFAULT =
#pragma pop_macro("CM_RESDES_WIDTH_DEFAULT")
      CM_RESDES_WIDTH_DEFAULT;
#pragma push_macro("CM_RESDES_WIDTH_32")
#undef CM_RESDES_WIDTH_32
    inline constexpr auto CM_RESDES_WIDTH_32 =
#pragma pop_macro("CM_RESDES_WIDTH_32")
      CM_RESDES_WIDTH_32;
#pragma push_macro("CM_RESDES_WIDTH_64")
#undef CM_RESDES_WIDTH_64
    inline constexpr auto CM_RESDES_WIDTH_64 =
#pragma pop_macro("CM_RESDES_WIDTH_64")
      CM_RESDES_WIDTH_64;
#pragma push_macro("CM_RESDES_WIDTH_BITS")
#undef CM_RESDES_WIDTH_BITS
    inline constexpr auto CM_RESDES_WIDTH_BITS =
#pragma pop_macro("CM_RESDES_WIDTH_BITS")
      CM_RESDES_WIDTH_BITS;

#pragma push_macro("PType_Range")
#undef PType_Range
    inline constexpr auto PType_Range =
#pragma pop_macro("PType_Range")
      PType_Range;

#pragma push_macro("mPCD_IO_8_16")
#undef mPCD_IO_8_16
    inline constexpr auto mPCD_IO_8_16 =
#pragma pop_macro("mPCD_IO_8_16")
      mPCD_IO_8_16;
#pragma push_macro("fPCD_IO_8")
#undef fPCD_IO_8
    inline constexpr auto fPCD_IO_8 =
#pragma pop_macro("fPCD_IO_8")
      fPCD_IO_8;
#pragma push_macro("fPCD_IO_16")
#undef fPCD_IO_16
    inline constexpr auto fPCD_IO_16 =
#pragma pop_macro("fPCD_IO_16")
      fPCD_IO_16;
#pragma push_macro("mPCD_MEM_8_16")
#undef mPCD_MEM_8_16
    inline constexpr auto mPCD_MEM_8_16 =
#pragma pop_macro("mPCD_MEM_8_16")
      mPCD_MEM_8_16;
#pragma push_macro("fPCD_MEM_8")
#undef fPCD_MEM_8
    inline constexpr auto fPCD_MEM_8 =
#pragma pop_macro("fPCD_MEM_8")
      fPCD_MEM_8;
#pragma push_macro("fPCD_MEM_16")
#undef fPCD_MEM_16
    inline constexpr auto fPCD_MEM_16 =
#pragma pop_macro("fPCD_MEM_16")
      fPCD_MEM_16;
#pragma push_macro("mPCD_MEM_A_C")
#undef mPCD_MEM_A_C
    inline constexpr auto mPCD_MEM_A_C =
#pragma pop_macro("mPCD_MEM_A_C")
      mPCD_MEM_A_C;
#pragma push_macro("fPCD_MEM1_A")
#undef fPCD_MEM1_A
    inline constexpr auto fPCD_MEM1_A =
#pragma pop_macro("fPCD_MEM1_A")
      fPCD_MEM1_A;
#pragma push_macro("fPCD_MEM2_A")
#undef fPCD_MEM2_A
    inline constexpr auto fPCD_MEM2_A =
#pragma pop_macro("fPCD_MEM2_A")
      fPCD_MEM2_A;
#pragma push_macro("fPCD_IO_ZW_8")
#undef fPCD_IO_ZW_8
    inline constexpr auto fPCD_IO_ZW_8 =
#pragma pop_macro("fPCD_IO_ZW_8")
      fPCD_IO_ZW_8;
#pragma push_macro("fPCD_IO_SRC_16")
#undef fPCD_IO_SRC_16
    inline constexpr auto fPCD_IO_SRC_16 =
#pragma pop_macro("fPCD_IO_SRC_16")
      fPCD_IO_SRC_16;
#pragma push_macro("fPCD_IO_WS_16")
#undef fPCD_IO_WS_16
    inline constexpr auto fPCD_IO_WS_16 =
#pragma pop_macro("fPCD_IO_WS_16")
      fPCD_IO_WS_16;
#pragma push_macro("mPCD_MEM_WS")
#undef mPCD_MEM_WS
    inline constexpr auto mPCD_MEM_WS =
#pragma pop_macro("mPCD_MEM_WS")
      mPCD_MEM_WS;
#pragma push_macro("fPCD_MEM_WS_ONE")
#undef fPCD_MEM_WS_ONE
    inline constexpr auto fPCD_MEM_WS_ONE =
#pragma pop_macro("fPCD_MEM_WS_ONE")
      fPCD_MEM_WS_ONE;
#pragma push_macro("fPCD_MEM_WS_TWO")
#undef fPCD_MEM_WS_TWO
    inline constexpr auto fPCD_MEM_WS_TWO =
#pragma pop_macro("fPCD_MEM_WS_TWO")
      fPCD_MEM_WS_TWO;
#pragma push_macro("fPCD_MEM_WS_THREE")
#undef fPCD_MEM_WS_THREE
    inline constexpr auto fPCD_MEM_WS_THREE =
#pragma pop_macro("fPCD_MEM_WS_THREE")
      fPCD_MEM_WS_THREE;

#pragma push_macro("fPCD_MEM_A")
#undef fPCD_MEM_A
    inline constexpr auto fPCD_MEM_A =
#pragma pop_macro("fPCD_MEM_A")
      fPCD_MEM_A;

#pragma push_macro("fPCD_ATTRIBUTES_PER_WINDOW")
#undef fPCD_ATTRIBUTES_PER_WINDOW
    inline constexpr auto fPCD_ATTRIBUTES_PER_WINDOW =
#pragma pop_macro("fPCD_ATTRIBUTES_PER_WINDOW")
      fPCD_ATTRIBUTES_PER_WINDOW;

#pragma push_macro("fPCD_IO1_16")
#undef fPCD_IO1_16
    inline constexpr auto fPCD_IO1_16 =
#pragma pop_macro("fPCD_IO1_16")
      fPCD_IO1_16;
#pragma push_macro("fPCD_IO1_ZW_8")
#undef fPCD_IO1_ZW_8
    inline constexpr auto fPCD_IO1_ZW_8 =
#pragma pop_macro("fPCD_IO1_ZW_8")
      fPCD_IO1_ZW_8;
#pragma push_macro("fPCD_IO1_SRC_16")
#undef fPCD_IO1_SRC_16
    inline constexpr auto fPCD_IO1_SRC_16 =
#pragma pop_macro("fPCD_IO1_SRC_16")
      fPCD_IO1_SRC_16;
#pragma push_macro("fPCD_IO1_WS_16")
#undef fPCD_IO1_WS_16
    inline constexpr auto fPCD_IO1_WS_16 =
#pragma pop_macro("fPCD_IO1_WS_16")
      fPCD_IO1_WS_16;

#pragma push_macro("fPCD_IO2_16")
#undef fPCD_IO2_16
    inline constexpr auto fPCD_IO2_16 =
#pragma pop_macro("fPCD_IO2_16")
      fPCD_IO2_16;
#pragma push_macro("fPCD_IO2_ZW_8")
#undef fPCD_IO2_ZW_8
    inline constexpr auto fPCD_IO2_ZW_8 =
#pragma pop_macro("fPCD_IO2_ZW_8")
      fPCD_IO2_ZW_8;
#pragma push_macro("fPCD_IO2_SRC_16")
#undef fPCD_IO2_SRC_16
    inline constexpr auto fPCD_IO2_SRC_16 =
#pragma pop_macro("fPCD_IO2_SRC_16")
      fPCD_IO2_SRC_16;
#pragma push_macro("fPCD_IO2_WS_16")
#undef fPCD_IO2_WS_16
    inline constexpr auto fPCD_IO2_WS_16 =
#pragma pop_macro("fPCD_IO2_WS_16")
      fPCD_IO2_WS_16;

#pragma push_macro("mPCD_MEM1_WS")
#undef mPCD_MEM1_WS
    inline constexpr auto mPCD_MEM1_WS =
#pragma pop_macro("mPCD_MEM1_WS")
      mPCD_MEM1_WS;
#pragma push_macro("fPCD_MEM1_WS_ONE")
#undef fPCD_MEM1_WS_ONE
    inline constexpr auto fPCD_MEM1_WS_ONE =
#pragma pop_macro("fPCD_MEM1_WS_ONE")
      fPCD_MEM1_WS_ONE;
#pragma push_macro("fPCD_MEM1_WS_TWO")
#undef fPCD_MEM1_WS_TWO
    inline constexpr auto fPCD_MEM1_WS_TWO =
#pragma pop_macro("fPCD_MEM1_WS_TWO")
      fPCD_MEM1_WS_TWO;
#pragma push_macro("fPCD_MEM1_WS_THREE")
#undef fPCD_MEM1_WS_THREE
    inline constexpr auto fPCD_MEM1_WS_THREE =
#pragma pop_macro("fPCD_MEM1_WS_THREE")
      fPCD_MEM1_WS_THREE;
#pragma push_macro("fPCD_MEM1_16")
#undef fPCD_MEM1_16
    inline constexpr auto fPCD_MEM1_16 =
#pragma pop_macro("fPCD_MEM1_16")
      fPCD_MEM1_16;

#pragma push_macro("mPCD_MEM2_WS")
#undef mPCD_MEM2_WS
    inline constexpr auto mPCD_MEM2_WS =
#pragma pop_macro("mPCD_MEM2_WS")
      mPCD_MEM2_WS;
#pragma push_macro("fPCD_MEM2_WS_ONE")
#undef fPCD_MEM2_WS_ONE
    inline constexpr auto fPCD_MEM2_WS_ONE =
#pragma pop_macro("fPCD_MEM2_WS_ONE")
      fPCD_MEM2_WS_ONE;
#pragma push_macro("fPCD_MEM2_WS_TWO")
#undef fPCD_MEM2_WS_TWO
    inline constexpr auto fPCD_MEM2_WS_TWO =
#pragma pop_macro("fPCD_MEM2_WS_TWO")
      fPCD_MEM2_WS_TWO;
#pragma push_macro("fPCD_MEM2_WS_THREE")
#undef fPCD_MEM2_WS_THREE
    inline constexpr auto fPCD_MEM2_WS_THREE =
#pragma pop_macro("fPCD_MEM2_WS_THREE")
      fPCD_MEM2_WS_THREE;
#pragma push_macro("fPCD_MEM2_16")
#undef fPCD_MEM2_16
    inline constexpr auto fPCD_MEM2_16 =
#pragma pop_macro("fPCD_MEM2_16")
      fPCD_MEM2_16;

#pragma push_macro("PCD_MAX_MEMORY")
#undef PCD_MAX_MEMORY
    inline constexpr auto PCD_MAX_MEMORY =
#pragma pop_macro("PCD_MAX_MEMORY")
      PCD_MAX_MEMORY;
#pragma push_macro("PCD_MAX_IO")
#undef PCD_MAX_IO
    inline constexpr auto PCD_MAX_IO =
#pragma pop_macro("PCD_MAX_IO")
      PCD_MAX_IO;

#pragma push_macro("mPMF_AUDIO_ENABLE")
#undef mPMF_AUDIO_ENABLE
    inline constexpr auto mPMF_AUDIO_ENABLE =
#pragma pop_macro("mPMF_AUDIO_ENABLE")
      mPMF_AUDIO_ENABLE;
#pragma push_macro("fPMF_AUDIO_ENABLE")
#undef fPMF_AUDIO_ENABLE
    inline constexpr auto fPMF_AUDIO_ENABLE =
#pragma pop_macro("fPMF_AUDIO_ENABLE")
      fPMF_AUDIO_ENABLE;

#pragma push_macro("BusNumberType_Range")
#undef BusNumberType_Range
    inline constexpr auto BusNumberType_Range =
#pragma pop_macro("BusNumberType_Range")
      BusNumberType_Range;

#pragma push_macro("CM_HWPI_NOT_DOCKABLE")
#undef CM_HWPI_NOT_DOCKABLE
    inline constexpr auto CM_HWPI_NOT_DOCKABLE =
#pragma pop_macro("CM_HWPI_NOT_DOCKABLE")
      CM_HWPI_NOT_DOCKABLE;
#pragma push_macro("CM_HWPI_UNDOCKED")
#undef CM_HWPI_UNDOCKED
    inline constexpr auto CM_HWPI_UNDOCKED =
#pragma pop_macro("CM_HWPI_UNDOCKED")
      CM_HWPI_UNDOCKED;
#pragma push_macro("CM_HWPI_DOCKED")
#undef CM_HWPI_DOCKED
    inline constexpr auto CM_HWPI_DOCKED =
#pragma pop_macro("CM_HWPI_DOCKED")
      CM_HWPI_DOCKED;

#pragma push_macro("ResType_All")
#undef ResType_All
    inline constexpr auto ResType_All =
#pragma pop_macro("ResType_All")
      ResType_All;
#pragma push_macro("ResType_None")
#undef ResType_None
    inline constexpr auto ResType_None =
#pragma pop_macro("ResType_None")
      ResType_None;
#pragma push_macro("ResType_Mem")
#undef ResType_Mem
    inline constexpr auto ResType_Mem =
#pragma pop_macro("ResType_Mem")
      ResType_Mem;
#pragma push_macro("ResType_IO")
#undef ResType_IO
    inline constexpr auto ResType_IO =
#pragma pop_macro("ResType_IO")
      ResType_IO;
#pragma push_macro("ResType_DMA")
#undef ResType_DMA
    inline constexpr auto ResType_DMA =
#pragma pop_macro("ResType_DMA")
      ResType_DMA;
#pragma push_macro("ResType_IRQ")
#undef ResType_IRQ
    inline constexpr auto ResType_IRQ =
#pragma pop_macro("ResType_IRQ")
      ResType_IRQ;
#pragma push_macro("ResType_DoNotUse")
#undef ResType_DoNotUse
    inline constexpr auto ResType_DoNotUse =
#pragma pop_macro("ResType_DoNotUse")
      ResType_DoNotUse;
#pragma push_macro("ResType_BusNumber")
#undef ResType_BusNumber
    inline constexpr auto ResType_BusNumber =
#pragma pop_macro("ResType_BusNumber")
      ResType_BusNumber;
#pragma push_macro("ResType_MemLarge")
#undef ResType_MemLarge
    inline constexpr auto ResType_MemLarge =
#pragma pop_macro("ResType_MemLarge")
      ResType_MemLarge;
#pragma push_macro("ResType_MAX")
#undef ResType_MAX
    inline constexpr auto ResType_MAX =
#pragma pop_macro("ResType_MAX")
      ResType_MAX;

#pragma push_macro("ResType_Ignored_Bit")
#undef ResType_Ignored_Bit
    inline constexpr auto ResType_Ignored_Bit =
#pragma pop_macro("ResType_Ignored_Bit")
      ResType_Ignored_Bit;
#pragma push_macro("ResType_ClassSpecific")
#undef ResType_ClassSpecific
    inline constexpr auto ResType_ClassSpecific =
#pragma pop_macro("ResType_ClassSpecific")
      ResType_ClassSpecific;
#pragma push_macro("ResType_Reserved")
#undef ResType_Reserved
    inline constexpr auto ResType_Reserved =
#pragma pop_macro("ResType_Reserved")
      ResType_Reserved;
#pragma push_macro("ResType_DevicePrivate")
#undef ResType_DevicePrivate
    inline constexpr auto ResType_DevicePrivate =
#pragma pop_macro("ResType_DevicePrivate")
      ResType_DevicePrivate;
#pragma push_macro("ResType_PcCardConfig")
#undef ResType_PcCardConfig
    inline constexpr auto ResType_PcCardConfig =
#pragma pop_macro("ResType_PcCardConfig")
      ResType_PcCardConfig;
#pragma push_macro("ResType_MfCardConfig")
#undef ResType_MfCardConfig
    inline constexpr auto ResType_MfCardConfig =
#pragma pop_macro("ResType_MfCardConfig")
      ResType_MfCardConfig;
#pragma push_macro("ResType_Connection")
#undef ResType_Connection
    inline constexpr auto ResType_Connection =
#pragma pop_macro("ResType_Connection")
      ResType_Connection;

#pragma push_macro("CM_ADD_RANGE_ADDIFCONFLICT")
#undef CM_ADD_RANGE_ADDIFCONFLICT
    inline constexpr auto CM_ADD_RANGE_ADDIFCONFLICT =
#pragma pop_macro("CM_ADD_RANGE_ADDIFCONFLICT")
      CM_ADD_RANGE_ADDIFCONFLICT;
#pragma push_macro("CM_ADD_RANGE_DONOTADDIFCONFLICT")
#undef CM_ADD_RANGE_DONOTADDIFCONFLICT
    inline constexpr auto CM_ADD_RANGE_DONOTADDIFCONFLICT =
#pragma pop_macro("CM_ADD_RANGE_DONOTADDIFCONFLICT")
      CM_ADD_RANGE_DONOTADDIFCONFLICT;
#pragma push_macro("CM_ADD_RANGE_BITS")
#undef CM_ADD_RANGE_BITS
    inline constexpr auto CM_ADD_RANGE_BITS =
#pragma pop_macro("CM_ADD_RANGE_BITS")
      CM_ADD_RANGE_BITS;

#pragma push_macro("BASIC_LOG_CONF")
#undef BASIC_LOG_CONF
    inline constexpr auto BASIC_LOG_CONF =
#pragma pop_macro("BASIC_LOG_CONF")
      BASIC_LOG_CONF;
#pragma push_macro("FILTERED_LOG_CONF")
#undef FILTERED_LOG_CONF
    inline constexpr auto FILTERED_LOG_CONF =
#pragma pop_macro("FILTERED_LOG_CONF")
      FILTERED_LOG_CONF;
#pragma push_macro("ALLOC_LOG_CONF")
#undef ALLOC_LOG_CONF
    inline constexpr auto ALLOC_LOG_CONF =
#pragma pop_macro("ALLOC_LOG_CONF")
      ALLOC_LOG_CONF;
#pragma push_macro("BOOT_LOG_CONF")
#undef BOOT_LOG_CONF
    inline constexpr auto BOOT_LOG_CONF =
#pragma pop_macro("BOOT_LOG_CONF")
      BOOT_LOG_CONF;
#pragma push_macro("FORCED_LOG_CONF")
#undef FORCED_LOG_CONF
    inline constexpr auto FORCED_LOG_CONF =
#pragma pop_macro("FORCED_LOG_CONF")
      FORCED_LOG_CONF;
#pragma push_macro("OVERRIDE_LOG_CONF")
#undef OVERRIDE_LOG_CONF
    inline constexpr auto OVERRIDE_LOG_CONF =
#pragma pop_macro("OVERRIDE_LOG_CONF")
      OVERRIDE_LOG_CONF;
#pragma push_macro("NUM_LOG_CONF")
#undef NUM_LOG_CONF
    inline constexpr auto NUM_LOG_CONF =
#pragma pop_macro("NUM_LOG_CONF")
      NUM_LOG_CONF;
#pragma push_macro("LOG_CONF_BITS")
#undef LOG_CONF_BITS
    inline constexpr auto LOG_CONF_BITS =
#pragma pop_macro("LOG_CONF_BITS")
      LOG_CONF_BITS;

#pragma push_macro("PRIORITY_EQUAL_FIRST")
#undef PRIORITY_EQUAL_FIRST
    inline constexpr auto PRIORITY_EQUAL_FIRST =
#pragma pop_macro("PRIORITY_EQUAL_FIRST")
      PRIORITY_EQUAL_FIRST;
#pragma push_macro("PRIORITY_EQUAL_LAST")
#undef PRIORITY_EQUAL_LAST
    inline constexpr auto PRIORITY_EQUAL_LAST =
#pragma pop_macro("PRIORITY_EQUAL_LAST")
      PRIORITY_EQUAL_LAST;
#pragma push_macro("PRIORITY_BIT")
#undef PRIORITY_BIT
    inline constexpr auto PRIORITY_BIT =
#pragma pop_macro("PRIORITY_BIT")
      PRIORITY_BIT;

#pragma push_macro("RegDisposition_OpenAlways")
#undef RegDisposition_OpenAlways
    inline constexpr auto RegDisposition_OpenAlways =
#pragma pop_macro("RegDisposition_OpenAlways")
      RegDisposition_OpenAlways;
#pragma push_macro("RegDisposition_OpenExisting")
#undef RegDisposition_OpenExisting
    inline constexpr auto RegDisposition_OpenExisting =
#pragma pop_macro("RegDisposition_OpenExisting")
      RegDisposition_OpenExisting;
#pragma push_macro("RegDisposition_Bits")
#undef RegDisposition_Bits
    inline constexpr auto RegDisposition_Bits =
#pragma pop_macro("RegDisposition_Bits")
      RegDisposition_Bits;

#pragma push_macro("CM_ADD_ID_HARDWARE")
#undef CM_ADD_ID_HARDWARE
    inline constexpr auto CM_ADD_ID_HARDWARE =
#pragma pop_macro("CM_ADD_ID_HARDWARE")
      CM_ADD_ID_HARDWARE;
#pragma push_macro("CM_ADD_ID_COMPATIBLE")
#undef CM_ADD_ID_COMPATIBLE
    inline constexpr auto CM_ADD_ID_COMPATIBLE =
#pragma pop_macro("CM_ADD_ID_COMPATIBLE")
      CM_ADD_ID_COMPATIBLE;
#pragma push_macro("CM_ADD_ID_BITS")
#undef CM_ADD_ID_BITS
    inline constexpr auto CM_ADD_ID_BITS =
#pragma pop_macro("CM_ADD_ID_BITS")
      CM_ADD_ID_BITS;

#pragma push_macro("CM_CREATE_DEVNODE_NORMAL")
#undef CM_CREATE_DEVNODE_NORMAL
    inline constexpr auto CM_CREATE_DEVNODE_NORMAL =
#pragma pop_macro("CM_CREATE_DEVNODE_NORMAL")
      CM_CREATE_DEVNODE_NORMAL;
#pragma push_macro("CM_CREATE_DEVNODE_NO_WAIT_INSTALL")
#undef CM_CREATE_DEVNODE_NO_WAIT_INSTALL
    inline constexpr auto CM_CREATE_DEVNODE_NO_WAIT_INSTALL =
#pragma pop_macro("CM_CREATE_DEVNODE_NO_WAIT_INSTALL")
      CM_CREATE_DEVNODE_NO_WAIT_INSTALL;
#pragma push_macro("CM_CREATE_DEVNODE_PHANTOM")
#undef CM_CREATE_DEVNODE_PHANTOM
    inline constexpr auto CM_CREATE_DEVNODE_PHANTOM =
#pragma pop_macro("CM_CREATE_DEVNODE_PHANTOM")
      CM_CREATE_DEVNODE_PHANTOM;
#pragma push_macro("CM_CREATE_DEVNODE_GENERATE_ID")
#undef CM_CREATE_DEVNODE_GENERATE_ID
    inline constexpr auto CM_CREATE_DEVNODE_GENERATE_ID =
#pragma pop_macro("CM_CREATE_DEVNODE_GENERATE_ID")
      CM_CREATE_DEVNODE_GENERATE_ID;
#pragma push_macro("CM_CREATE_DEVNODE_DO_NOT_INSTALL")
#undef CM_CREATE_DEVNODE_DO_NOT_INSTALL
    inline constexpr auto CM_CREATE_DEVNODE_DO_NOT_INSTALL =
#pragma pop_macro("CM_CREATE_DEVNODE_DO_NOT_INSTALL")
      CM_CREATE_DEVNODE_DO_NOT_INSTALL;
#pragma push_macro("CM_CREATE_DEVNODE_BITS")
#undef CM_CREATE_DEVNODE_BITS
    inline constexpr auto CM_CREATE_DEVNODE_BITS =
#pragma pop_macro("CM_CREATE_DEVNODE_BITS")
      CM_CREATE_DEVNODE_BITS;

#pragma push_macro("CM_CREATE_DEVINST_NORMAL")
#undef CM_CREATE_DEVINST_NORMAL
    inline constexpr auto CM_CREATE_DEVINST_NORMAL =
#pragma pop_macro("CM_CREATE_DEVINST_NORMAL")
      CM_CREATE_DEVINST_NORMAL;
#pragma push_macro("CM_CREATE_DEVINST_NO_WAIT_INSTALL")
#undef CM_CREATE_DEVINST_NO_WAIT_INSTALL
    inline constexpr auto CM_CREATE_DEVINST_NO_WAIT_INSTALL =
#pragma pop_macro("CM_CREATE_DEVINST_NO_WAIT_INSTALL")
      CM_CREATE_DEVINST_NO_WAIT_INSTALL;
#pragma push_macro("CM_CREATE_DEVINST_PHANTOM")
#undef CM_CREATE_DEVINST_PHANTOM
    inline constexpr auto CM_CREATE_DEVINST_PHANTOM =
#pragma pop_macro("CM_CREATE_DEVINST_PHANTOM")
      CM_CREATE_DEVINST_PHANTOM;
#pragma push_macro("CM_CREATE_DEVINST_GENERATE_ID")
#undef CM_CREATE_DEVINST_GENERATE_ID
    inline constexpr auto CM_CREATE_DEVINST_GENERATE_ID =
#pragma pop_macro("CM_CREATE_DEVINST_GENERATE_ID")
      CM_CREATE_DEVINST_GENERATE_ID;
#pragma push_macro("CM_CREATE_DEVINST_DO_NOT_INSTALL")
#undef CM_CREATE_DEVINST_DO_NOT_INSTALL
    inline constexpr auto CM_CREATE_DEVINST_DO_NOT_INSTALL =
#pragma pop_macro("CM_CREATE_DEVINST_DO_NOT_INSTALL")
      CM_CREATE_DEVINST_DO_NOT_INSTALL;
#pragma push_macro("CM_CREATE_DEVINST_BITS")
#undef CM_CREATE_DEVINST_BITS
    inline constexpr auto CM_CREATE_DEVINST_BITS =
#pragma pop_macro("CM_CREATE_DEVINST_BITS")
      CM_CREATE_DEVINST_BITS;

#pragma push_macro("CM_DELETE_CLASS_ONLY")
#undef CM_DELETE_CLASS_ONLY
    inline constexpr auto CM_DELETE_CLASS_ONLY =
#pragma pop_macro("CM_DELETE_CLASS_ONLY")
      CM_DELETE_CLASS_ONLY;
#pragma push_macro("CM_DELETE_CLASS_SUBKEYS")
#undef CM_DELETE_CLASS_SUBKEYS
    inline constexpr auto CM_DELETE_CLASS_SUBKEYS =
#pragma pop_macro("CM_DELETE_CLASS_SUBKEYS")
      CM_DELETE_CLASS_SUBKEYS;
#pragma push_macro("CM_DELETE_CLASS_INTERFACE")
#undef CM_DELETE_CLASS_INTERFACE
    inline constexpr auto CM_DELETE_CLASS_INTERFACE =
#pragma pop_macro("CM_DELETE_CLASS_INTERFACE")
      CM_DELETE_CLASS_INTERFACE;
#pragma push_macro("CM_DELETE_CLASS_BITS")
#undef CM_DELETE_CLASS_BITS
    inline constexpr auto CM_DELETE_CLASS_BITS =
#pragma pop_macro("CM_DELETE_CLASS_BITS")
      CM_DELETE_CLASS_BITS;

#pragma push_macro("CM_ENUMERATE_CLASSES_INSTALLER")
#undef CM_ENUMERATE_CLASSES_INSTALLER
    inline constexpr auto CM_ENUMERATE_CLASSES_INSTALLER =
#pragma pop_macro("CM_ENUMERATE_CLASSES_INSTALLER")
      CM_ENUMERATE_CLASSES_INSTALLER;
#pragma push_macro("CM_ENUMERATE_CLASSES_INTERFACE")
#undef CM_ENUMERATE_CLASSES_INTERFACE
    inline constexpr auto CM_ENUMERATE_CLASSES_INTERFACE =
#pragma pop_macro("CM_ENUMERATE_CLASSES_INTERFACE")
      CM_ENUMERATE_CLASSES_INTERFACE;
#pragma push_macro("CM_ENUMERATE_CLASSES_BITS")
#undef CM_ENUMERATE_CLASSES_BITS
    inline constexpr auto CM_ENUMERATE_CLASSES_BITS =
#pragma pop_macro("CM_ENUMERATE_CLASSES_BITS")
      CM_ENUMERATE_CLASSES_BITS;

#pragma push_macro("CM_DETECT_NEW_PROFILE")
#undef CM_DETECT_NEW_PROFILE
    inline constexpr auto CM_DETECT_NEW_PROFILE =
#pragma pop_macro("CM_DETECT_NEW_PROFILE")
      CM_DETECT_NEW_PROFILE;
#pragma push_macro("CM_DETECT_CRASHED")
#undef CM_DETECT_CRASHED
    inline constexpr auto CM_DETECT_CRASHED =
#pragma pop_macro("CM_DETECT_CRASHED")
      CM_DETECT_CRASHED;
#pragma push_macro("CM_DETECT_HWPROF_FIRST_BOOT")
#undef CM_DETECT_HWPROF_FIRST_BOOT
    inline constexpr auto CM_DETECT_HWPROF_FIRST_BOOT =
#pragma pop_macro("CM_DETECT_HWPROF_FIRST_BOOT")
      CM_DETECT_HWPROF_FIRST_BOOT;
#pragma push_macro("CM_DETECT_RUN")
#undef CM_DETECT_RUN
    inline constexpr auto CM_DETECT_RUN =
#pragma pop_macro("CM_DETECT_RUN")
      CM_DETECT_RUN;
#pragma push_macro("CM_DETECT_BITS")
#undef CM_DETECT_BITS
    inline constexpr auto CM_DETECT_BITS =
#pragma pop_macro("CM_DETECT_BITS")
      CM_DETECT_BITS;

#pragma push_macro("CM_DISABLE_POLITE")
#undef CM_DISABLE_POLITE
    inline constexpr auto CM_DISABLE_POLITE =
#pragma pop_macro("CM_DISABLE_POLITE")
      CM_DISABLE_POLITE;
#pragma push_macro("CM_DISABLE_ABSOLUTE")
#undef CM_DISABLE_ABSOLUTE
    inline constexpr auto CM_DISABLE_ABSOLUTE =
#pragma pop_macro("CM_DISABLE_ABSOLUTE")
      CM_DISABLE_ABSOLUTE;
#pragma push_macro("CM_DISABLE_HARDWARE")
#undef CM_DISABLE_HARDWARE
    inline constexpr auto CM_DISABLE_HARDWARE =
#pragma pop_macro("CM_DISABLE_HARDWARE")
      CM_DISABLE_HARDWARE;
#pragma push_macro("CM_DISABLE_UI_NOT_OK")
#undef CM_DISABLE_UI_NOT_OK
    inline constexpr auto CM_DISABLE_UI_NOT_OK =
#pragma pop_macro("CM_DISABLE_UI_NOT_OK")
      CM_DISABLE_UI_NOT_OK;
#pragma push_macro("CM_DISABLE_PERSIST")
#undef CM_DISABLE_PERSIST
    inline constexpr auto CM_DISABLE_PERSIST =
#pragma pop_macro("CM_DISABLE_PERSIST")
      CM_DISABLE_PERSIST;
#pragma push_macro("CM_DISABLE_BITS")
#undef CM_DISABLE_BITS
    inline constexpr auto CM_DISABLE_BITS =
#pragma pop_macro("CM_DISABLE_BITS")
      CM_DISABLE_BITS;

#pragma push_macro("CM_GETIDLIST_FILTER_NONE")
#undef CM_GETIDLIST_FILTER_NONE
    inline constexpr auto CM_GETIDLIST_FILTER_NONE =
#pragma pop_macro("CM_GETIDLIST_FILTER_NONE")
      CM_GETIDLIST_FILTER_NONE;
#pragma push_macro("CM_GETIDLIST_FILTER_ENUMERATOR")
#undef CM_GETIDLIST_FILTER_ENUMERATOR
    inline constexpr auto CM_GETIDLIST_FILTER_ENUMERATOR =
#pragma pop_macro("CM_GETIDLIST_FILTER_ENUMERATOR")
      CM_GETIDLIST_FILTER_ENUMERATOR;
#pragma push_macro("CM_GETIDLIST_FILTER_SERVICE")
#undef CM_GETIDLIST_FILTER_SERVICE
    inline constexpr auto CM_GETIDLIST_FILTER_SERVICE =
#pragma pop_macro("CM_GETIDLIST_FILTER_SERVICE")
      CM_GETIDLIST_FILTER_SERVICE;
#pragma push_macro("CM_GETIDLIST_FILTER_EJECTRELATIONS")
#undef CM_GETIDLIST_FILTER_EJECTRELATIONS
    inline constexpr auto CM_GETIDLIST_FILTER_EJECTRELATIONS =
#pragma pop_macro("CM_GETIDLIST_FILTER_EJECTRELATIONS")
      CM_GETIDLIST_FILTER_EJECTRELATIONS;
#pragma push_macro("CM_GETIDLIST_FILTER_REMOVALRELATIONS")
#undef CM_GETIDLIST_FILTER_REMOVALRELATIONS
    inline constexpr auto CM_GETIDLIST_FILTER_REMOVALRELATIONS =
#pragma pop_macro("CM_GETIDLIST_FILTER_REMOVALRELATIONS")
      CM_GETIDLIST_FILTER_REMOVALRELATIONS;
#pragma push_macro("CM_GETIDLIST_FILTER_POWERRELATIONS")
#undef CM_GETIDLIST_FILTER_POWERRELATIONS
    inline constexpr auto CM_GETIDLIST_FILTER_POWERRELATIONS =
#pragma pop_macro("CM_GETIDLIST_FILTER_POWERRELATIONS")
      CM_GETIDLIST_FILTER_POWERRELATIONS;
#pragma push_macro("CM_GETIDLIST_FILTER_BUSRELATIONS")
#undef CM_GETIDLIST_FILTER_BUSRELATIONS
    inline constexpr auto CM_GETIDLIST_FILTER_BUSRELATIONS =
#pragma pop_macro("CM_GETIDLIST_FILTER_BUSRELATIONS")
      CM_GETIDLIST_FILTER_BUSRELATIONS;
#pragma push_macro("CM_GETIDLIST_DONOTGENERATE")
#undef CM_GETIDLIST_DONOTGENERATE
    inline constexpr auto CM_GETIDLIST_DONOTGENERATE =
#pragma pop_macro("CM_GETIDLIST_DONOTGENERATE")
      CM_GETIDLIST_DONOTGENERATE;

#pragma push_macro("CM_GETIDLIST_FILTER_TRANSPORTRELATIONS")
#undef CM_GETIDLIST_FILTER_TRANSPORTRELATIONS
    inline constexpr auto CM_GETIDLIST_FILTER_TRANSPORTRELATIONS =
#pragma pop_macro("CM_GETIDLIST_FILTER_TRANSPORTRELATIONS")
      CM_GETIDLIST_FILTER_TRANSPORTRELATIONS;
#pragma push_macro("CM_GETIDLIST_FILTER_PRESENT")
#undef CM_GETIDLIST_FILTER_PRESENT
    inline constexpr auto CM_GETIDLIST_FILTER_PRESENT =
#pragma pop_macro("CM_GETIDLIST_FILTER_PRESENT")
      CM_GETIDLIST_FILTER_PRESENT;
#pragma push_macro("CM_GETIDLIST_FILTER_CLASS")
#undef CM_GETIDLIST_FILTER_CLASS
    inline constexpr auto CM_GETIDLIST_FILTER_CLASS =
#pragma pop_macro("CM_GETIDLIST_FILTER_CLASS")
      CM_GETIDLIST_FILTER_CLASS;
#pragma push_macro("CM_GETIDLIST_FILTER_BITS")
#undef CM_GETIDLIST_FILTER_BITS
    inline constexpr auto CM_GETIDLIST_FILTER_BITS =
#pragma pop_macro("CM_GETIDLIST_FILTER_BITS")
      CM_GETIDLIST_FILTER_BITS;

#pragma push_macro("CM_GET_DEVICE_INTERFACE_LIST_PRESENT")
#undef CM_GET_DEVICE_INTERFACE_LIST_PRESENT
    inline constexpr auto CM_GET_DEVICE_INTERFACE_LIST_PRESENT =
#pragma pop_macro("CM_GET_DEVICE_INTERFACE_LIST_PRESENT")
      CM_GET_DEVICE_INTERFACE_LIST_PRESENT;
#pragma push_macro("CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES")
#undef CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES
    inline constexpr auto CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES =
#pragma pop_macro("CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES")
      CM_GET_DEVICE_INTERFACE_LIST_ALL_DEVICES;
#pragma push_macro("CM_GET_DEVICE_INTERFACE_LIST_BITS")
#undef CM_GET_DEVICE_INTERFACE_LIST_BITS
    inline constexpr auto CM_GET_DEVICE_INTERFACE_LIST_BITS =
#pragma pop_macro("CM_GET_DEVICE_INTERFACE_LIST_BITS")
      CM_GET_DEVICE_INTERFACE_LIST_BITS;

#pragma push_macro("CM_DRP_DEVICEDESC")
#undef CM_DRP_DEVICEDESC
    inline constexpr auto CM_DRP_DEVICEDESC =
#pragma pop_macro("CM_DRP_DEVICEDESC")
      CM_DRP_DEVICEDESC;
#pragma push_macro("CM_DRP_HARDWAREID")
#undef CM_DRP_HARDWAREID
    inline constexpr auto CM_DRP_HARDWAREID =
#pragma pop_macro("CM_DRP_HARDWAREID")
      CM_DRP_HARDWAREID;
#pragma push_macro("CM_DRP_COMPATIBLEIDS")
#undef CM_DRP_COMPATIBLEIDS
    inline constexpr auto CM_DRP_COMPATIBLEIDS =
#pragma pop_macro("CM_DRP_COMPATIBLEIDS")
      CM_DRP_COMPATIBLEIDS;
#pragma push_macro("CM_DRP_UNUSED0")
#undef CM_DRP_UNUSED0
    inline constexpr auto CM_DRP_UNUSED0 =
#pragma pop_macro("CM_DRP_UNUSED0")
      CM_DRP_UNUSED0;
#pragma push_macro("CM_DRP_SERVICE")
#undef CM_DRP_SERVICE
    inline constexpr auto CM_DRP_SERVICE =
#pragma pop_macro("CM_DRP_SERVICE")
      CM_DRP_SERVICE;
#pragma push_macro("CM_DRP_UNUSED1")
#undef CM_DRP_UNUSED1
    inline constexpr auto CM_DRP_UNUSED1 =
#pragma pop_macro("CM_DRP_UNUSED1")
      CM_DRP_UNUSED1;
#pragma push_macro("CM_DRP_UNUSED2")
#undef CM_DRP_UNUSED2
    inline constexpr auto CM_DRP_UNUSED2 =
#pragma pop_macro("CM_DRP_UNUSED2")
      CM_DRP_UNUSED2;
#pragma push_macro("CM_DRP_CLASS")
#undef CM_DRP_CLASS
    inline constexpr auto CM_DRP_CLASS =
#pragma pop_macro("CM_DRP_CLASS")
      CM_DRP_CLASS;
#pragma push_macro("CM_DRP_CLASSGUID")
#undef CM_DRP_CLASSGUID
    inline constexpr auto CM_DRP_CLASSGUID =
#pragma pop_macro("CM_DRP_CLASSGUID")
      CM_DRP_CLASSGUID;
#pragma push_macro("CM_DRP_DRIVER")
#undef CM_DRP_DRIVER
    inline constexpr auto CM_DRP_DRIVER =
#pragma pop_macro("CM_DRP_DRIVER")
      CM_DRP_DRIVER;
#pragma push_macro("CM_DRP_CONFIGFLAGS")
#undef CM_DRP_CONFIGFLAGS
    inline constexpr auto CM_DRP_CONFIGFLAGS =
#pragma pop_macro("CM_DRP_CONFIGFLAGS")
      CM_DRP_CONFIGFLAGS;
#pragma push_macro("CM_DRP_MFG")
#undef CM_DRP_MFG
    inline constexpr auto CM_DRP_MFG =
#pragma pop_macro("CM_DRP_MFG")
      CM_DRP_MFG;
#pragma push_macro("CM_DRP_FRIENDLYNAME")
#undef CM_DRP_FRIENDLYNAME
    inline constexpr auto CM_DRP_FRIENDLYNAME =
#pragma pop_macro("CM_DRP_FRIENDLYNAME")
      CM_DRP_FRIENDLYNAME;
#pragma push_macro("CM_DRP_LOCATION_INFORMATION")
#undef CM_DRP_LOCATION_INFORMATION
    inline constexpr auto CM_DRP_LOCATION_INFORMATION =
#pragma pop_macro("CM_DRP_LOCATION_INFORMATION")
      CM_DRP_LOCATION_INFORMATION;
#pragma push_macro("CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME")
#undef CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME
    inline constexpr auto CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME =
#pragma pop_macro("CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME")
      CM_DRP_PHYSICAL_DEVICE_OBJECT_NAME;
#pragma push_macro("CM_DRP_CAPABILITIES")
#undef CM_DRP_CAPABILITIES
    inline constexpr auto CM_DRP_CAPABILITIES =
#pragma pop_macro("CM_DRP_CAPABILITIES")
      CM_DRP_CAPABILITIES;
#pragma push_macro("CM_DRP_UI_NUMBER")
#undef CM_DRP_UI_NUMBER
    inline constexpr auto CM_DRP_UI_NUMBER =
#pragma pop_macro("CM_DRP_UI_NUMBER")
      CM_DRP_UI_NUMBER;
#pragma push_macro("CM_DRP_UPPERFILTERS")
#undef CM_DRP_UPPERFILTERS
    inline constexpr auto CM_DRP_UPPERFILTERS =
#pragma pop_macro("CM_DRP_UPPERFILTERS")
      CM_DRP_UPPERFILTERS;

#pragma push_macro("CM_CRP_UPPERFILTERS")
#undef CM_CRP_UPPERFILTERS
    inline constexpr auto CM_CRP_UPPERFILTERS =
#pragma pop_macro("CM_CRP_UPPERFILTERS")
      CM_CRP_UPPERFILTERS;
#pragma push_macro("CM_DRP_LOWERFILTERS")
#undef CM_DRP_LOWERFILTERS
    inline constexpr auto CM_DRP_LOWERFILTERS =
#pragma pop_macro("CM_DRP_LOWERFILTERS")
      CM_DRP_LOWERFILTERS;
#pragma push_macro("CM_CRP_LOWERFILTERS")
#undef CM_CRP_LOWERFILTERS
    inline constexpr auto CM_CRP_LOWERFILTERS =
#pragma pop_macro("CM_CRP_LOWERFILTERS")
      CM_CRP_LOWERFILTERS;

#pragma push_macro("CM_DRP_BUSTYPEGUID")
#undef CM_DRP_BUSTYPEGUID
    inline constexpr auto CM_DRP_BUSTYPEGUID =
#pragma pop_macro("CM_DRP_BUSTYPEGUID")
      CM_DRP_BUSTYPEGUID;
#pragma push_macro("CM_DRP_LEGACYBUSTYPE")
#undef CM_DRP_LEGACYBUSTYPE
    inline constexpr auto CM_DRP_LEGACYBUSTYPE =
#pragma pop_macro("CM_DRP_LEGACYBUSTYPE")
      CM_DRP_LEGACYBUSTYPE;
#pragma push_macro("CM_DRP_BUSNUMBER")
#undef CM_DRP_BUSNUMBER
    inline constexpr auto CM_DRP_BUSNUMBER =
#pragma pop_macro("CM_DRP_BUSNUMBER")
      CM_DRP_BUSNUMBER;
#pragma push_macro("CM_DRP_ENUMERATOR_NAME")
#undef CM_DRP_ENUMERATOR_NAME
    inline constexpr auto CM_DRP_ENUMERATOR_NAME =
#pragma pop_macro("CM_DRP_ENUMERATOR_NAME")
      CM_DRP_ENUMERATOR_NAME;
#pragma push_macro("CM_DRP_SECURITY")
#undef CM_DRP_SECURITY
    inline constexpr auto CM_DRP_SECURITY =
#pragma pop_macro("CM_DRP_SECURITY")
      CM_DRP_SECURITY;
#pragma push_macro("CM_CRP_SECURITY")
#undef CM_CRP_SECURITY
    inline constexpr auto CM_CRP_SECURITY =
#pragma pop_macro("CM_CRP_SECURITY")
      CM_CRP_SECURITY;
#pragma push_macro("CM_DRP_SECURITY_SDS")
#undef CM_DRP_SECURITY_SDS
    inline constexpr auto CM_DRP_SECURITY_SDS =
#pragma pop_macro("CM_DRP_SECURITY_SDS")
      CM_DRP_SECURITY_SDS;
#pragma push_macro("CM_CRP_SECURITY_SDS")
#undef CM_CRP_SECURITY_SDS
    inline constexpr auto CM_CRP_SECURITY_SDS =
#pragma pop_macro("CM_CRP_SECURITY_SDS")
      CM_CRP_SECURITY_SDS;
#pragma push_macro("CM_DRP_DEVTYPE")
#undef CM_DRP_DEVTYPE
    inline constexpr auto CM_DRP_DEVTYPE =
#pragma pop_macro("CM_DRP_DEVTYPE")
      CM_DRP_DEVTYPE;
#pragma push_macro("CM_CRP_DEVTYPE")
#undef CM_CRP_DEVTYPE
    inline constexpr auto CM_CRP_DEVTYPE =
#pragma pop_macro("CM_CRP_DEVTYPE")
      CM_CRP_DEVTYPE;
#pragma push_macro("CM_DRP_EXCLUSIVE")
#undef CM_DRP_EXCLUSIVE
    inline constexpr auto CM_DRP_EXCLUSIVE =
#pragma pop_macro("CM_DRP_EXCLUSIVE")
      CM_DRP_EXCLUSIVE;
#pragma push_macro("CM_CRP_EXCLUSIVE")
#undef CM_CRP_EXCLUSIVE
    inline constexpr auto CM_CRP_EXCLUSIVE =
#pragma pop_macro("CM_CRP_EXCLUSIVE")
      CM_CRP_EXCLUSIVE;
#pragma push_macro("CM_DRP_CHARACTERISTICS")
#undef CM_DRP_CHARACTERISTICS
    inline constexpr auto CM_DRP_CHARACTERISTICS =
#pragma pop_macro("CM_DRP_CHARACTERISTICS")
      CM_DRP_CHARACTERISTICS;
#pragma push_macro("CM_CRP_CHARACTERISTICS")
#undef CM_CRP_CHARACTERISTICS
    inline constexpr auto CM_CRP_CHARACTERISTICS =
#pragma pop_macro("CM_CRP_CHARACTERISTICS")
      CM_CRP_CHARACTERISTICS;
#pragma push_macro("CM_DRP_ADDRESS")
#undef CM_DRP_ADDRESS
    inline constexpr auto CM_DRP_ADDRESS =
#pragma pop_macro("CM_DRP_ADDRESS")
      CM_DRP_ADDRESS;
#pragma push_macro("CM_DRP_UI_NUMBER_DESC_FORMAT")
#undef CM_DRP_UI_NUMBER_DESC_FORMAT
    inline constexpr auto CM_DRP_UI_NUMBER_DESC_FORMAT =
#pragma pop_macro("CM_DRP_UI_NUMBER_DESC_FORMAT")
      CM_DRP_UI_NUMBER_DESC_FORMAT;

#pragma push_macro("CM_DRP_DEVICE_POWER_DATA")
#undef CM_DRP_DEVICE_POWER_DATA
    inline constexpr auto CM_DRP_DEVICE_POWER_DATA =
#pragma pop_macro("CM_DRP_DEVICE_POWER_DATA")
      CM_DRP_DEVICE_POWER_DATA;
#pragma push_macro("CM_DRP_REMOVAL_POLICY")
#undef CM_DRP_REMOVAL_POLICY
    inline constexpr auto CM_DRP_REMOVAL_POLICY =
#pragma pop_macro("CM_DRP_REMOVAL_POLICY")
      CM_DRP_REMOVAL_POLICY;
#pragma push_macro("CM_DRP_REMOVAL_POLICY_HW_DEFAULT")
#undef CM_DRP_REMOVAL_POLICY_HW_DEFAULT
    inline constexpr auto CM_DRP_REMOVAL_POLICY_HW_DEFAULT =
#pragma pop_macro("CM_DRP_REMOVAL_POLICY_HW_DEFAULT")
      CM_DRP_REMOVAL_POLICY_HW_DEFAULT;
#pragma push_macro("CM_DRP_REMOVAL_POLICY_OVERRIDE")
#undef CM_DRP_REMOVAL_POLICY_OVERRIDE
    inline constexpr auto CM_DRP_REMOVAL_POLICY_OVERRIDE =
#pragma pop_macro("CM_DRP_REMOVAL_POLICY_OVERRIDE")
      CM_DRP_REMOVAL_POLICY_OVERRIDE;
#pragma push_macro("CM_DRP_INSTALL_STATE")
#undef CM_DRP_INSTALL_STATE
    inline constexpr auto CM_DRP_INSTALL_STATE =
#pragma pop_macro("CM_DRP_INSTALL_STATE")
      CM_DRP_INSTALL_STATE;
#pragma push_macro("CM_DRP_LOCATION_PATHS")
#undef CM_DRP_LOCATION_PATHS
    inline constexpr auto CM_DRP_LOCATION_PATHS =
#pragma pop_macro("CM_DRP_LOCATION_PATHS")
      CM_DRP_LOCATION_PATHS;
#pragma push_macro("CM_DRP_BASE_CONTAINERID")
#undef CM_DRP_BASE_CONTAINERID
    inline constexpr auto CM_DRP_BASE_CONTAINERID =
#pragma pop_macro("CM_DRP_BASE_CONTAINERID")
      CM_DRP_BASE_CONTAINERID;

#pragma push_macro("CM_DRP_MIN")
#undef CM_DRP_MIN
    inline constexpr auto CM_DRP_MIN =
#pragma pop_macro("CM_DRP_MIN")
      CM_DRP_MIN;
#pragma push_macro("CM_CRP_MIN")
#undef CM_CRP_MIN
    inline constexpr auto CM_CRP_MIN =
#pragma pop_macro("CM_CRP_MIN")
      CM_CRP_MIN;
#pragma push_macro("CM_DRP_MAX")
#undef CM_DRP_MAX
    inline constexpr auto CM_DRP_MAX =
#pragma pop_macro("CM_DRP_MAX")
      CM_DRP_MAX;
#pragma push_macro("CM_CRP_MAX")
#undef CM_CRP_MAX
    inline constexpr auto CM_CRP_MAX =
#pragma pop_macro("CM_CRP_MAX")
      CM_CRP_MAX;

#pragma push_macro("CM_DEVCAP_LOCKSUPPORTED")
#undef CM_DEVCAP_LOCKSUPPORTED
    inline constexpr auto CM_DEVCAP_LOCKSUPPORTED =
#pragma pop_macro("CM_DEVCAP_LOCKSUPPORTED")
      CM_DEVCAP_LOCKSUPPORTED;
#pragma push_macro("CM_DEVCAP_EJECTSUPPORTED")
#undef CM_DEVCAP_EJECTSUPPORTED
    inline constexpr auto CM_DEVCAP_EJECTSUPPORTED =
#pragma pop_macro("CM_DEVCAP_EJECTSUPPORTED")
      CM_DEVCAP_EJECTSUPPORTED;
#pragma push_macro("CM_DEVCAP_REMOVABLE")
#undef CM_DEVCAP_REMOVABLE
    inline constexpr auto CM_DEVCAP_REMOVABLE =
#pragma pop_macro("CM_DEVCAP_REMOVABLE")
      CM_DEVCAP_REMOVABLE;
#pragma push_macro("CM_DEVCAP_DOCKDEVICE")
#undef CM_DEVCAP_DOCKDEVICE
    inline constexpr auto CM_DEVCAP_DOCKDEVICE =
#pragma pop_macro("CM_DEVCAP_DOCKDEVICE")
      CM_DEVCAP_DOCKDEVICE;
#pragma push_macro("CM_DEVCAP_UNIQUEID")
#undef CM_DEVCAP_UNIQUEID
    inline constexpr auto CM_DEVCAP_UNIQUEID =
#pragma pop_macro("CM_DEVCAP_UNIQUEID")
      CM_DEVCAP_UNIQUEID;
#pragma push_macro("CM_DEVCAP_SILENTINSTALL")
#undef CM_DEVCAP_SILENTINSTALL
    inline constexpr auto CM_DEVCAP_SILENTINSTALL =
#pragma pop_macro("CM_DEVCAP_SILENTINSTALL")
      CM_DEVCAP_SILENTINSTALL;
#pragma push_macro("CM_DEVCAP_RAWDEVICEOK")
#undef CM_DEVCAP_RAWDEVICEOK
    inline constexpr auto CM_DEVCAP_RAWDEVICEOK =
#pragma pop_macro("CM_DEVCAP_RAWDEVICEOK")
      CM_DEVCAP_RAWDEVICEOK;
#pragma push_macro("CM_DEVCAP_SURPRISEREMOVALOK")
#undef CM_DEVCAP_SURPRISEREMOVALOK
    inline constexpr auto CM_DEVCAP_SURPRISEREMOVALOK =
#pragma pop_macro("CM_DEVCAP_SURPRISEREMOVALOK")
      CM_DEVCAP_SURPRISEREMOVALOK;
#pragma push_macro("CM_DEVCAP_HARDWAREDISABLED")
#undef CM_DEVCAP_HARDWAREDISABLED
    inline constexpr auto CM_DEVCAP_HARDWAREDISABLED =
#pragma pop_macro("CM_DEVCAP_HARDWAREDISABLED")
      CM_DEVCAP_HARDWAREDISABLED;
#pragma push_macro("CM_DEVCAP_NONDYNAMIC")
#undef CM_DEVCAP_NONDYNAMIC
    inline constexpr auto CM_DEVCAP_NONDYNAMIC =
#pragma pop_macro("CM_DEVCAP_NONDYNAMIC")
      CM_DEVCAP_NONDYNAMIC;
#pragma push_macro("CM_DEVCAP_SECUREDEVICE")
#undef CM_DEVCAP_SECUREDEVICE
    inline constexpr auto CM_DEVCAP_SECUREDEVICE =
#pragma pop_macro("CM_DEVCAP_SECUREDEVICE")
      CM_DEVCAP_SECUREDEVICE;

#pragma push_macro("CM_REMOVAL_POLICY_EXPECT_NO_REMOVAL")
#undef CM_REMOVAL_POLICY_EXPECT_NO_REMOVAL
    inline constexpr auto CM_REMOVAL_POLICY_EXPECT_NO_REMOVAL =
#pragma pop_macro("CM_REMOVAL_POLICY_EXPECT_NO_REMOVAL")
      CM_REMOVAL_POLICY_EXPECT_NO_REMOVAL;
#pragma push_macro("CM_REMOVAL_POLICY_EXPECT_ORDERLY_REMOVAL")
#undef CM_REMOVAL_POLICY_EXPECT_ORDERLY_REMOVAL
    inline constexpr auto CM_REMOVAL_POLICY_EXPECT_ORDERLY_REMOVAL =
#pragma pop_macro("CM_REMOVAL_POLICY_EXPECT_ORDERLY_REMOVAL")
      CM_REMOVAL_POLICY_EXPECT_ORDERLY_REMOVAL;
#pragma push_macro("CM_REMOVAL_POLICY_EXPECT_SURPRISE_REMOVAL")
#undef CM_REMOVAL_POLICY_EXPECT_SURPRISE_REMOVAL
    inline constexpr auto CM_REMOVAL_POLICY_EXPECT_SURPRISE_REMOVAL =
#pragma pop_macro("CM_REMOVAL_POLICY_EXPECT_SURPRISE_REMOVAL")
      CM_REMOVAL_POLICY_EXPECT_SURPRISE_REMOVAL;

#pragma push_macro("CM_INSTALL_STATE_INSTALLED")
#undef CM_INSTALL_STATE_INSTALLED
    inline constexpr auto CM_INSTALL_STATE_INSTALLED =
#pragma pop_macro("CM_INSTALL_STATE_INSTALLED")
      CM_INSTALL_STATE_INSTALLED;
#pragma push_macro("CM_INSTALL_STATE_NEEDS_REINSTALL")
#undef CM_INSTALL_STATE_NEEDS_REINSTALL
    inline constexpr auto CM_INSTALL_STATE_NEEDS_REINSTALL =
#pragma pop_macro("CM_INSTALL_STATE_NEEDS_REINSTALL")
      CM_INSTALL_STATE_NEEDS_REINSTALL;
#pragma push_macro("CM_INSTALL_STATE_FAILED_INSTALL")
#undef CM_INSTALL_STATE_FAILED_INSTALL
    inline constexpr auto CM_INSTALL_STATE_FAILED_INSTALL =
#pragma pop_macro("CM_INSTALL_STATE_FAILED_INSTALL")
      CM_INSTALL_STATE_FAILED_INSTALL;
#pragma push_macro("CM_INSTALL_STATE_FINISH_INSTALL")
#undef CM_INSTALL_STATE_FINISH_INSTALL
    inline constexpr auto CM_INSTALL_STATE_FINISH_INSTALL =
#pragma pop_macro("CM_INSTALL_STATE_FINISH_INSTALL")
      CM_INSTALL_STATE_FINISH_INSTALL;

#pragma push_macro("CM_LOCATE_DEVNODE_NORMAL")
#undef CM_LOCATE_DEVNODE_NORMAL
    inline constexpr auto CM_LOCATE_DEVNODE_NORMAL =
#pragma pop_macro("CM_LOCATE_DEVNODE_NORMAL")
      CM_LOCATE_DEVNODE_NORMAL;
#pragma push_macro("CM_LOCATE_DEVNODE_PHANTOM")
#undef CM_LOCATE_DEVNODE_PHANTOM
    inline constexpr auto CM_LOCATE_DEVNODE_PHANTOM =
#pragma pop_macro("CM_LOCATE_DEVNODE_PHANTOM")
      CM_LOCATE_DEVNODE_PHANTOM;
#pragma push_macro("CM_LOCATE_DEVNODE_CANCELREMOVE")
#undef CM_LOCATE_DEVNODE_CANCELREMOVE
    inline constexpr auto CM_LOCATE_DEVNODE_CANCELREMOVE =
#pragma pop_macro("CM_LOCATE_DEVNODE_CANCELREMOVE")
      CM_LOCATE_DEVNODE_CANCELREMOVE;
#pragma push_macro("CM_LOCATE_DEVNODE_NOVALIDATION")
#undef CM_LOCATE_DEVNODE_NOVALIDATION
    inline constexpr auto CM_LOCATE_DEVNODE_NOVALIDATION =
#pragma pop_macro("CM_LOCATE_DEVNODE_NOVALIDATION")
      CM_LOCATE_DEVNODE_NOVALIDATION;
#pragma push_macro("CM_LOCATE_DEVNODE_BITS")
#undef CM_LOCATE_DEVNODE_BITS
    inline constexpr auto CM_LOCATE_DEVNODE_BITS =
#pragma pop_macro("CM_LOCATE_DEVNODE_BITS")
      CM_LOCATE_DEVNODE_BITS;

#pragma push_macro("CM_LOCATE_DEVINST_NORMAL")
#undef CM_LOCATE_DEVINST_NORMAL
    inline constexpr auto CM_LOCATE_DEVINST_NORMAL =
#pragma pop_macro("CM_LOCATE_DEVINST_NORMAL")
      CM_LOCATE_DEVINST_NORMAL;
#pragma push_macro("CM_LOCATE_DEVINST_PHANTOM")
#undef CM_LOCATE_DEVINST_PHANTOM
    inline constexpr auto CM_LOCATE_DEVINST_PHANTOM =
#pragma pop_macro("CM_LOCATE_DEVINST_PHANTOM")
      CM_LOCATE_DEVINST_PHANTOM;
#pragma push_macro("CM_LOCATE_DEVINST_CANCELREMOVE")
#undef CM_LOCATE_DEVINST_CANCELREMOVE
    inline constexpr auto CM_LOCATE_DEVINST_CANCELREMOVE =
#pragma pop_macro("CM_LOCATE_DEVINST_CANCELREMOVE")
      CM_LOCATE_DEVINST_CANCELREMOVE;
#pragma push_macro("CM_LOCATE_DEVINST_NOVALIDATION")
#undef CM_LOCATE_DEVINST_NOVALIDATION
    inline constexpr auto CM_LOCATE_DEVINST_NOVALIDATION =
#pragma pop_macro("CM_LOCATE_DEVINST_NOVALIDATION")
      CM_LOCATE_DEVINST_NOVALIDATION;
#pragma push_macro("CM_LOCATE_DEVINST_BITS")
#undef CM_LOCATE_DEVINST_BITS
    inline constexpr auto CM_LOCATE_DEVINST_BITS =
#pragma pop_macro("CM_LOCATE_DEVINST_BITS")
      CM_LOCATE_DEVINST_BITS;

#pragma push_macro("CM_OPEN_CLASS_KEY_INSTALLER")
#undef CM_OPEN_CLASS_KEY_INSTALLER
    inline constexpr auto CM_OPEN_CLASS_KEY_INSTALLER =
#pragma pop_macro("CM_OPEN_CLASS_KEY_INSTALLER")
      CM_OPEN_CLASS_KEY_INSTALLER;
#pragma push_macro("CM_OPEN_CLASS_KEY_INTERFACE")
#undef CM_OPEN_CLASS_KEY_INTERFACE
    inline constexpr auto CM_OPEN_CLASS_KEY_INTERFACE =
#pragma pop_macro("CM_OPEN_CLASS_KEY_INTERFACE")
      CM_OPEN_CLASS_KEY_INTERFACE;
#pragma push_macro("CM_OPEN_CLASS_KEY_BITS")
#undef CM_OPEN_CLASS_KEY_BITS
    inline constexpr auto CM_OPEN_CLASS_KEY_BITS =
#pragma pop_macro("CM_OPEN_CLASS_KEY_BITS")
      CM_OPEN_CLASS_KEY_BITS;

#pragma push_macro("CM_REMOVE_UI_OK")
#undef CM_REMOVE_UI_OK
    inline constexpr auto CM_REMOVE_UI_OK =
#pragma pop_macro("CM_REMOVE_UI_OK")
      CM_REMOVE_UI_OK;
#pragma push_macro("CM_REMOVE_UI_NOT_OK")
#undef CM_REMOVE_UI_NOT_OK
    inline constexpr auto CM_REMOVE_UI_NOT_OK =
#pragma pop_macro("CM_REMOVE_UI_NOT_OK")
      CM_REMOVE_UI_NOT_OK;
#pragma push_macro("CM_REMOVE_NO_RESTART")
#undef CM_REMOVE_NO_RESTART
    inline constexpr auto CM_REMOVE_NO_RESTART =
#pragma pop_macro("CM_REMOVE_NO_RESTART")
      CM_REMOVE_NO_RESTART;

#pragma push_macro("CM_REMOVE_DISABLE")
#undef CM_REMOVE_DISABLE
    inline constexpr auto CM_REMOVE_DISABLE =
#pragma pop_macro("CM_REMOVE_DISABLE")
      CM_REMOVE_DISABLE;

#pragma push_macro("CM_REMOVE_BITS")
#undef CM_REMOVE_BITS
    inline constexpr auto CM_REMOVE_BITS =
#pragma pop_macro("CM_REMOVE_BITS")
      CM_REMOVE_BITS;

#pragma push_macro("CM_QUERY_REMOVE_UI_OK")
#undef CM_QUERY_REMOVE_UI_OK
    inline constexpr auto CM_QUERY_REMOVE_UI_OK =
#pragma pop_macro("CM_QUERY_REMOVE_UI_OK")
      CM_QUERY_REMOVE_UI_OK;
#pragma push_macro("CM_QUERY_REMOVE_UI_NOT_OK")
#undef CM_QUERY_REMOVE_UI_NOT_OK
    inline constexpr auto CM_QUERY_REMOVE_UI_NOT_OK =
#pragma pop_macro("CM_QUERY_REMOVE_UI_NOT_OK")
      CM_QUERY_REMOVE_UI_NOT_OK;
#pragma push_macro("CM_QUERY_REMOVE_BITS")
#undef CM_QUERY_REMOVE_BITS
    inline constexpr auto CM_QUERY_REMOVE_BITS =
#pragma pop_macro("CM_QUERY_REMOVE_BITS")
      CM_QUERY_REMOVE_BITS;

#pragma push_macro("CM_REENUMERATE_NORMAL")
#undef CM_REENUMERATE_NORMAL
    inline constexpr auto CM_REENUMERATE_NORMAL =
#pragma pop_macro("CM_REENUMERATE_NORMAL")
      CM_REENUMERATE_NORMAL;
#pragma push_macro("CM_REENUMERATE_SYNCHRONOUS")
#undef CM_REENUMERATE_SYNCHRONOUS
    inline constexpr auto CM_REENUMERATE_SYNCHRONOUS =
#pragma pop_macro("CM_REENUMERATE_SYNCHRONOUS")
      CM_REENUMERATE_SYNCHRONOUS;

#pragma push_macro("CM_REENUMERATE_RETRY_INSTALLATION")
#undef CM_REENUMERATE_RETRY_INSTALLATION
    inline constexpr auto CM_REENUMERATE_RETRY_INSTALLATION =
#pragma pop_macro("CM_REENUMERATE_RETRY_INSTALLATION")
      CM_REENUMERATE_RETRY_INSTALLATION;
#pragma push_macro("CM_REENUMERATE_ASYNCHRONOUS")
#undef CM_REENUMERATE_ASYNCHRONOUS
    inline constexpr auto CM_REENUMERATE_ASYNCHRONOUS =
#pragma pop_macro("CM_REENUMERATE_ASYNCHRONOUS")
      CM_REENUMERATE_ASYNCHRONOUS;

#pragma push_macro("CM_REENUMERATE_BITS")
#undef CM_REENUMERATE_BITS
    inline constexpr auto CM_REENUMERATE_BITS =
#pragma pop_macro("CM_REENUMERATE_BITS")
      CM_REENUMERATE_BITS;

#pragma push_macro("CM_REGISTER_DEVICE_DRIVER_STATIC")
#undef CM_REGISTER_DEVICE_DRIVER_STATIC
    inline constexpr auto CM_REGISTER_DEVICE_DRIVER_STATIC =
#pragma pop_macro("CM_REGISTER_DEVICE_DRIVER_STATIC")
      CM_REGISTER_DEVICE_DRIVER_STATIC;
#pragma push_macro("CM_REGISTER_DEVICE_DRIVER_DISABLEABLE")
#undef CM_REGISTER_DEVICE_DRIVER_DISABLEABLE
    inline constexpr auto CM_REGISTER_DEVICE_DRIVER_DISABLEABLE =
#pragma pop_macro("CM_REGISTER_DEVICE_DRIVER_DISABLEABLE")
      CM_REGISTER_DEVICE_DRIVER_DISABLEABLE;
#pragma push_macro("CM_REGISTER_DEVICE_DRIVER_REMOVABLE")
#undef CM_REGISTER_DEVICE_DRIVER_REMOVABLE
    inline constexpr auto CM_REGISTER_DEVICE_DRIVER_REMOVABLE =
#pragma pop_macro("CM_REGISTER_DEVICE_DRIVER_REMOVABLE")
      CM_REGISTER_DEVICE_DRIVER_REMOVABLE;
#pragma push_macro("CM_REGISTER_DEVICE_DRIVER_BITS")
#undef CM_REGISTER_DEVICE_DRIVER_BITS
    inline constexpr auto CM_REGISTER_DEVICE_DRIVER_BITS =
#pragma pop_macro("CM_REGISTER_DEVICE_DRIVER_BITS")
      CM_REGISTER_DEVICE_DRIVER_BITS;

#pragma push_macro("CM_REGISTRY_HARDWARE")
#undef CM_REGISTRY_HARDWARE
    inline constexpr auto CM_REGISTRY_HARDWARE =
#pragma pop_macro("CM_REGISTRY_HARDWARE")
      CM_REGISTRY_HARDWARE;
#pragma push_macro("CM_REGISTRY_SOFTWARE")
#undef CM_REGISTRY_SOFTWARE
    inline constexpr auto CM_REGISTRY_SOFTWARE =
#pragma pop_macro("CM_REGISTRY_SOFTWARE")
      CM_REGISTRY_SOFTWARE;
#pragma push_macro("CM_REGISTRY_USER")
#undef CM_REGISTRY_USER
    inline constexpr auto CM_REGISTRY_USER =
#pragma pop_macro("CM_REGISTRY_USER")
      CM_REGISTRY_USER;
#pragma push_macro("CM_REGISTRY_CONFIG")
#undef CM_REGISTRY_CONFIG
    inline constexpr auto CM_REGISTRY_CONFIG =
#pragma pop_macro("CM_REGISTRY_CONFIG")
      CM_REGISTRY_CONFIG;
#pragma push_macro("CM_REGISTRY_BITS")
#undef CM_REGISTRY_BITS
    inline constexpr auto CM_REGISTRY_BITS =
#pragma pop_macro("CM_REGISTRY_BITS")
      CM_REGISTRY_BITS;

#pragma push_macro("CM_SET_DEVNODE_PROBLEM_NORMAL")
#undef CM_SET_DEVNODE_PROBLEM_NORMAL
    inline constexpr auto CM_SET_DEVNODE_PROBLEM_NORMAL =
#pragma pop_macro("CM_SET_DEVNODE_PROBLEM_NORMAL")
      CM_SET_DEVNODE_PROBLEM_NORMAL;
#pragma push_macro("CM_SET_DEVNODE_PROBLEM_OVERRIDE")
#undef CM_SET_DEVNODE_PROBLEM_OVERRIDE
    inline constexpr auto CM_SET_DEVNODE_PROBLEM_OVERRIDE =
#pragma pop_macro("CM_SET_DEVNODE_PROBLEM_OVERRIDE")
      CM_SET_DEVNODE_PROBLEM_OVERRIDE;
#pragma push_macro("CM_SET_DEVNODE_PROBLEM_BITS")
#undef CM_SET_DEVNODE_PROBLEM_BITS
    inline constexpr auto CM_SET_DEVNODE_PROBLEM_BITS =
#pragma pop_macro("CM_SET_DEVNODE_PROBLEM_BITS")
      CM_SET_DEVNODE_PROBLEM_BITS;

#pragma push_macro("CM_SET_DEVINST_PROBLEM_NORMAL")
#undef CM_SET_DEVINST_PROBLEM_NORMAL
    inline constexpr auto CM_SET_DEVINST_PROBLEM_NORMAL =
#pragma pop_macro("CM_SET_DEVINST_PROBLEM_NORMAL")
      CM_SET_DEVINST_PROBLEM_NORMAL;
#pragma push_macro("CM_SET_DEVINST_PROBLEM_OVERRIDE")
#undef CM_SET_DEVINST_PROBLEM_OVERRIDE
    inline constexpr auto CM_SET_DEVINST_PROBLEM_OVERRIDE =
#pragma pop_macro("CM_SET_DEVINST_PROBLEM_OVERRIDE")
      CM_SET_DEVINST_PROBLEM_OVERRIDE;
#pragma push_macro("CM_SET_DEVINST_PROBLEM_BITS")
#undef CM_SET_DEVINST_PROBLEM_BITS
    inline constexpr auto CM_SET_DEVINST_PROBLEM_BITS =
#pragma pop_macro("CM_SET_DEVINST_PROBLEM_BITS")
      CM_SET_DEVINST_PROBLEM_BITS;

#pragma push_macro("CM_SET_HW_PROF_FLAGS_UI_NOT_OK")
#undef CM_SET_HW_PROF_FLAGS_UI_NOT_OK
    inline constexpr auto CM_SET_HW_PROF_FLAGS_UI_NOT_OK =
#pragma pop_macro("CM_SET_HW_PROF_FLAGS_UI_NOT_OK")
      CM_SET_HW_PROF_FLAGS_UI_NOT_OK;
#pragma push_macro("CM_SET_HW_PROF_FLAGS_BITS")
#undef CM_SET_HW_PROF_FLAGS_BITS
    inline constexpr auto CM_SET_HW_PROF_FLAGS_BITS =
#pragma pop_macro("CM_SET_HW_PROF_FLAGS_BITS")
      CM_SET_HW_PROF_FLAGS_BITS;

#pragma push_macro("CM_SETUP_DEVNODE_READY")
#undef CM_SETUP_DEVNODE_READY
    inline constexpr auto CM_SETUP_DEVNODE_READY =
#pragma pop_macro("CM_SETUP_DEVNODE_READY")
      CM_SETUP_DEVNODE_READY;
#pragma push_macro("CM_SETUP_DEVINST_READY")
#undef CM_SETUP_DEVINST_READY
    inline constexpr auto CM_SETUP_DEVINST_READY =
#pragma pop_macro("CM_SETUP_DEVINST_READY")
      CM_SETUP_DEVINST_READY;
#pragma push_macro("CM_SETUP_DOWNLOAD")
#undef CM_SETUP_DOWNLOAD
    inline constexpr auto CM_SETUP_DOWNLOAD =
#pragma pop_macro("CM_SETUP_DOWNLOAD")
      CM_SETUP_DOWNLOAD;
#pragma push_macro("CM_SETUP_WRITE_LOG_CONFS")
#undef CM_SETUP_WRITE_LOG_CONFS
    inline constexpr auto CM_SETUP_WRITE_LOG_CONFS =
#pragma pop_macro("CM_SETUP_WRITE_LOG_CONFS")
      CM_SETUP_WRITE_LOG_CONFS;
#pragma push_macro("CM_SETUP_PROP_CHANGE")
#undef CM_SETUP_PROP_CHANGE
    inline constexpr auto CM_SETUP_PROP_CHANGE =
#pragma pop_macro("CM_SETUP_PROP_CHANGE")
      CM_SETUP_PROP_CHANGE;

#pragma push_macro("CM_SETUP_DEVNODE_RESET")
#undef CM_SETUP_DEVNODE_RESET
    inline constexpr auto CM_SETUP_DEVNODE_RESET =
#pragma pop_macro("CM_SETUP_DEVNODE_RESET")
      CM_SETUP_DEVNODE_RESET;
#pragma push_macro("CM_SETUP_DEVINST_RESET")
#undef CM_SETUP_DEVINST_RESET
    inline constexpr auto CM_SETUP_DEVINST_RESET =
#pragma pop_macro("CM_SETUP_DEVINST_RESET")
      CM_SETUP_DEVINST_RESET;

#pragma push_macro("CM_SETUP_DEVNODE_CONFIG")
#undef CM_SETUP_DEVNODE_CONFIG
    inline constexpr auto CM_SETUP_DEVNODE_CONFIG =
#pragma pop_macro("CM_SETUP_DEVNODE_CONFIG")
      CM_SETUP_DEVNODE_CONFIG;
#pragma push_macro("CM_SETUP_DEVINST_CONFIG")
#undef CM_SETUP_DEVINST_CONFIG
    inline constexpr auto CM_SETUP_DEVINST_CONFIG =
#pragma pop_macro("CM_SETUP_DEVINST_CONFIG")
      CM_SETUP_DEVINST_CONFIG;
#pragma push_macro("CM_SETUP_DEVNODE_CONFIG_CLASS")
#undef CM_SETUP_DEVNODE_CONFIG_CLASS
    inline constexpr auto CM_SETUP_DEVNODE_CONFIG_CLASS =
#pragma pop_macro("CM_SETUP_DEVNODE_CONFIG_CLASS")
      CM_SETUP_DEVNODE_CONFIG_CLASS;
#pragma push_macro("CM_SETUP_DEVINST_CONFIG_CLASS")
#undef CM_SETUP_DEVINST_CONFIG_CLASS
    inline constexpr auto CM_SETUP_DEVINST_CONFIG_CLASS =
#pragma pop_macro("CM_SETUP_DEVINST_CONFIG_CLASS")
      CM_SETUP_DEVINST_CONFIG_CLASS;

#pragma push_macro("CM_SETUP_DEVNODE_CONFIG_EXTENSIONS")
#undef CM_SETUP_DEVNODE_CONFIG_EXTENSIONS
    inline constexpr auto CM_SETUP_DEVNODE_CONFIG_EXTENSIONS =
#pragma pop_macro("CM_SETUP_DEVNODE_CONFIG_EXTENSIONS")
      CM_SETUP_DEVNODE_CONFIG_EXTENSIONS;
#pragma push_macro("CM_SETUP_DEVINST_CONFIG_EXTENSIONS")
#undef CM_SETUP_DEVINST_CONFIG_EXTENSIONS
    inline constexpr auto CM_SETUP_DEVINST_CONFIG_EXTENSIONS =
#pragma pop_macro("CM_SETUP_DEVINST_CONFIG_EXTENSIONS")
      CM_SETUP_DEVINST_CONFIG_EXTENSIONS;

#pragma push_macro("CM_SETUP_DEVNODE_CONFIG_RESET")
#undef CM_SETUP_DEVNODE_CONFIG_RESET
    inline constexpr auto CM_SETUP_DEVNODE_CONFIG_RESET =
#pragma pop_macro("CM_SETUP_DEVNODE_CONFIG_RESET")
      CM_SETUP_DEVNODE_CONFIG_RESET;
#pragma push_macro("CM_SETUP_DEVINST_CONFIG_RESET")
#undef CM_SETUP_DEVINST_CONFIG_RESET
    inline constexpr auto CM_SETUP_DEVINST_CONFIG_RESET =
#pragma pop_macro("CM_SETUP_DEVINST_CONFIG_RESET")
      CM_SETUP_DEVINST_CONFIG_RESET;

#pragma push_macro("CM_SETUP_BITS")
#undef CM_SETUP_BITS
    inline constexpr auto CM_SETUP_BITS =
#pragma pop_macro("CM_SETUP_BITS")
      CM_SETUP_BITS;

#pragma push_macro("CM_QUERY_ARBITRATOR_RAW")
#undef CM_QUERY_ARBITRATOR_RAW
    inline constexpr auto CM_QUERY_ARBITRATOR_RAW =
#pragma pop_macro("CM_QUERY_ARBITRATOR_RAW")
      CM_QUERY_ARBITRATOR_RAW;
#pragma push_macro("CM_QUERY_ARBITRATOR_TRANSLATED")
#undef CM_QUERY_ARBITRATOR_TRANSLATED
    inline constexpr auto CM_QUERY_ARBITRATOR_TRANSLATED =
#pragma pop_macro("CM_QUERY_ARBITRATOR_TRANSLATED")
      CM_QUERY_ARBITRATOR_TRANSLATED;
#pragma push_macro("CM_QUERY_ARBITRATOR_BITS")
#undef CM_QUERY_ARBITRATOR_BITS
    inline constexpr auto CM_QUERY_ARBITRATOR_BITS =
#pragma pop_macro("CM_QUERY_ARBITRATOR_BITS")
      CM_QUERY_ARBITRATOR_BITS;

#pragma push_macro("CM_CUSTOMDEVPROP_MERGE_MULTISZ")
#undef CM_CUSTOMDEVPROP_MERGE_MULTISZ
    inline constexpr auto CM_CUSTOMDEVPROP_MERGE_MULTISZ =
#pragma pop_macro("CM_CUSTOMDEVPROP_MERGE_MULTISZ")
      CM_CUSTOMDEVPROP_MERGE_MULTISZ;
#pragma push_macro("CM_CUSTOMDEVPROP_BITS")
#undef CM_CUSTOMDEVPROP_BITS
    inline constexpr auto CM_CUSTOMDEVPROP_BITS =
#pragma pop_macro("CM_CUSTOMDEVPROP_BITS")
      CM_CUSTOMDEVPROP_BITS;

#pragma push_macro("CM_NAME_ATTRIBUTE_NAME_RETRIEVED_FROM_DEVICE")
#undef CM_NAME_ATTRIBUTE_NAME_RETRIEVED_FROM_DEVICE
    inline constexpr auto CM_NAME_ATTRIBUTE_NAME_RETRIEVED_FROM_DEVICE =
#pragma pop_macro("CM_NAME_ATTRIBUTE_NAME_RETRIEVED_FROM_DEVICE")
      CM_NAME_ATTRIBUTE_NAME_RETRIEVED_FROM_DEVICE;
#pragma push_macro("CM_NAME_ATTRIBUTE_USER_ASSIGNED_NAME")
#undef CM_NAME_ATTRIBUTE_USER_ASSIGNED_NAME
    inline constexpr auto CM_NAME_ATTRIBUTE_USER_ASSIGNED_NAME =
#pragma pop_macro("CM_NAME_ATTRIBUTE_USER_ASSIGNED_NAME")
      CM_NAME_ATTRIBUTE_USER_ASSIGNED_NAME;

#pragma push_macro("CM_CLASS_PROPERTY_INSTALLER")
#undef CM_CLASS_PROPERTY_INSTALLER
    inline constexpr auto CM_CLASS_PROPERTY_INSTALLER =
#pragma pop_macro("CM_CLASS_PROPERTY_INSTALLER")
      CM_CLASS_PROPERTY_INSTALLER;
#pragma push_macro("CM_CLASS_PROPERTY_INTERFACE")
#undef CM_CLASS_PROPERTY_INTERFACE
    inline constexpr auto CM_CLASS_PROPERTY_INTERFACE =
#pragma pop_macro("CM_CLASS_PROPERTY_INTERFACE")
      CM_CLASS_PROPERTY_INTERFACE;
#pragma push_macro("CM_CLASS_PROPERTY_BITS")
#undef CM_CLASS_PROPERTY_BITS
    inline constexpr auto CM_CLASS_PROPERTY_BITS =
#pragma pop_macro("CM_CLASS_PROPERTY_BITS")
      CM_CLASS_PROPERTY_BITS;

#pragma push_macro("CM_NOTIFY_FILTER_FLAG_ALL_INTERFACE_CLASSES")
#undef CM_NOTIFY_FILTER_FLAG_ALL_INTERFACE_CLASSES
    inline constexpr auto CM_NOTIFY_FILTER_FLAG_ALL_INTERFACE_CLASSES =
#pragma pop_macro("CM_NOTIFY_FILTER_FLAG_ALL_INTERFACE_CLASSES")
      CM_NOTIFY_FILTER_FLAG_ALL_INTERFACE_CLASSES;
#pragma push_macro("CM_NOTIFY_FILTER_FLAG_ALL_DEVICE_INSTANCES")
#undef CM_NOTIFY_FILTER_FLAG_ALL_DEVICE_INSTANCES
    inline constexpr auto CM_NOTIFY_FILTER_FLAG_ALL_DEVICE_INSTANCES =
#pragma pop_macro("CM_NOTIFY_FILTER_FLAG_ALL_DEVICE_INSTANCES")
      CM_NOTIFY_FILTER_FLAG_ALL_DEVICE_INSTANCES;

#pragma push_macro("CM_NOTIFY_FILTER_VALID_FLAGS")
#undef CM_NOTIFY_FILTER_VALID_FLAGS
    inline constexpr auto CM_NOTIFY_FILTER_VALID_FLAGS =
#pragma pop_macro("CM_NOTIFY_FILTER_VALID_FLAGS")
      CM_NOTIFY_FILTER_VALID_FLAGS;

#pragma push_macro("CM_GLOBAL_STATE_CAN_DO_UI")
#undef CM_GLOBAL_STATE_CAN_DO_UI
    inline constexpr auto CM_GLOBAL_STATE_CAN_DO_UI =
#pragma pop_macro("CM_GLOBAL_STATE_CAN_DO_UI")
      CM_GLOBAL_STATE_CAN_DO_UI;
#pragma push_macro("CM_GLOBAL_STATE_ON_BIG_STACK")
#undef CM_GLOBAL_STATE_ON_BIG_STACK
    inline constexpr auto CM_GLOBAL_STATE_ON_BIG_STACK =
#pragma pop_macro("CM_GLOBAL_STATE_ON_BIG_STACK")
      CM_GLOBAL_STATE_ON_BIG_STACK;
#pragma push_macro("CM_GLOBAL_STATE_SERVICES_AVAILABLE")
#undef CM_GLOBAL_STATE_SERVICES_AVAILABLE
    inline constexpr auto CM_GLOBAL_STATE_SERVICES_AVAILABLE =
#pragma pop_macro("CM_GLOBAL_STATE_SERVICES_AVAILABLE")
      CM_GLOBAL_STATE_SERVICES_AVAILABLE;
#pragma push_macro("CM_GLOBAL_STATE_SHUTTING_DOWN")
#undef CM_GLOBAL_STATE_SHUTTING_DOWN
    inline constexpr auto CM_GLOBAL_STATE_SHUTTING_DOWN =
#pragma pop_macro("CM_GLOBAL_STATE_SHUTTING_DOWN")
      CM_GLOBAL_STATE_SHUTTING_DOWN;
#pragma push_macro("CM_GLOBAL_STATE_DETECTION_PENDING")
#undef CM_GLOBAL_STATE_DETECTION_PENDING
    inline constexpr auto CM_GLOBAL_STATE_DETECTION_PENDING =
#pragma pop_macro("CM_GLOBAL_STATE_DETECTION_PENDING")
      CM_GLOBAL_STATE_DETECTION_PENDING;

#pragma push_macro("CM_GLOBAL_STATE_REBOOT_REQUIRED")
#undef CM_GLOBAL_STATE_REBOOT_REQUIRED
    inline constexpr auto CM_GLOBAL_STATE_REBOOT_REQUIRED =
#pragma pop_macro("CM_GLOBAL_STATE_REBOOT_REQUIRED")
      CM_GLOBAL_STATE_REBOOT_REQUIRED;

#pragma push_macro("CR_SUCCESS")
#undef CR_SUCCESS
    inline constexpr auto CR_SUCCESS =
#pragma pop_macro("CR_SUCCESS")
      CR_SUCCESS;
#pragma push_macro("CR_DEFAULT")
#undef CR_DEFAULT
    inline constexpr auto CR_DEFAULT =
#pragma pop_macro("CR_DEFAULT")
      CR_DEFAULT;
#pragma push_macro("CR_OUT_OF_MEMORY")
#undef CR_OUT_OF_MEMORY
    inline constexpr auto CR_OUT_OF_MEMORY =
#pragma pop_macro("CR_OUT_OF_MEMORY")
      CR_OUT_OF_MEMORY;
#pragma push_macro("CR_INVALID_POINTER")
#undef CR_INVALID_POINTER
    inline constexpr auto CR_INVALID_POINTER =
#pragma pop_macro("CR_INVALID_POINTER")
      CR_INVALID_POINTER;
#pragma push_macro("CR_INVALID_FLAG")
#undef CR_INVALID_FLAG
    inline constexpr auto CR_INVALID_FLAG =
#pragma pop_macro("CR_INVALID_FLAG")
      CR_INVALID_FLAG;
#pragma push_macro("CR_INVALID_DEVNODE")
#undef CR_INVALID_DEVNODE
    inline constexpr auto CR_INVALID_DEVNODE =
#pragma pop_macro("CR_INVALID_DEVNODE")
      CR_INVALID_DEVNODE;
#pragma push_macro("CR_INVALID_DEVINST")
#undef CR_INVALID_DEVINST
    inline constexpr auto CR_INVALID_DEVINST =
#pragma pop_macro("CR_INVALID_DEVINST")
      CR_INVALID_DEVINST;
#pragma push_macro("CR_INVALID_RES_DES")
#undef CR_INVALID_RES_DES
    inline constexpr auto CR_INVALID_RES_DES =
#pragma pop_macro("CR_INVALID_RES_DES")
      CR_INVALID_RES_DES;
#pragma push_macro("CR_INVALID_LOG_CONF")
#undef CR_INVALID_LOG_CONF
    inline constexpr auto CR_INVALID_LOG_CONF =
#pragma pop_macro("CR_INVALID_LOG_CONF")
      CR_INVALID_LOG_CONF;
#pragma push_macro("CR_INVALID_ARBITRATOR")
#undef CR_INVALID_ARBITRATOR
    inline constexpr auto CR_INVALID_ARBITRATOR =
#pragma pop_macro("CR_INVALID_ARBITRATOR")
      CR_INVALID_ARBITRATOR;
#pragma push_macro("CR_INVALID_NODELIST")
#undef CR_INVALID_NODELIST
    inline constexpr auto CR_INVALID_NODELIST =
#pragma pop_macro("CR_INVALID_NODELIST")
      CR_INVALID_NODELIST;
#pragma push_macro("CR_DEVNODE_HAS_REQS")
#undef CR_DEVNODE_HAS_REQS
    inline constexpr auto CR_DEVNODE_HAS_REQS =
#pragma pop_macro("CR_DEVNODE_HAS_REQS")
      CR_DEVNODE_HAS_REQS;
#pragma push_macro("CR_DEVINST_HAS_REQS")
#undef CR_DEVINST_HAS_REQS
    inline constexpr auto CR_DEVINST_HAS_REQS =
#pragma pop_macro("CR_DEVINST_HAS_REQS")
      CR_DEVINST_HAS_REQS;
#pragma push_macro("CR_INVALID_RESOURCEID")
#undef CR_INVALID_RESOURCEID
    inline constexpr auto CR_INVALID_RESOURCEID =
#pragma pop_macro("CR_INVALID_RESOURCEID")
      CR_INVALID_RESOURCEID;
#pragma push_macro("CR_DLVXD_NOT_FOUND")
#undef CR_DLVXD_NOT_FOUND
    inline constexpr auto CR_DLVXD_NOT_FOUND =
#pragma pop_macro("CR_DLVXD_NOT_FOUND")
      CR_DLVXD_NOT_FOUND;
#pragma push_macro("CR_NO_SUCH_DEVNODE")
#undef CR_NO_SUCH_DEVNODE
    inline constexpr auto CR_NO_SUCH_DEVNODE =
#pragma pop_macro("CR_NO_SUCH_DEVNODE")
      CR_NO_SUCH_DEVNODE;
#pragma push_macro("CR_NO_SUCH_DEVINST")
#undef CR_NO_SUCH_DEVINST
    inline constexpr auto CR_NO_SUCH_DEVINST =
#pragma pop_macro("CR_NO_SUCH_DEVINST")
      CR_NO_SUCH_DEVINST;
#pragma push_macro("CR_NO_MORE_LOG_CONF")
#undef CR_NO_MORE_LOG_CONF
    inline constexpr auto CR_NO_MORE_LOG_CONF =
#pragma pop_macro("CR_NO_MORE_LOG_CONF")
      CR_NO_MORE_LOG_CONF;
#pragma push_macro("CR_NO_MORE_RES_DES")
#undef CR_NO_MORE_RES_DES
    inline constexpr auto CR_NO_MORE_RES_DES =
#pragma pop_macro("CR_NO_MORE_RES_DES")
      CR_NO_MORE_RES_DES;
#pragma push_macro("CR_ALREADY_SUCH_DEVNODE")
#undef CR_ALREADY_SUCH_DEVNODE
    inline constexpr auto CR_ALREADY_SUCH_DEVNODE =
#pragma pop_macro("CR_ALREADY_SUCH_DEVNODE")
      CR_ALREADY_SUCH_DEVNODE;
#pragma push_macro("CR_ALREADY_SUCH_DEVINST")
#undef CR_ALREADY_SUCH_DEVINST
    inline constexpr auto CR_ALREADY_SUCH_DEVINST =
#pragma pop_macro("CR_ALREADY_SUCH_DEVINST")
      CR_ALREADY_SUCH_DEVINST;
#pragma push_macro("CR_INVALID_RANGE_LIST")
#undef CR_INVALID_RANGE_LIST
    inline constexpr auto CR_INVALID_RANGE_LIST =
#pragma pop_macro("CR_INVALID_RANGE_LIST")
      CR_INVALID_RANGE_LIST;
#pragma push_macro("CR_INVALID_RANGE")
#undef CR_INVALID_RANGE
    inline constexpr auto CR_INVALID_RANGE =
#pragma pop_macro("CR_INVALID_RANGE")
      CR_INVALID_RANGE;
#pragma push_macro("CR_FAILURE")
#undef CR_FAILURE
    inline constexpr auto CR_FAILURE =
#pragma pop_macro("CR_FAILURE")
      CR_FAILURE;
#pragma push_macro("CR_NO_SUCH_LOGICAL_DEV")
#undef CR_NO_SUCH_LOGICAL_DEV
    inline constexpr auto CR_NO_SUCH_LOGICAL_DEV =
#pragma pop_macro("CR_NO_SUCH_LOGICAL_DEV")
      CR_NO_SUCH_LOGICAL_DEV;
#pragma push_macro("CR_CREATE_BLOCKED")
#undef CR_CREATE_BLOCKED
    inline constexpr auto CR_CREATE_BLOCKED =
#pragma pop_macro("CR_CREATE_BLOCKED")
      CR_CREATE_BLOCKED;
#pragma push_macro("CR_NOT_SYSTEM_VM")
#undef CR_NOT_SYSTEM_VM
    inline constexpr auto CR_NOT_SYSTEM_VM =
#pragma pop_macro("CR_NOT_SYSTEM_VM")
      CR_NOT_SYSTEM_VM;
#pragma push_macro("CR_REMOVE_VETOED")
#undef CR_REMOVE_VETOED
    inline constexpr auto CR_REMOVE_VETOED =
#pragma pop_macro("CR_REMOVE_VETOED")
      CR_REMOVE_VETOED;
#pragma push_macro("CR_APM_VETOED")
#undef CR_APM_VETOED
    inline constexpr auto CR_APM_VETOED =
#pragma pop_macro("CR_APM_VETOED")
      CR_APM_VETOED;
#pragma push_macro("CR_INVALID_LOAD_TYPE")
#undef CR_INVALID_LOAD_TYPE
    inline constexpr auto CR_INVALID_LOAD_TYPE =
#pragma pop_macro("CR_INVALID_LOAD_TYPE")
      CR_INVALID_LOAD_TYPE;
#pragma push_macro("CR_BUFFER_SMALL")
#undef CR_BUFFER_SMALL
    inline constexpr auto CR_BUFFER_SMALL =
#pragma pop_macro("CR_BUFFER_SMALL")
      CR_BUFFER_SMALL;
#pragma push_macro("CR_NO_ARBITRATOR")
#undef CR_NO_ARBITRATOR
    inline constexpr auto CR_NO_ARBITRATOR =
#pragma pop_macro("CR_NO_ARBITRATOR")
      CR_NO_ARBITRATOR;
#pragma push_macro("CR_NO_REGISTRY_HANDLE")
#undef CR_NO_REGISTRY_HANDLE
    inline constexpr auto CR_NO_REGISTRY_HANDLE =
#pragma pop_macro("CR_NO_REGISTRY_HANDLE")
      CR_NO_REGISTRY_HANDLE;
#pragma push_macro("CR_REGISTRY_ERROR")
#undef CR_REGISTRY_ERROR
    inline constexpr auto CR_REGISTRY_ERROR =
#pragma pop_macro("CR_REGISTRY_ERROR")
      CR_REGISTRY_ERROR;
#pragma push_macro("CR_INVALID_DEVICE_ID")
#undef CR_INVALID_DEVICE_ID
    inline constexpr auto CR_INVALID_DEVICE_ID =
#pragma pop_macro("CR_INVALID_DEVICE_ID")
      CR_INVALID_DEVICE_ID;
#pragma push_macro("CR_INVALID_DATA")
#undef CR_INVALID_DATA
    inline constexpr auto CR_INVALID_DATA =
#pragma pop_macro("CR_INVALID_DATA")
      CR_INVALID_DATA;
#pragma push_macro("CR_INVALID_API")
#undef CR_INVALID_API
    inline constexpr auto CR_INVALID_API =
#pragma pop_macro("CR_INVALID_API")
      CR_INVALID_API;
#pragma push_macro("CR_DEVLOADER_NOT_READY")
#undef CR_DEVLOADER_NOT_READY
    inline constexpr auto CR_DEVLOADER_NOT_READY =
#pragma pop_macro("CR_DEVLOADER_NOT_READY")
      CR_DEVLOADER_NOT_READY;
#pragma push_macro("CR_NEED_RESTART")
#undef CR_NEED_RESTART
    inline constexpr auto CR_NEED_RESTART =
#pragma pop_macro("CR_NEED_RESTART")
      CR_NEED_RESTART;
#pragma push_macro("CR_NO_MORE_HW_PROFILES")
#undef CR_NO_MORE_HW_PROFILES
    inline constexpr auto CR_NO_MORE_HW_PROFILES =
#pragma pop_macro("CR_NO_MORE_HW_PROFILES")
      CR_NO_MORE_HW_PROFILES;
#pragma push_macro("CR_DEVICE_NOT_THERE")
#undef CR_DEVICE_NOT_THERE
    inline constexpr auto CR_DEVICE_NOT_THERE =
#pragma pop_macro("CR_DEVICE_NOT_THERE")
      CR_DEVICE_NOT_THERE;
#pragma push_macro("CR_NO_SUCH_VALUE")
#undef CR_NO_SUCH_VALUE
    inline constexpr auto CR_NO_SUCH_VALUE =
#pragma pop_macro("CR_NO_SUCH_VALUE")
      CR_NO_SUCH_VALUE;
#pragma push_macro("CR_WRONG_TYPE")
#undef CR_WRONG_TYPE
    inline constexpr auto CR_WRONG_TYPE =
#pragma pop_macro("CR_WRONG_TYPE")
      CR_WRONG_TYPE;
#pragma push_macro("CR_INVALID_PRIORITY")
#undef CR_INVALID_PRIORITY
    inline constexpr auto CR_INVALID_PRIORITY =
#pragma pop_macro("CR_INVALID_PRIORITY")
      CR_INVALID_PRIORITY;
#pragma push_macro("CR_NOT_DISABLEABLE")
#undef CR_NOT_DISABLEABLE
    inline constexpr auto CR_NOT_DISABLEABLE =
#pragma pop_macro("CR_NOT_DISABLEABLE")
      CR_NOT_DISABLEABLE;
#pragma push_macro("CR_FREE_RESOURCES")
#undef CR_FREE_RESOURCES
    inline constexpr auto CR_FREE_RESOURCES =
#pragma pop_macro("CR_FREE_RESOURCES")
      CR_FREE_RESOURCES;
#pragma push_macro("CR_QUERY_VETOED")
#undef CR_QUERY_VETOED
    inline constexpr auto CR_QUERY_VETOED =
#pragma pop_macro("CR_QUERY_VETOED")
      CR_QUERY_VETOED;
#pragma push_macro("CR_CANT_SHARE_IRQ")
#undef CR_CANT_SHARE_IRQ
    inline constexpr auto CR_CANT_SHARE_IRQ =
#pragma pop_macro("CR_CANT_SHARE_IRQ")
      CR_CANT_SHARE_IRQ;
#pragma push_macro("CR_NO_DEPENDENT")
#undef CR_NO_DEPENDENT
    inline constexpr auto CR_NO_DEPENDENT =
#pragma pop_macro("CR_NO_DEPENDENT")
      CR_NO_DEPENDENT;
#pragma push_macro("CR_SAME_RESOURCES")
#undef CR_SAME_RESOURCES
    inline constexpr auto CR_SAME_RESOURCES =
#pragma pop_macro("CR_SAME_RESOURCES")
      CR_SAME_RESOURCES;
#pragma push_macro("CR_NO_SUCH_REGISTRY_KEY")
#undef CR_NO_SUCH_REGISTRY_KEY
    inline constexpr auto CR_NO_SUCH_REGISTRY_KEY =
#pragma pop_macro("CR_NO_SUCH_REGISTRY_KEY")
      CR_NO_SUCH_REGISTRY_KEY;
#pragma push_macro("CR_INVALID_MACHINENAME")
#undef CR_INVALID_MACHINENAME
    inline constexpr auto CR_INVALID_MACHINENAME =
#pragma pop_macro("CR_INVALID_MACHINENAME")
      CR_INVALID_MACHINENAME;
#pragma push_macro("CR_REMOTE_COMM_FAILURE")
#undef CR_REMOTE_COMM_FAILURE
    inline constexpr auto CR_REMOTE_COMM_FAILURE =
#pragma pop_macro("CR_REMOTE_COMM_FAILURE")
      CR_REMOTE_COMM_FAILURE;
#pragma push_macro("CR_MACHINE_UNAVAILABLE")
#undef CR_MACHINE_UNAVAILABLE
    inline constexpr auto CR_MACHINE_UNAVAILABLE =
#pragma pop_macro("CR_MACHINE_UNAVAILABLE")
      CR_MACHINE_UNAVAILABLE;
#pragma push_macro("CR_NO_CM_SERVICES")
#undef CR_NO_CM_SERVICES
    inline constexpr auto CR_NO_CM_SERVICES =
#pragma pop_macro("CR_NO_CM_SERVICES")
      CR_NO_CM_SERVICES;
#pragma push_macro("CR_ACCESS_DENIED")
#undef CR_ACCESS_DENIED
    inline constexpr auto CR_ACCESS_DENIED =
#pragma pop_macro("CR_ACCESS_DENIED")
      CR_ACCESS_DENIED;
#pragma push_macro("CR_CALL_NOT_IMPLEMENTED")
#undef CR_CALL_NOT_IMPLEMENTED
    inline constexpr auto CR_CALL_NOT_IMPLEMENTED =
#pragma pop_macro("CR_CALL_NOT_IMPLEMENTED")
      CR_CALL_NOT_IMPLEMENTED;
#pragma push_macro("CR_INVALID_PROPERTY")
#undef CR_INVALID_PROPERTY
    inline constexpr auto CR_INVALID_PROPERTY =
#pragma pop_macro("CR_INVALID_PROPERTY")
      CR_INVALID_PROPERTY;
#pragma push_macro("CR_DEVICE_INTERFACE_ACTIVE")
#undef CR_DEVICE_INTERFACE_ACTIVE
    inline constexpr auto CR_DEVICE_INTERFACE_ACTIVE =
#pragma pop_macro("CR_DEVICE_INTERFACE_ACTIVE")
      CR_DEVICE_INTERFACE_ACTIVE;
#pragma push_macro("CR_NO_SUCH_DEVICE_INTERFACE")
#undef CR_NO_SUCH_DEVICE_INTERFACE
    inline constexpr auto CR_NO_SUCH_DEVICE_INTERFACE =
#pragma pop_macro("CR_NO_SUCH_DEVICE_INTERFACE")
      CR_NO_SUCH_DEVICE_INTERFACE;
#pragma push_macro("CR_INVALID_REFERENCE_STRING")
#undef CR_INVALID_REFERENCE_STRING
    inline constexpr auto CR_INVALID_REFERENCE_STRING =
#pragma pop_macro("CR_INVALID_REFERENCE_STRING")
      CR_INVALID_REFERENCE_STRING;
#pragma push_macro("CR_INVALID_CONFLICT_LIST")
#undef CR_INVALID_CONFLICT_LIST
    inline constexpr auto CR_INVALID_CONFLICT_LIST =
#pragma pop_macro("CR_INVALID_CONFLICT_LIST")
      CR_INVALID_CONFLICT_LIST;
#pragma push_macro("CR_INVALID_INDEX")
#undef CR_INVALID_INDEX
    inline constexpr auto CR_INVALID_INDEX =
#pragma pop_macro("CR_INVALID_INDEX")
      CR_INVALID_INDEX;
#pragma push_macro("CR_INVALID_STRUCTURE_SIZE")
#undef CR_INVALID_STRUCTURE_SIZE
    inline constexpr auto CR_INVALID_STRUCTURE_SIZE =
#pragma pop_macro("CR_INVALID_STRUCTURE_SIZE")
      CR_INVALID_STRUCTURE_SIZE;
#pragma push_macro("NUM_CR_RESULTS")
#undef NUM_CR_RESULTS
    inline constexpr auto NUM_CR_RESULTS =
#pragma pop_macro("NUM_CR_RESULTS")
      NUM_CR_RESULTS;
} // namespace win32
