
#pragma once

#include "../types.h"

typedef void (*Task)(void);


class Timer {
public:
	static void schedule(Task task, DeltaTime duration);
};
