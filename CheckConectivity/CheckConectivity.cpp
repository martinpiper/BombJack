#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <string>
#include <map>
#include <set>

int main(int argc, char **argv)
{
	std::map<std::string, std::string> nameToNet;
	std::multimap<std::string, std::string> componentPinToName;
	std::set<std::string> outputNetName;
	std::set<std::string> finalReport;

	std::string line;

	if (argc >= 4)
	{
		std::ifstream fileFilter(argv[3]);
		if (!fileFilter.is_open())
		{
			printf("Error opening: %s", argv[3]);
			return -1;
		}

		while (getline(fileFilter, line))
		{
			size_t pos;
			while ( (pos = line.find_first_of(" \t\n")) != std::string::npos)
			{
				line.erase(pos, 1);
			}
			if (!line.empty())
			{
				outputNetName.insert(line);
			}
		}
	}


	std::ifstream fileIPC(argv[1]);
	if (!fileIPC.is_open())
	{
		printf("Error opening: %s", argv[1]);
		return - 1;
	}

	bool enableNameFetching = false;

	while (getline(fileIPC, line))
	{
		if (line.at(0) == 'C' && line.at(1) == ' ')
		{
			enableNameFetching = true;
			continue;
		}

		if (enableNameFetching && line.at(0) == 'P' && line.at(1) == ' ')
		{
			size_t pos = line.find_first_of(' ');
			line = line.substr(pos);
			pos = line.find_first_not_of(' ');
			line = line.substr(pos);
			pos = line.find_first_of(' ');

			std::string name = line.substr(0, pos);
			line = line.substr(pos);

			pos = line.find_first_not_of(' ');
			std::string net = line.substr(pos);

			nameToNet.insert(std::pair<std::string,std::string>(name , net));
			continue;
		}

		if (enableNameFetching && line.at(0) == '3' && line.at(1) == '1' && line.at(2) == '7')
		{
			line = line.substr(3);
			size_t pos = line.find_first_of(' ');
			std::string name = line.substr(0,pos);
			line = line.substr(pos);

			pos = line.find_first_not_of(' ');
			line = line.substr(pos);
			pos = line.find_first_of('-');
			line.replace(pos, 1, ":");
			pos = line.find_first_of(' ', pos);

			std::string component = line.substr(0, pos);
			while ((pos = component.find(' ')) != std::string::npos)
			{
				component.erase(pos,1);
			}


			componentPinToName.insert(std::pair<std::string, std::string>(component, name));
			continue;
		}

	}

	fileIPC.close();

	std::ifstream fileProductionCheck(argv[2]);
	if (!fileProductionCheck.is_open())
	{
		printf("Error opening: %s", argv[2]);
		return -1;
	}

	const std::string missingConnection = "Missing connection:";
	while (getline(fileProductionCheck, line))
	{
		size_t pos;
		if ( (pos = line.find(missingConnection)) != std::string::npos)
		{
			line = line.substr(pos + missingConnection.length());

			while ((pos = line.find(' ')) != std::string::npos)
			{
				line.erase(pos, 1);
			}

			pos = line.find_first_of(',');
			std::string component1 = line.substr(0, pos);
			std::string component2 = line.substr(pos + 1);

			std::multimap<std::string, std::string>::iterator component1Iterator = componentPinToName.lower_bound(component1);
			std::multimap<std::string, std::string>::iterator component1IteratorEnd = componentPinToName.upper_bound(component1);
			while (component1Iterator != component1IteratorEnd)
			{
				std::multimap<std::string, std::string>::iterator component2Iterator = componentPinToName.lower_bound(component2);
				std::multimap<std::string, std::string>::iterator component2IteratorEnd = componentPinToName.upper_bound(component2);

				while (component2Iterator != component2IteratorEnd)
				{
					std::string name1 = component1Iterator->second;
					std::string name2 = component2Iterator->second;

					std::string net1 = nameToNet.find(name1)->second;
					std::string net2 = nameToNet.find(name2)->second;

					std::string runningReport = "Missing: " + net1 + " , " + net2;
					while (runningReport.length() < 60)
					{
						runningReport.append(" ");
					}

					runningReport.append(" From: " + component1 + " , " + component2);

					std::cout << runningReport << std::endl;

					pos = net1.find_last_of('.');
					net1 = net1.substr(0, pos);
					pos = net2.find_last_of('.');
					net2 = net2.substr(0, pos);

					if (net1.compare(net2) == 0)
					{
						bool inserted = outputNetName.insert(net1).second;
						if (inserted)
						{
							while (net1.length() < 20)
							{
								net1.append(" ");
							}
							finalReport.insert(net1 + " From: " + component1.c_str() + "," + component2.c_str());
						}
					}

					component2Iterator++;
				}
				component1Iterator++;
			}
		}
	}

	std::cout << std::endl << std::endl << "Final report" << std::endl;

	std::set<std::string>::iterator lineIter = finalReport.begin();
	while(lineIter != finalReport.end())
	{
		std::cout << *lineIter++ << std::endl;
	}

	fileProductionCheck.close();
	return -1;
}
