#include <bitset>
#include <array>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <cassert>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <sstream>
#include <cstdint>
#include <filesystem>
#include <mutex>

#define _WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "EndianSwap.h"
#include "FileWatcher.h"

#if defined(_WIN32)  && defined(_MSC_VER)
// fucking retarded microsoft UB-names (underscore names are compiler reserved) everything posixy the movie
#define execvp _execvp
#endif

namespace fs = std::filesystem;

std::string printByteArray(const std::vector<std::uint8_t>& vec)
{
	auto nLength = vec.size();
	auto* buf = (std::uint8_t*)vec.data();
	constexpr std::uint8_t nibbleCount = 2;
	std::string str(nibbleCount * nLength * 2, '\0');
	//std::stringstream ss;

	for (int i = 0; i < nLength; i++)
	{
		//char temp[4];
		sprintf(&str[nibbleCount * i], "%02X", buf[i]);
		//ss << &temp[0] << ' ';
	}

	return str;
	//return ss.str();
}

inline int roundUp(int numToRound, int multiple)
{
	assert(multiple && ((multiple & (multiple - 1)) == 0));
	return (numToRound + multiple - 1) & -multiple;
}

inline std::vector<std::uint8_t> ReadAllOf(std::istream& is) {
	std::vector<std::uint8_t> data;

	// return an empty vector if the stream is bad
	if (!is || is.eof())
		return {};

	is.seekg(0, std::istream::end);
	auto len = is.tellg();
	is.seekg(0, std::istream::beg);

	data.resize(len);
	is.read((char*)&data[0], len);

	return data;
}

static std::uint8_t rand_div;
static std::uint8_t rand_add;
static std::uint8_t rand_sub;

std::uint16_t poke_rand() {
	// ignore for "now"

	// do seed check instead
	// no two bytes of the seed can be identical, they are always going to be different to each other
}


