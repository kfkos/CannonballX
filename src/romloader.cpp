/***************************************************************************
    Binary File Loader. 
    
    Handles loading an individual binary file to memory.
    Supports reading bytes, words and longs from this area of memory.

    Copyright Chris White.
    See license.txt for more details.
***************************************************************************/

#include <iostream>
#include <fstream>
#include <cstddef>       // for std::size_t
#include <boost/crc.hpp> // CRC Checking via Boost library.
#include <map>
#include <Xtl.h>

#include "stdint.hpp"
#include "romloader.hpp"
#include "frontend/config.hpp"

// Map to store contents of directory by CRC 32 value. Similar to Hashmap.
static std::map<int, std::string> map;
static bool map_created;


RomLoader::RomLoader()
{
    rom = NULL;
    map_created = false;
    loaded = false;
}

RomLoader::~RomLoader()
{
    if (rom != NULL)
        delete[] rom;
}

void RomLoader::init(const uint32_t length)
{
    // Setup pointer to function load_crc32
    load = &RomLoader::load_crc32;

    this->length = length;
    rom = new uint8_t[length];
}

void RomLoader::unload(void)
{
    delete[] rom;
    rom = NULL;
}

// --------------------------------------------------------------------------------------------
// Create Map of files in ROM directory by CRC32 value
// This should be faster than brute force searching every file in the directory every time.
// --------------------------------------------------------------------------------------------

int RomLoader::create_map()
{
    map_created = true;
	
	std::string path = "D:\\" + config.data.rom_path + "\\*";
	int size = path.size();
	char dir[100]; // TO-DO (kfkos): hardcoded wert durch dynamische speicherallokierung ersetzen
	strcpy(dir, path.c_str());

	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	hFind = FindFirstFile(dir, &wfd);

	if (INVALID_HANDLE_VALUE == hFind) {
		OutputDebugString("WARNING: Could not open ROM directory");
	} else {
		OutputDebugString("INFO: Successfully opened ROM directory");
		do
        {
            // Process file
			std::string file = "D:\\" + config.data.rom_path + "\\" + wfd.cFileName;
			char* buffer = new char[length];
			DWORD bytesRead;
			HANDLE hFile = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
			ReadFile(hFile, buffer, length, &bytesRead, NULL);

			// Check CRC on file
			boost::crc_32_type result;
			result.process_bytes(buffer, (size_t) bytesRead);

			// Insert file into MAP between CRC and filename
			map.insert(std::pair<int, std::string>(result.checksum(), file));

			// Clean Up
			delete[] buffer;
			CloseHandle(hFile);
        } while( FindNextFile( hFind, &wfd ) );

        // Close the find handle.
        FindClose( hFind );
	}

    if (map.empty())
        OutputDebugString("WARNING: Could not create CRC32 Map. Did you copy the ROM files into the directory?");

    return 0; //success
}


// ------------------------------------------------------------------------------------------------
// Search and load ROM by CRC32 value as opposed to filename.
// Advantage: More resilient to renamed romsets.
// ------------------------------------------------------------------------------------------------
int RomLoader::load_crc32(const char* debug, const int offset, const int length, const int expected_crc, const uint8_t interleave, const bool verbose)
{
    if (!map_created)
        create_map();

    if (map.empty())
        return 1;
	
    std::map<int, std::string>::iterator search = map.find(expected_crc);

    // Cannot find file by CRC value in map
    if (search == map.end())
    {
        if (verbose) std::cout << "Unable to locate rom in path: " << config.data.rom_path << " possible name: " << debug << " crc32: 0x" << std::hex << expected_crc << std::endl;
		loaded = false;
        return 1;
    }

    // Correct ROM found
    std::string file = search->second;

    // Process file
	char* buffer = new char[length];
	DWORD bytesRead;
	HANDLE hFile = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	ReadFile(hFile, buffer, length, &bytesRead, NULL);

	// Interleave file as necessary
    for (int i = 0; i < length; i++)
        rom[(i * interleave) + offset] = buffer[i];

	// Clean Up
	delete[] buffer;
	CloseHandle(hFile);
	loaded = true;

    return 0; // success
}

// --------------------------------------------------------------------------------------------
// Load Binary File (LayOut Levels, Tilemap Data etc.)
// --------------------------------------------------------------------------------------------

int RomLoader::load_binary(const char* filename)
{
    std::string file = "D:\\";
	file.append(filename);
	char dir[100]; // TO-DO (kfkos): hardcoded wert durch dynamische speicherallokierung ersetzen
	strcpy(dir, file.c_str());

	WIN32_FIND_DATA wfd;
	HANDLE hFind;

	hFind = FindFirstFile(dir, &wfd);

	if (INVALID_HANDLE_VALUE == hFind) {
		OutputDebugString("WARNING: Could not open RES directory");
		return 1;
	} else {
		OutputDebugString("INFO: Successfully opened RES directory");
        // Process file
		std::string file = "D:\\" + config.data.res_path + "" + wfd.cFileName;
		char* buffer = new char[wfd.nFileSizeLow];
		DWORD bytesRead;
		HANDLE hFile = CreateFile(file.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
		ReadFile(hFile, buffer, wfd.nFileSizeLow, &bytesRead, NULL);
		rom = (uint8_t*) buffer;
		
		// Clean Up
		delete[] buffer;
		CloseHandle(hFile);
        FindClose(hFind);
	}

	return 0;
}