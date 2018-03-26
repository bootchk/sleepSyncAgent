
#pragma once

#include "../types.h"

typedef void (*Task)(void);


class TaskTimer {
public:
	static void schedule(Task task, DeltaTime duration);
};
