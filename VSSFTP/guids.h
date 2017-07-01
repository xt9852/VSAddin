﻿// guids.h: definitions of GUIDs/IIDs/CLSIDs used in this VsPackage

/*
Do not use #pragma once, as this file needs to be included twice.  Once to declare the externs
for the GUIDs, and again right after including initguid.h to actually define the GUIDs.
*/



// package guid
// { 6a0dbf1c-e4e4-4424-95fc-8aa55b8c551b }
#define guidVSSFTPPkg { 0x6A0DBF1C, 0xE4E4, 0x4424, { 0x95, 0xFC, 0x8A, 0xA5, 0x5B, 0x8C, 0x55, 0x1B } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_VSSFTP,
0x6A0DBF1C, 0xE4E4, 0x4424, 0x95, 0xFC, 0x8A, 0xA5, 0x5B, 0x8C, 0x55, 0x1B );
#endif

// Command set guid for our commands (used with IOleCommandTarget)
// { 14b7f28c-e232-4a10-9655-da8293a3c3ce }
#define guidVSSFTPCmdSet { 0x14B7F28C, 0xE232, 0x4A10, { 0x96, 0x55, 0xDA, 0x82, 0x93, 0xA3, 0xC3, 0xCE } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_VSSFTPCmdSet, 
0x14B7F28C, 0xE232, 0x4A10, 0x96, 0x55, 0xDA, 0x82, 0x93, 0xA3, 0xC3, 0xCE );
#endif

//Guid for the image list referenced in the VSCT file
// { a348a708-2ffd-42a6-952d-d4ace430a5d0 }
#define guidImages { 0xA348A708, 0x2FFD, 0x42A6, { 0x95, 0x2D, 0xD4, 0xAC, 0xE4, 0x30, 0xA5, 0xD0 } }
#ifdef DEFINE_GUID
DEFINE_GUID(CLSID_Images, 
0xA348A708, 0x2FFD, 0x42A6, 0x95, 0x2D, 0xD4, 0xAC, 0xE4, 0x30, 0xA5, 0xD0 );
#endif

