#include <FileWatcher.h>

#define _WIN32_LEAN_AND_MEAN
#define _WIN32_WINNT 0x0601 // 6.1 - 7
#include <windows.h>

struct FileWatcher::Impl {
	// implementation class for win32

	void SetPath(std::filesystem::path& path) {
		this->path = path;
	}

	static DWORD StartWatchingThreadImpl(LPVOID thing) {
		FileWatcher::Impl* impl = (FileWatcher::Impl*)thing;

		/*
		char bruh[260];
		WideCharToMultiByte(CP_ACP, 0, impl->path.c_str(), lstrlenW(impl->path.c_str()), bruh, sizeof(bruh), NULL, NULL);
		std::printf("reeee %s\n", bruh);
		*/

		impl->hDir = CreateFileW(impl->path.c_str(), GENERIC_READ | FILE_LIST_DIRECTORY, FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE, NULL, OPEN_EXISTING, FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED, NULL);

		if (impl->hDir == INVALID_HANDLE_VALUE) {
			// couldn't open directory
			return 1;
		}

		char buf[2048]; // crusty ass fucking api

		//char oldName[MAX_PATH];
		//char newName[MAX_PATH];

		char filename[MAX_PATH];

		DWORD nRet;
		BOOL result = TRUE;
		OVERLAPPED PollingOverlap;

		FILE_NOTIFY_INFORMATION* pNotify;
		PollingOverlap.OffsetHigh = 0;
		PollingOverlap.hEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

		impl->watching = true;

		while (result && impl->watching) {

			result = ReadDirectoryChangesW(
				impl->hDir,// handle to the directory to be watched
				&buf,// pointer to the buffer to receive the read results
				sizeof(buf),// length of lpBuffer
				TRUE,// flag for monitoring directory or directory tree
				FILE_NOTIFY_CHANGE_FILE_NAME |
				FILE_NOTIFY_CHANGE_DIR_NAME |
				FILE_NOTIFY_CHANGE_SIZE,
				//FILE_NOTIFY_CHANGE_LAST_WRITE |
				//FILE_NOTIFY_CHANGE_LAST_ACCESS |
				//FILE_NOTIFY_CHANGE_CREATION,
				&nRet,// number of bytes returned
				&PollingOverlap,// pointer to structure needed for overlapped I/O
				NULL);

			WaitForSingleObject(PollingOverlap.hEvent, INFINITE);;

			int offset = 0;
			bool err = false;

			do {
				pNotify = (FILE_NOTIFY_INFORMATION*)((char*)buf + offset);
				//strcpy(filename, "");
				int filenamelen = WideCharToMultiByte(CP_ACP, 0, pNotify->FileName, pNotify->FileNameLength / 2, filename, sizeof(filename), NULL, NULL);
				filename[pNotify->FileNameLength / 2] = '\0';

				FileWatcher::FileSystemModificationData data;

				data.path = filename;

				switch (pNotify->Action)
				{
				case FILE_ACTION_ADDED:
					//printf("\nThe file is added to the directory: [%s] \n", filename);
					data.operation = FileWatcher::FileSystemOperation::FileCreated;
					break;
				case FILE_ACTION_REMOVED:
					//printf("\nThe file is removed from the directory: [%s] \n", filename);
					data.operation = FileWatcher::FileSystemOperation::FileDeleted;
					break;
				case FILE_ACTION_MODIFIED:
					//printf("\nThe file is modified. This can be a change in the time stamp or attributes: [%s]\n", filename);
					data.operation = FileWatcher::FileSystemOperation::FileModified;
					break;
					/**
				case FILE_ACTION_RENAMED_OLD_NAME:
					printf("\nThe file was renamed and this is the old name: [%s]\n", filename);
					break;
				case FILE_ACTION_RENAMED_NEW_NAME:
					printf("\nThe file was renamed and this is the new name: [%s]\n", filename);
					break;
					*/
				default:
					//printf("\nDefault error.\n");
					err = true;
					break;
				}
				
				offset += pNotify->NextEntryOffset;

				if (impl->cb && !err)
					impl->cb(data);

			} while (pNotify->NextEntryOffset); //(offset != 0);
		}

		// clean up handles once we stop watching
		CloseHandle(PollingOverlap.hEvent);
		CloseHandle(impl->hDir);
		return 0;
	}

	void StartWatchingImpl() {
		// create impl thread
		if(hThread == INVALID_HANDLE_VALUE) {
			hThread = CreateThread(NULL, 0, &FileWatcher::Impl::StartWatchingThreadImpl, (LPVOID)this, 0, NULL);
		}
	}

	void StopWatchingImpl() {
		if (!watching)
			return;

		watching = false;

		// Wait for the thread to gracefully exit before continuing
		WaitForSingleObject(hThread, INFINITE);
		hThread = INVALID_HANDLE_VALUE;
		//TerminateThread(hThread, 9);
	}

	bool IsWatching() const {
		return watching;
	}

	FileWatcher::FileSystemModificationCallback cb;
	std::filesystem::path path;

	bool watching;

	// win32 garbage
	HANDLE hDir;
	HANDLE hThread = INVALID_HANDLE_VALUE;
};

// the true block inside of this beautified if statement is safe for any code touching the impl to run
#define FILEWATCHER_IMPL_GUARD() \
	if(impl != nullptr)

FileWatcher::FileWatcher() {
	impl = new Impl();
}

FileWatcher::~FileWatcher() {
	FILEWATCHER_IMPL_GUARD() {
		impl->StopWatchingImpl();
		delete impl;
	}
}

void FileWatcher::SetModificationCallback(FileSystemModificationCallback&& callback) {
	FILEWATCHER_IMPL_GUARD() {
		impl->cb = std::move(callback);
	}
}

void FileWatcher::StartWatching() {
	FILEWATCHER_IMPL_GUARD() {
		impl->StartWatchingImpl();
	}
}

void FileWatcher::StopWatching() {
	FILEWATCHER_IMPL_GUARD() {
		impl->StartWatchingImpl();
	}
}

FileWatcher FileWatcher::OpenFor(std::filesystem::path& path) {
	FileWatcher watcher;
	watcher.impl->SetPath(path);
	return watcher;
}