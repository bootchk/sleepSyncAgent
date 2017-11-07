
#pragma once

struct CliqueRecord {
	SystemID masterID;
	DeltaTime offsetToNextMastersSync;
	unsigned int countSlaves;
};
