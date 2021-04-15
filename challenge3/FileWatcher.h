

#include <functional>
#include <string>
#include <filesystem>

struct FileWatcher {

	enum class FileSystemOperation {
		FileCreated,
		FileModified,
		FileDeleted
	};

	/**
	 * data given to callback when callback fired
	 */
	struct FileSystemModificationData {
		std::string path;
		FileSystemOperation operation;
	};


	typedef std::function<void(FileSystemModificationData)> FileSystemModificationCallback;
	
	FileWatcher();
	~FileWatcher();

	void SetModificationCallback(FileSystemModificationCallback&& callback);

	void StartWatching();
	void StopWatching();


	/**
	 * Create a FileWatcher for the given directory
	 */
	static FileWatcher OpenFor(std::filesystem::path& path);

private:
	struct Impl;
	Impl* impl{};
};