#ifndef ARTIFACT_HPP
#define ARTIFACT_HPP

#include <string>
#include <algorithm>

enum Artifact {
	FIFO,
	ROUNDROBIN,
	CUSTOM
};

Artifact ArtifactFromString(std::string);
std::string ArtifactToString(Artifact);

#endif