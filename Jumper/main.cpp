#include <iostream>
#include <string>
#include "jump.h"

#include <Windows.h>

int main(int argc, char * argv[])
{
	jump::url_data urldata;

	if (argc < 2) {
		std::cerr << "Usage: upload-jump-file.exe path/to/file1 [path/to/file2] [...]" << std::endl;
		return 1;
	}

	for (int i = 1; i < argc; i++) {

		std::string file = argv[i];
		std::string extension;

		if (file.find(".") == std::string::npos) {
			extension = ".txt";
		}
		else {
			extension = file.substr(file.find_last_of("."));
		}

		std::cout << "Trying to upload " << file << std::endl;

		try {
			urldata = jump::get_upload_url_s();
			std::cout << "    Got upload URL " << std::endl;
		}
		catch (jump::JumpException je) {
			std::cerr << je.what() << std::endl;
			continue;
		}

		try {
			jump::upload_file(urldata.url, file, urldata.content_type);
			std::cout << "    Uploaded file" << std::endl;
		}
		catch (jump::JumpException je) {
			std::cerr << je.what() << std::endl;
			continue;
		}

		try {
			urldata = jump::gen_jump_file_url_s(urldata.tmp_key, extension);
			std::cout << "    Got jump.wtf URL: " << urldata.url << std::endl;
			std::cout << "    CDN URL: " << urldata.cdn_url << std::endl;
		}
		catch (jump::JumpException je) {
			std::cerr << je.what() << std::endl;
			continue;
		}

		const char* output = urldata.cdn_url.c_str();
		std::cout << output << "\n";
		const size_t len = strlen(output) + 1;
		HGLOBAL hMem = GlobalAlloc(GMEM_MOVEABLE, len);
		memcpy(GlobalLock(hMem), output, len);
		GlobalUnlock(hMem);
		OpenClipboard(0);
		EmptyClipboard();
		SetClipboardData(CF_TEXT, hMem);
		CloseClipboard();
	}

	return 0;
}