int main()
{
	// unknown
	std::vector<std::vector<std::uint8_t>> toSearch = { {0x0B, 0x0B, 0x0B, 0x74, 0x0A}, {0x0F, 0x0B, 0x0F, 0x0A, 0x0A}, {0x0F, 0x0F, 0x0A, 0x0A, 0x0B}, {0x0F, 0x0B, 0x0A, 0x0A, 0x0A} };

	// seed 0x00000001
	//std::vector<std::vector<std::uint8_t>> toSearch = { {0x0B, 0x0A, 0x0F, 0x0F, 0x0F}, {0x0B, 0x7A, 0x7A, 0x0F, 0x0F}, {0x7A, 0x7A, 0x0A, 0x33, 0x0F}, {0x0A, 0x0A, 0x0A, 0x74, 0x0F} };

	// seed 0x000002E1
	//std::vector<std::vector<std::uint8_t>> toSearch = { {0x0F, 0x6F, 0x0B, 0x0A, 0x74}, {0x0F, 0x0F, 0x7A, 0x0A, 0x0F}, {0x0F, 0x0F, 0x0A, 0x0B, 0x0B}, {0x0A, 0x0A, 0x74, 0x7A, 0x0A} };

	// seed 0xC0000281
	//std::vector<std::vector<std::uint8_t>> toSearch = { {0x7A, 0x7A, 0x0B, 0x0A, 0x0B}, {0x0F, 0x0A, 0x0B, 0x74, 0x0B}, {0x0F, 0x0F, 0x0A, 0x0B, 0x0F}, {0x6C, 0x0A, 0x0A, 0x74, 0x6E} };

	int startLine = 2;

	//printf("test beeff %d\n", 0x69420);

	std::string stateTemplate = "crackertemplate.sna";

	std::ifstream start_save(stateTemplate, std::ios::binary);

	std::mutex bufLock; // lock on the buffer. Whatever thread has this is free to go
	auto buf = ReadAllOf(start_save);

	if (buf.empty()) {
		std::printf("uhhhuh\n");
	}
	else {
		std::printf("[*] Loaded template save file \"%s\" into memory.\n", stateTemplate.c_str());

		// idiot check #1
		if (buf[0] != 'B' && buf[1] != 'G' && buf[2] != 'B' && buf[3] != '1') {
			std::printf("[!] Template save file \"%s\" is probably invalid.\n", stateTemplate.c_str());
		}
	}

	start_save.close();

	int loops = 0;

	auto path = std::filesystem::current_path() / "save";

	std::atomic<std::uint64_t> seeds_processed = 0;

	std::vector<std::array<std::uint8_t, 0x240>> chunks;

	auto runner = [&](std::uint32_t start, std::uint32_t end) {

		auto id = std::this_thread::get_id();

		std::cout << "[*] Thread " << id << " starting..." << std::endl;

		std::stringstream ss;
		std::stringstream ssEnd;
		std::stringstream ssBgb;

		ss << "save\\crackerstart-" << id << ".sna";
		ssEnd << "save\\crackerend-" << id << ".sna";

		/*
		std::vector<char*> bgb_argv;

		bgb_argv.push_back("-rom");
		bgb_argv.push_back(ss.str().data());
		bgb_argv.push_back(ss.str().data());


		bgb_argv.push_back("-hf");
		bgb_argv.push_back("-autoexit");
		bgb_argv.push_back("-stateonexit");
		bgb_argv.push_back(ssEnd.str().data());

		bgb_argv.push_back("-nobatt");
		bgb_argv.push_back("-br");
		bgb_argv.push_back("02:A572");
		*/

		ssBgb << "..\\..\\bgb.exe -rom " << ss.str() << " -hf -autoexit -stateonexit " << ssEnd.str() << " -nobatt -br 02:A572";

		FileWatcher f = FileWatcher::OpenFor(path);

		std::atomic_bool killbgb = false;

		f.SetModificationCallback([&](FileWatcher::FileSystemModificationData data) {
			if (data.operation == FileWatcher::FileSystemOperation::FileModified) {
				if (data.path == ssEnd.str().substr(5)) {
					//std::printf("THEIN THREAD SAVE HAS BEEN WRITTEN\n");
					killbgb.store(true);
				}
			}
			});

		f.StartWatching();

		//std::cout << "[*] Save filename: \"" << ss.str() << "\"\n";
		//std::cout << "[*] End Save filename: \"" << ssEnd.str() << "\"\n";
		//std::cout << "[*] BGB Command Line: \"" << ssBgb.str() << "\"\n";

		std::ofstream starter(ss.str(), std::ios::out | std::ios::binary);

		auto write_seed = [&](std::uint32_t seed) {
			// lock the mutex here cause we need to
			//std::lock_guard<std::mutex> lkBuf(bufLock);
			auto bufCopy = buf;

			//if (&bufCopy != nullptr)
			{
				if (!starter) {
					starter.open(ss.str(), std::ios::out | std::ios::binary);
				}

				//std::cout << "[*] Writing seed " << std::hex << seed << std::endl;

				// Write the seed into the save file buffer
				//seed = 0xFACEFACC;
				memcpy(&bufCopy[0x1EDD], &seed, sizeof(std::uint32_t));
				starter.seekp(0, std::ofstream::beg);

				//std::cout << "WHORE" << bufCopy.size();

				starter.write((char*)&bufCopy[0], bufCopy.size());

				//starter.flush();

				starter.close();
			}


		};

		auto retrieve_chunks = [&]()->std::array<std::uint8_t, 0x240> {
			std::array<std::uint8_t, 0x240> ret;

			std::ifstream end(ssEnd.str(), std::ifstream::binary);

			end.seekg(0x1C25, std::ifstream::beg);

			end.read((char*)&ret[0], 0x240);

			//			for (int y = 24; y >= 0; y--)
			//			{
			//				for (int x = 0; x < 24; x++)
			//				{
			//					int i = (x + (y * 24));
			//					end.seekg(0x1C25 + i, std::ifstream::beg);

			//					end.read((char*)&ret[i], 1);
			//				}
			//			}

			return ret;
		};

		for (std::uint32_t seed = start + 1; seed < end; seed += 16)
		{
			// perform the mario
			loops++;

			//std::cout << "seed: " << std::hex << seed << ", seed < " << end;

			write_seed(EndianSwap(seed));

			bool shouldSkip = false;
			std::uint8_t seedSplit[4];
			seedSplit[0] = (seed & 0xFF000000) >> 24;
			seedSplit[1] = (seed & 0x00FF0000) >> 16;
			seedSplit[2] = (seed & 0x0000FF00) >> 8;
			seedSplit[3] = (seed & 0x000000FF);

			//std::cout << "[*] split: " << std::hex << seedSplit[0] << seedSplit[1] << seedSplit[2] << seedSplit[3] << std::endl;

			for (int i = 0; i < 4; ++i)
			{
				for (int j = 0; j < 4; ++j)
				{
					if (i != j && seedSplit[i] == seedSplit[j])
					{
						shouldSkip = true;
						//std::cout << "[*] skipping " << std::hex << seed << std::endl;
					}
				}
			}

			if (shouldSkip)
				continue;

			//std::cout << "[*] (Thread ID " << std::dec << id << ") Running BGB for seed " << std::hex << seed << std::endl;

			//system("..\\..\\bgb -rom crackerstart.sna");
			//system(ssBgb.str().c_str());
			//execvp(ssBgb.str().c_str(), null);
			//execvp("..\\..\\bgb.exe", bgb_argv.data());

			STARTUPINFOA sa;
			PROCESS_INFORMATION pi;
			memset(&sa, 0x00, sizeof(sa));
			memset(&pi, 0x00, sizeof(pi));

			if (!CreateProcessA(nullptr, ssBgb.str().data(), NULL, NULL, FALSE, 0, NULL, NULL, &sa, &pi)) {
				std::cout << "FUCK\n";
			}

			while (killbgb.load() == false) {
				std::this_thread::sleep_for(std::chrono::milliseconds(25));
			}

			//std::cout << "[*] (Thread ID " << std::dec << id << ") Killing BGB since it made state for seed " << std::hex << seed << std::endl;

			TerminateProcess(pi.hProcess, 9);
			killbgb.store(false);

			//std::cout << "seed: " << std::hex << seed << std::endl;

			seeds_processed.fetch_add(1);

			// Once we're here the end save has been made. Let's look at it.
			auto chunks = retrieve_chunks();

			//int bufferOffset = (x + chunkX) + ((y + chunkY) * 24);
			//endSearch = toSearch[y][x] == chunks[bufferOffset];

			int goalNumber = toSearch.size() * toSearch[0].size();
			int matchCount = 0;
			bool stop = false;

			if (seed == 0xC0000281)
			{
				for (size_t i = 0; i < 20; i++)
				{
					std::cout << "RUNNING CHECK! ";
				}
			}

			for (int chunkX = 8; chunkX < 8 + 8 && !stop; chunkX++)
			{
				for (int chunkY = 8 - startLine; chunkY < 8 + 8 && !stop; chunkY++)
				{
					matchCount = 0;

					for (int y = 0; y < toSearch.size(); y++)
					{
						for (int x = 0; x < toSearch[y].size(); x++)
						{
							int bufferOffset = (x + chunkX) + ((y + chunkY) * 24);
							if (toSearch[y][x] == chunks[bufferOffset])
							{
								matchCount++;
							}


						}
					}

					if (matchCount == goalNumber)
					{
						std::cout << "\b\n\nMONEYSHOT: " << std::hex << seed << " thread " << std::dec << id << std::endl;
						std::vector<std::uint8_t> myvector(chunks.begin(), chunks.end());
						std::cout << "\n\n" << printByteArray(myvector);
						exit(0);
					}
				}
			}

			if (seed == 0xC0000281)
			{
				for (size_t i = 0; i < 20; i++)
				{
					std::cout << "CHECK IS DONE! ";
				}
			}
		}
	};

	std::vector<std::thread> threads;

	auto count = std::thread::hardware_concurrency();
	//auto count = 2u;
	auto roundedCount = roundUp(UINT_MAX / count, 0x10);

	std::cout << "[*] Each thread gets " << std::hex << roundedCount / 16 << " seeds" << std::endl;

	for (auto i = 0u; i < count; ++i) {
		std::cout << "[*] Spinning up thread " << i << "..." << std::endl;

		//std::uint32_t threadStartOffset = ;
		//std::uint32_t threadEndOffset = (UINT_MAX/16)*i+1;

		threads.push_back(std::thread(runner, (roundedCount)*i, (roundedCount) * (i + 1)));
	}

	// wait for all the threads to exit
	for (auto& t : threads) {
		try {
			t.detach();
		}
		catch (const std::exception& e) {
			std::cout << "FUCK: " << e.what() << '\n';
		}
	}

	std::cout << "\n\n\n";
	std::uint32_t total_processed = 0;
	const std::uint32_t totalSeeds = 16 * 16 * 16 * 16 * 16 * 16 * 16;

	while (true) {
		total_processed += seeds_processed.load();
		std::cout << "[*] Seeds per second: " << std::dec << seeds_processed.load() << ", Total: " << total_processed /*<< " (" << std::fixed << std::setprecision(3) << (float)total_processed/totalSeeds << "%)"*/ << "    " << '\r';

		seeds_processed.store(0);
		Sleep(1000);
	}

	printf("loops: %d", loops);
}
