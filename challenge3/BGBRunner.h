#include <atomic>

struct BGBRunner {


	void NotifyShouldKill();

private:
	std::atomic_bool kill_bgb;
};