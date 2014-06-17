<html>
<body>
<pre>
<h1>Build Log</h1>
<h3>
--------------------Configuration: ShowDeviceUUID - Win32 (WCE ARMV4I) Debug--------------------
</h3>
<h3>Command Lines</h3>
Creating command line "rc.exe /l 0x409 /fo"ARMV4IDbg/ShowDeviceUUID.res" /d UNDER_CE=500 /d _WIN32_WCE=500 /d "UNICODE" /d "_UNICODE" /d "DEBUG" /d "WCE_PLATFORM_MALATA_CAR_NAV" /d "THUMB" /d "_THUMB_" /d "ARM" /d "_ARM_" /d "ARMV4I" /r "E:\DAB_SDK\ShowDeviceUUID\ShowDeviceUUID.rc"" 
Creating temporary file "C:\DOCUME~1\NASa\LOCALS~1\Temp\RSPC1.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=500 /D _WIN32_WCE=500 /D "WCE_PLATFORM_MALATA_CAR_NAV" /D "UNICODE" /D "_UNICODE" /Fp"ARMV4IDbg/ShowDeviceUUID.pch" /Yu"stdafx.h" /Fo"ARMV4IDbg/" /Fd"ARMV4IDbg/" /QRarch4T /QRinterwork-return /MC /c 
"E:\DAB_SDK\ShowDeviceUUID\ShowDeviceUUID.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\NASa\LOCALS~1\Temp\RSPC1.tmp" 
Creating temporary file "C:\DOCUME~1\NASa\LOCALS~1\Temp\RSPC2.tmp" with contents
[
/nologo /W3 /Zi /Od /D "DEBUG" /D "ARM" /D "_ARM_" /D "ARMV4I" /D UNDER_CE=500 /D _WIN32_WCE=500 /D "WCE_PLATFORM_MALATA_CAR_NAV" /D "UNICODE" /D "_UNICODE" /Fp"ARMV4IDbg/ShowDeviceUUID.pch" /Yc"stdafx.h" /Fo"ARMV4IDbg/" /Fd"ARMV4IDbg/" /QRarch4T /QRinterwork-return /MC /c 
"E:\DAB_SDK\ShowDeviceUUID\StdAfx.cpp"
]
Creating command line "clarm.exe @C:\DOCUME~1\NASa\LOCALS~1\Temp\RSPC2.tmp" 
Creating temporary file "C:\DOCUME~1\NASa\LOCALS~1\Temp\RSPC3.tmp" with contents
[
commctrl.lib coredll.lib /nologo /base:"0x00010000" /stack:0x10000,0x1000 /entry:"WinMainCRTStartup" /incremental:yes /pdb:"ARMV4IDbg/ShowDeviceUUID.pdb" /debug /nodefaultlib:"libc.lib /nodefaultlib:libcd.lib /nodefaultlib:libcmt.lib /nodefaultlib:libcmtd.lib /nodefaultlib:msvcrt.lib /nodefaultlib:msvcrtd.lib" /out:"ARMV4IDbg/ShowDeviceUUID.exe" /subsystem:windowsce,5.00 /MACHINE:THUMB 
.\ARMV4IDbg\ShowDeviceUUID.obj
.\ARMV4IDbg\StdAfx.obj
.\ARMV4IDbg\ShowDeviceUUID.res
]
Creating command line "link.exe @C:\DOCUME~1\NASa\LOCALS~1\Temp\RSPC3.tmp"
<h3>Output Window</h3>
Compiling resources...
Compiling...
StdAfx.cpp
Compiling...
ShowDeviceUUID.cpp
Linking...
corelibc.lib(pegwmain.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
corelibc.lib(crt0dat.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info
corelibc.lib(crt0init.obj) : warning LNK4209: debugging information corrupt; recompile module; linking object as if no debug info




<h3>Results</h3>
ShowDeviceUUID.exe - 0 error(s), 3 warning(s)
</pre>
</body>
</html>
