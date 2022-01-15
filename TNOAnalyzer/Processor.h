#pragma once
#include <string>
class Processor
{
private:
	bool GetPhysParams(std::string& sFileName);
	
public:

	void ReadDirectories();

	void SetPriority();
};