///////debug.cpp
#include "stdafx.h"
#include <afx.h>

#include "Debug.h"
#include <tchar.h>

#define MAX_DEBUG_STRING 8192 * 2
#define MAX_REGISTRY_NAME 255
#define MAX_FILE_PATH 32767



VOID OutputDebug(LPWSTR strFormat, ...)
{
	va_list vArgs;

	va_start(vArgs, strFormat);
	LPWSTR strDebug = (LPWSTR) malloc((MAX_DEBUG_STRING + 2) * sizeof(WCHAR)); // do not use zalloc here
	SecureZeroMemory(strDebug, (MAX_DEBUG_STRING + 2) * sizeof(WCHAR));
	vswprintf_s(strDebug, MAX_DEBUG_STRING, strFormat, vArgs);
	OutputDebugString(strDebug);

	free(strDebug);
}

/* simple hexdump, taken from stackoverflow :) */
VOID HexDump(WCHAR *desc, VOID *addr, UINT len) {
	DWORD i;
	unsigned char buff[17];
	unsigned char *pc = (unsigned char*)addr;

	// Output description if given.
	if (desc != NULL)
		OutputDebug(L"%s:\n", desc);

	// Process every byte in the data.
	for (i = 0; i < len; i++) {
		// Multiple of 16 means new line (with line offset).

		if ((i % 16) == 0) {
			// Just don't print ASCII for the zeroth line.
			if (i != 0)
				OutputDebug(L"  %S\n", buff);

			// Output the offset.
			OutputDebug(L"  %04x ", i);
		}

		// Now the hex code for the specific character.
		OutputDebug(L" %02x", pc[i]);

		// And store a printable ASCII character for later.
		if ((pc[i] < 0x20) || (pc[i] > 0x7e))
			buff[i % 16] = '.';
		else
			buff[i % 16] = pc[i];
		buff[(i % 16) + 1] = '\0';
	}

	// Pad out last line if not exactly 16 characters.
	while ((i % 16) != 0) {
		OutputDebug(L"   ");
		i++;
	}

	// And print the final ASCII bit.
	OutputDebug(L"  %S\n", buff);
}

