#include <fstream>
#include <cstdlib>
#include <iostream>
#include <vector>
#include <string>
#include "raidRecover.h"

int main(int argc, char *argv[])
{
    std::string inPath;
    std::vector<std::string> paths;
    std::vector<std::string> devices;
    std::string outPath;
    int raidVersion = 6, stripeSize = -1, lostImages = -1;

    if (argc < 5)
    {
        std::cout << "\"Raid faster - recover better\"(v0.8.0) will recover/rebuild a damaged raidsystem " << std::endl;
        std::cout << "from remaining images. It handles Raid-0, Raid-1 and Raid-5." << std::endl;
        std::cout << "Copyright (C) 2014 crisbi - Christian Zoubek, Sabine Seufert" << std::endl << std::endl;
        std::cerr << "Try: './rfrb [-P PathToImageFolder] [-o PathToOutputFolder] ([-r #RaidVersion] [-s #Stripesize] [-n #NumberOfLostImages])'" << std::endl;
        std::cerr << "or './rfrb [-p PathToImage1] [-p PathToImage2] ... [-o PathToOutputFolder] ([-r #RaidVersion] [-s #Stripesize] [-n #NumberOfLostImages])'" << std::endl;
        std::cerr << "or './rfrb [-d DeviceName] [-d DeviceName2] ... [-o PathToOutputFolder] ([-r #RaidVersion] [-s #Stripesize] [-n #NumberOfLostImages])'" << std::endl;
        return 1;
    }

    bool in = false;
    bool out = false;
    bool dev = false;
    for (int i = 1; i < argc-1; i++)
    {
        std::string input = argv[i];
        if (input.compare("-h") == 0)
        {
            std::cout << "\"Raid faster - recover better\"(v0.8.0) will recover/rebuild a damaged raidsystem " << std::endl;
            std::cout << "from remaining images. It handles Raid-0, Raid-1 and Raid-5." << std::endl;
            std::cout << "Copyright (C) 2014 crisbi - Christian Zoubek, Sabine Seufert" << std::endl << std::endl;
            std::cerr << "Usage 1: './rfrb [-P PathToImageFolder] [-o PathToOutputFolder] ([-r #RaidVersion] [-s #Stripesize] [-n #NumberOfLostImages])'" << std::endl;
            std::cerr << "Usage 2: './rfrb [-p PathToImage1] [-p PathToImage2] ... [-o PathToOutputFolder] ([-r #RaidVersion] [-s #Stripesize] [-n #NumberOfLostImages])'" << std::endl;
            std::cerr << "Usage 3: './rfrb [-d DeviceName] [-d DeviceName2] ... [-o PathToOutputFolder] ([-r #RaidVersion] [-s #Stripesize] [-n #NumberOfLostImages])'" << std::endl;
            return 1;
        }
        if (input.compare("-P") == 0)
        {
            inPath = argv[i+1];
            in = true;
            std::cout << "Input Folder: " << inPath << std::endl;
        }
        else if (input.compare("-p") == 0)
        {
            paths.push_back(argv[i+1]);
            in = true;
            std::cout << "image: " << argv[i+1] << std::endl;
        }
        else if (input.compare("-o") == 0)
        {
            outPath = argv[i+1];
            out = true;
            std::cout << "Output Folder: " << outPath << std::endl;
        }
        else if (input.compare("-r") == 0)
        {
            raidVersion = atoi(argv[i+1]);
            std::cout << "RaidVersion: " << raidVersion << std::endl;
        }
        else if (input.compare("-s") == 0)
        {
            stripeSize = atoi(argv[i+1]);
            std::cout << "Stripesize: " << stripeSize << std::endl;
        }
        else if (input.compare("-n") == 0)
        {
            lostImages = atoi(argv[i+1]);
            std::cout << "Lost discs: " << lostImages << std::endl;
        }
        else if (input.compare("-d") == 0)
        {
            devices.push_back(argv[i+1]);
            dev = true;
            std::cout << "Device: " << argv[i+1] << std::endl;
        }
        else
        {
            continue;
        }
    }

    if (in == true && out == true && ((inPath.size() > 0 && paths.empty() && !dev) || (paths.size() > 0 && inPath == "" && !dev)))
    {
        std::cout << "Valid Input" << std::endl;
    }
    else if (dev == true && out == true && !in && paths.empty())
    {
        std::cout << "Valid Input with Devices" << std::endl;
    }
    else
    {
        std::cout << "Not a valid Input. Please try again." << std::endl;
        std::cout << "Please use: './rfrb [-P PathToImageFolder] [-o PathToOutput-Folder]'" << std::endl;
        std::cout << "or: './rfrb [-p PathToImage1] [-p PathToImage2] [-o PathToOutput-Folder]'" << std::endl;
        std::cout << "or: './rfrb [-d DeviceName] [-d DeviceName2] ... [-o PathToOutput-Folder]'" << std::endl;
        return 1;
    }

    RaidRecover *raidR;
    if (paths.empty() && !dev)
    {
        if (!(inPath.at(inPath.size()-1) == '/'))
            inPath += "/";
        if (!(outPath.at(outPath.size()-1) == '/'))
            outPath += "/";
        raidR = new RaidRecover(inPath, outPath);
    }
    else if (dev)
    {
        for (const auto& device : devices)
        {
            std::string devicePath = "/dev/" + device;
            paths.push_back(devicePath);
        }
        raidR = new RaidRecover(paths, outPath);
    }
    else
    {
        raidR = new RaidRecover(paths, outPath);
    }

    if (raidVersion == 0 || raidVersion == 1 || raidVersion == 5)
    {
        raidR->setRaid(raidVersion);
    }

    if (stripeSize >= 0)
    {
        raidR->setStripeSize(stripeSize);
    }

    if (lostImages >= 0)
    {
        raidR->setLostImages(lostImages);
    }

    std::cout << "Interpreted parameters. Now starting to analyse the Raid system." << std::endl;

    raidR->run();

    return 0;
}
