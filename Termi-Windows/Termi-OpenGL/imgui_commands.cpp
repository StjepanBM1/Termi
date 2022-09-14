/**
 * @author Andrej Bartulin
 * PROJECT: Termi-Windows version with OpenGL and Dear ImGui rendering system
 * LICENSE: ringwormGO General License 1.0 | (RGL) 2022
 * DESCRIPTION: Main file for implementing commands code
 * INFORAMTION: Compile solution, else check Victor Gordan's video
*/

#include "imgui_code.hpp"

using namespace std;

#pragma warning(disable : 4996)

#pragma comment(lib, "Advapi32.lib")
#pragma comment(lib, "Kernel32.lib")

/*
 * Commands main code
 * Return version of Windows operating system
*/
static const char* OperatingSystem()
{
    OSVERSIONINFO vi;
    vi.dwOSVersionInfoSize = sizeof(vi);
    if (GetVersionEx(&vi) == 0)
    {
        return 0;
    }

    switch (vi.dwPlatformId)
    {
        case VER_PLATFORM_WIN32s:
            return "Windows 3.x";
        case VER_PLATFORM_WIN32_WINDOWS:
            return vi.dwMinorVersion == 0 ? "Windows 95" : "Windows 98";
        case VER_PLATFORM_WIN32_NT:
            return "Windows NT";
        default:
            return "Unknown";
    }
}

/* Return uptime time in seoconds */
uint64_t UptimeS()
{
    return GetTickCount64() / 1000;
}

/* Return uptime time in minutes */
uint64_t UptimeM()
{
    return GetTickCount64() / 60000;
}

/* Return uptime time in hours */
uint64_t UptimeH()
{
    return GetTickCount64() / 3600000;
}

int base64(std::vector<std::string>& vect)
{
    vector<string> out;

    if (vect[1] == "-e")
    {
        for (auto x : vect)
        {
            out.push_back(base64_encode(x));
        }

        vector<decltype(out)::value_type>(out.begin() + 2, out.end()).swap(out);

        console.AddLog("Result:\n");

        for (auto y : out)
        {
            console.AddLog("%s", y.c_str());
        }

        return 0;
    }

    else if (vect[1] == "-d")
    {
        for (auto x : vect)
        {
            out.push_back(base64_decode(x));
        }

        vector<decltype(out)::value_type>(out.begin() + 2, out.end()).swap(out);

        console.AddLog("Result:\n");

        for (auto y : out)
        {
            console.AddLog("%s", y.c_str());
        }

        return 0;
    }

    else
    {
        console.AddLog("Unknown parametar '%s'! Returning 1...\n", vect[1].c_str());
        return 1;
    }
}

int cd(std::vector<std::string>& vect)
{
    return chdir(vect[1].c_str());
}

int change_setting(std::vector<std::string>& vect)
{
    int setting = stoi(vect[1]);

    try
    {
        if (setting != 5 && setting != 8)
        {
            return static_cast<int>(console.Settings(setting, stof(vect[2])));
        }

        else if (setting == 5)
        {
            console.startup_command = vect[2];
            console.Settings(5, 0);
        }

        else if (setting == 8)
        {
            console.font_name = vect[2];
            console.Settings(8, 0);
        }
    }

    catch (const std::exception& e)
    {
        console.AddLog("Catched exception. Exception result: %s", e.what());
        return 1;
    }
}

int echo(std::vector<std::string>& vect)
{
    if (isStarting(vect[1], "$"))
    {
        console.AddLog("Variables not supported yet!\n");
        return 1;
    }

    else
    {
        for (string x : vect)
        {
            console.AddLog("%s", x.c_str());
        }
    }

    return 0;
}

int find_command(std::vector<std::string>& vect)
{
    std::vector<std::string> out;

    string path;

    if (vect.size() < 2)
    {
        path = ".\\";
    }

    else
    {
        path = vect[1];
    }

    out.push_back("list");
    out.push_back(path);
    out.push_back("!");

    list_dir(out);

    return 2;
}

int list_dir(std::vector<std::string>& vect)
{
    struct dirent* d;
    struct stat dst;

    DIR* dr;
    string path;

    if (vect.size() < 2)
    {
        path = ".\\";
    }

    else
    {
        path = vect[1];
    }

    chdir(path.c_str());
    path = ".\\";

    dr = opendir(path.c_str());

    if (dr != NULL)
    {
        for (d = readdir(dr); d != NULL; d = readdir(dr))
        {
            string type = d->d_name;
            type = path + type;
            if (stat(type.c_str(), &dst) == 0)
            {
                if (dst.st_mode & S_IFDIR)
                {
                    type = "FOLDER";
                }

                else if (dst.st_mode & S_IFREG)
                {
                    type = "FILE";
                }
            }

            filename.push_back(string(d->d_name));
            filetype.push_back(type);

            console.AddLog("%s - %s", d->d_name, type.c_str());
        }

        /*for (auto x : filename)
        {
            console.AddLog("%s - %s", x);
        }*/

        //if (vect.size() != 3)
        //{
            filename.clear();
            filetype.clear();
        //}

        closedir(dr);
    }

    return 0;
}

int new_dir(std::vector<std::string>& vect)
{
    if (mkdir(vect[1].c_str()) == -1)
    {
        console.AddLog("Error while creating directory!\n");
        return 1;
    }

    else
    {
        console.AddLog("Directory %s created!\n", vect[1].c_str());
        return 0;
    }
}

int neofetch(std::vector<std::string>& vect)
{
    /* Username and computer name */
    TCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserName((TCHAR*)username, &username_len);

    TCHAR computer[UNLEN + 1];
    DWORD computer_len = UNLEN + 1;
    GetComputerName((TCHAR*)computer, &computer_len);

    char new_username[UNLEN];
    char new_computer[UNLEN];

    wcstombs(new_username, username, wcslen(username) + 1);
    wcstombs(new_computer, computer, wcslen(computer) + 1);

    /* Memory */
    MEMORYSTATUSEX statex;
    statex.dwLength = sizeof(statex);
    GlobalMemoryStatusEx(&statex);

    console.AddLog("\n");
    console.AddLog("\t %s @ %s\n", new_username, new_computer);
    console.AddLog("--------------------------------------------\n");

    console.AddLog("\n");
    console.AddLog("\tOperating system: %s\t\n", OperatingSystem());

    if (UptimeH() < 1)
    {
        console.AddLog("\tUptime : %d minutes\t\n", UptimeM());
    }
    else
    {
        console.AddLog("\tUptime : %d hours\t\n", UptimeH());
    }

    int regs[4] = { 0 };
    char vendor[13];
    __cpuid(regs, 0);              // mov eax,0; cpuid
    memcpy(vendor, &regs[1], 4);   // copy EBX
    memcpy(vendor + 4, &regs[3], 4); // copy EDX
    memcpy(vendor + 8, &regs[2], 4); // copy ECX
    vendor[12] = '\0';

    console.AddLog("\t--------------------- CPU --------------------\t\n");

    char CPUString[0x20];
    char CPUBrandString[0x40];
    int CPUInfo[4] = { -1 };
    int nSteppingID = 0;
    int nModel = 0;
    int nFamily = 0;
    int nProcessorType = 0;
    int nExtendedmodel = 0;
    int nExtendedfamily = 0;
    int nBrandIndex = 0;
    int nCLFLUSHcachelinesize = 0;
    int nLogicalProcessors = 0;
    int nAPICPhysicalID = 0;
    int nFeatureInfo = 0;
    int nCacheLineSize = 0;
    int nL2Associativity = 0;
    int nCacheSizeK = 0;
    int nPhysicalAddress = 0;
    int nVirtualAddress = 0;
    int nRet = 0;

    int nCores = 0;
    int nCacheType = 0;
    int nCacheLevel = 0;
    int nMaxThread = 0;
    int nSysLineSize = 0;
    int nPhysicalLinePartitions = 0;
    int nWaysAssociativity = 0;
    int nNumberSets = 0;

    unsigned    nIds, nExIds, i;

    bool    bSSE3Instructions = false;
    bool    bMONITOR_MWAIT = false;
    bool    bCPLQualifiedDebugStore = false;
    bool    bVirtualMachineExtensions = false;
    bool    bEnhancedIntelSpeedStepTechnology = false;
    bool    bThermalMonitor2 = false;
    bool    bSupplementalSSE3 = false;
    bool    bL1ContextID = false;
    bool    bCMPXCHG16B = false;
    bool    bxTPRUpdateControl = false;
    bool    bPerfDebugCapabilityMSR = false;
    bool    bSSE41Extensions = false;
    bool    bSSE42Extensions = false;
    bool    bPOPCNT = false;

    bool    bMultithreading = false;

    bool    bLAHF_SAHFAvailable = false;
    bool    bCmpLegacy = false;
    bool    bSVM = false;
    bool    bExtApicSpace = false;
    bool    bAltMovCr8 = false;
    bool    bLZCNT = false;
    bool    bSSE4A = false;
    bool    bMisalignedSSE = false;
    bool    bPREFETCH = false;
    bool    bSKINITandDEV = false;
    bool    bSYSCALL_SYSRETAvailable = false;
    bool    bExecuteDisableBitAvailable = false;
    bool    bMMXExtensions = false;
    bool    bFFXSR = false;
    bool    b1GBSupport = false;
    bool    bRDTSCP = false;
    bool    b64Available = false;
    bool    b3DNowExt = false;
    bool    b3DNow = false;
    bool    bNestedPaging = false;
    bool    bLBRVisualization = false;
    bool    bFP128 = false;
    bool    bMOVOptimization = false;

    bool    bSelfInit = false;
    bool    bFullyAssociative = false;


    // __cpuid with an InfoType argument of 0 returns the number of
    // valid Ids in CPUInfo[0] and the CPU identification string in
    // the other three array elements. The CPU identification string is
    // not in linear order. The code below arranges the information 
    // in a human readable form.
    __cpuid(CPUInfo, 0);
    nIds = CPUInfo[0];
    memset(CPUString, 0, sizeof(CPUString));
    *((int*)CPUString) = CPUInfo[1];
    *((int*)(CPUString + 4)) = CPUInfo[3];
    *((int*)(CPUString + 8)) = CPUInfo[2];

    // Get the information associated with each valid Id
    for (i = 0; i <= nIds; ++i)
    {
        __cpuid(CPUInfo, i);

        // Interpret CPU feature information.
        if (i == 1)
        {
            nSteppingID = CPUInfo[0] & 0xf;
            nModel = (CPUInfo[0] >> 4) & 0xf;
            nFamily = (CPUInfo[0] >> 8) & 0xf;
            nProcessorType = (CPUInfo[0] >> 12) & 0x3;
            nExtendedmodel = (CPUInfo[0] >> 16) & 0xf;
            nExtendedfamily = (CPUInfo[0] >> 20) & 0xff;
            nBrandIndex = CPUInfo[1] & 0xff;
            nCLFLUSHcachelinesize = ((CPUInfo[1] >> 8) & 0xff) * 8;
            nLogicalProcessors = ((CPUInfo[1] >> 16) & 0xff);
            nAPICPhysicalID = (CPUInfo[1] >> 24) & 0xff;
            bSSE3Instructions = (CPUInfo[2] & 0x1) || false;
            bMONITOR_MWAIT = (CPUInfo[2] & 0x8) || false;
            bCPLQualifiedDebugStore = (CPUInfo[2] & 0x10) || false;
            bVirtualMachineExtensions = (CPUInfo[2] & 0x20) || false;
            bEnhancedIntelSpeedStepTechnology = (CPUInfo[2] & 0x80) || false;
            bThermalMonitor2 = (CPUInfo[2] & 0x100) || false;
            bSupplementalSSE3 = (CPUInfo[2] & 0x200) || false;
            bL1ContextID = (CPUInfo[2] & 0x300) || false;
            bCMPXCHG16B = (CPUInfo[2] & 0x2000) || false;
            bxTPRUpdateControl = (CPUInfo[2] & 0x4000) || false;
            bPerfDebugCapabilityMSR = (CPUInfo[2] & 0x8000) || false;
            bSSE41Extensions = (CPUInfo[2] & 0x80000) || false;
            bSSE42Extensions = (CPUInfo[2] & 0x100000) || false;
            bPOPCNT = (CPUInfo[2] & 0x800000) || false;
            nFeatureInfo = CPUInfo[3];
            bMultithreading = (nFeatureInfo & (1 << 28)) || false;
        }
    }

    // Calling __cpuid with 0x80000000 as the InfoType argument
    // gets the number of valid extended IDs.
    __cpuid(CPUInfo, 0x80000000);
    nExIds = CPUInfo[0];
    memset(CPUBrandString, 0, sizeof(CPUBrandString));

    // Get the information associated with each extended ID.
    for (i = 0x80000000; i <= nExIds; ++i)
    {
        __cpuid(CPUInfo, i);

        if (i == 0x80000001)
        {
            bLAHF_SAHFAvailable = (CPUInfo[2] & 0x1) || false;
            bCmpLegacy = (CPUInfo[2] & 0x2) || false;
            bSVM = (CPUInfo[2] & 0x4) || false;
            bExtApicSpace = (CPUInfo[2] & 0x8) || false;
            bAltMovCr8 = (CPUInfo[2] & 0x10) || false;
            bLZCNT = (CPUInfo[2] & 0x20) || false;
            bSSE4A = (CPUInfo[2] & 0x40) || false;
            bMisalignedSSE = (CPUInfo[2] & 0x80) || false;
            bPREFETCH = (CPUInfo[2] & 0x100) || false;
            bSKINITandDEV = (CPUInfo[2] & 0x1000) || false;
            bSYSCALL_SYSRETAvailable = (CPUInfo[3] & 0x800) || false;
            bExecuteDisableBitAvailable = (CPUInfo[3] & 0x10000) || false;
            bMMXExtensions = (CPUInfo[3] & 0x40000) || false;
            bFFXSR = (CPUInfo[3] & 0x200000) || false;
            b1GBSupport = (CPUInfo[3] & 0x400000) || false;
            bRDTSCP = (CPUInfo[3] & 0x8000000) || false;
            b64Available = (CPUInfo[3] & 0x20000000) || false;
            b3DNowExt = (CPUInfo[3] & 0x40000000) || false;
            b3DNow = (CPUInfo[3] & 0x80000000) || false;
        }

        // Interpret CPU brand string and cache information.
        if (i == 0x80000002)
            memcpy(CPUBrandString, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000003)
            memcpy(CPUBrandString + 16, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000004)
            memcpy(CPUBrandString + 32, CPUInfo, sizeof(CPUInfo));
        else if (i == 0x80000006)
        {
            nCacheLineSize = CPUInfo[2] & 0xff;
            nL2Associativity = (CPUInfo[2] >> 12) & 0xf;
            nCacheSizeK = (CPUInfo[2] >> 16) & 0xffff;
        }
        else if (i == 0x80000008)
        {
            nPhysicalAddress = CPUInfo[0] & 0xff;
            nVirtualAddress = (CPUInfo[0] >> 8) & 0xff;
        }
        else if (i == 0x8000000A)
        {
            bNestedPaging = (CPUInfo[3] & 0x1) || false;
            bLBRVisualization = (CPUInfo[3] & 0x2) || false;
        }
        else if (i == 0x8000001A)
        {
            bFP128 = (CPUInfo[0] & 0x1) || false;
            bMOVOptimization = (CPUInfo[0] & 0x2) || false;
        }
    }

    // Display all the information in user-friendly format.

    console.AddLog("\tCPU String: %s\t\n", CPUString);

    if (nIds >= 1)
    {
        if (nSteppingID)
            console.AddLog("\tStepping ID = %d\t\n", nSteppingID);
        if (nModel)
            console.AddLog("\tModel = %d\t\n", nModel);
        if (nFamily)
            console.AddLog("\tFamily = %d\t\n", nFamily);
        if (nProcessorType)
            console.AddLog("\tProcessor Type = %d\t\n", nProcessorType);
        if (nExtendedmodel)
            console.AddLog("\tExtended model = %d\t\n", nExtendedmodel);
        if (nExtendedfamily)
            console.AddLog("\tExtended family = %d\t\n", nExtendedfamily);
        if (nBrandIndex)
            console.AddLog("\tBrand Index = %d\t\n", nBrandIndex);
        if (nCLFLUSHcachelinesize)
            console.AddLog("\tCLFLUSH cache line size = %d\t\n",
                nCLFLUSHcachelinesize);
        if (bMultithreading && (nLogicalProcessors > 0))
            console.AddLog("\tLogical Processor Count = %d\t\n", nLogicalProcessors);
        if (nAPICPhysicalID)
            console.AddLog("\tAPIC Physical ID = %d\t\n", nAPICPhysicalID);
    }

    console.AddLog("\n");

    if (nExIds >= 0x80000004)
        console.AddLog("\tCPU Brand String: %s\t\n", CPUBrandString);

    if (nExIds >= 0x80000006)
    {
        console.AddLog("\tCache Line Size = %d\t\n", nCacheLineSize);
        console.AddLog("\tL2 Associativity = %d\t\n", nL2Associativity);
        console.AddLog("\tCache Size = %dK\t\n", nCacheSizeK);
    }


    for (i = 0;; i++)
    {
        __cpuidex(CPUInfo, 0x4, i);
        if (!(CPUInfo[0] & 0xf0)) break;

        if (i == 0)
        {
            nCores = CPUInfo[0] >> 26;
            console.AddLog("\tNumber of Cores = %d\t\n", nCores + 1);
        }

        nCacheType = (CPUInfo[0] & 0x1f);
        nCacheLevel = (CPUInfo[0] & 0xe0) >> 5;
        bSelfInit = (CPUInfo[0] & 0x100) >> 8;
        bFullyAssociative = (CPUInfo[0] & 0x200) >> 9;
        nMaxThread = (CPUInfo[0] & 0x03ffc000) >> 14;
        nSysLineSize = (CPUInfo[1] & 0x0fff);
        nPhysicalLinePartitions = (CPUInfo[1] & 0x03ff000) >> 12;
        nWaysAssociativity = (CPUInfo[1]) >> 22;
        nNumberSets = CPUInfo[2];

        console.AddLog("\n");

        console.AddLog("\tECX Index %d\t\n", i);
        switch (nCacheType)
        {
        case 0:
            console.AddLog("\t   Type: Null\t\n");
            break;
        case 1:
            console.AddLog("\t   Type: Data Cache\t\n");
            break;
        case 2:
            console.AddLog("\t   Type: Instruction Cache\t\n");
            break;
        case 3:
            console.AddLog("\t   Type: Unified Cache\t\n");
            break;
        default:
            console.AddLog("\t   Type: Unknown\t\n");
        }

        console.AddLog("\t   Level = %d\t\n", nCacheLevel + 1);
        if (bSelfInit)
        {
            console.AddLog("\t   Self Initializing\t\n");
        }
        else
        {
            console.AddLog("\t   Not Self Initializing\t\n");
        }
        if (bFullyAssociative)
        {
            console.AddLog("\t   Is Fully Associatve\t\n");
        }
        else
        {
            console.AddLog("\t   Is Not Fully Associatve\t\n");
        }
        console.AddLog("\t   Max Threads = %d\t\n",
            nMaxThread + 1);
        console.AddLog("\t   System Line Size = %d\t\n",
            nSysLineSize + 1);
        console.AddLog("\t   Physical Line Partions = %d\t\n",
            nPhysicalLinePartitions + 1);
        console.AddLog("\t   Ways of Associativity = %d\t\n",
            nWaysAssociativity + 1);
        console.AddLog("\t   Number of Sets = %d\t\n",
            nNumberSets + 1);
    }

    console.AddLog("\t--------------------- CPU --------------------\t\n");

    console.AddLog("\tMemory: %f GB\t\n", (float)statex.ullTotalPhys / (1024 * 1024 * 1024));
    console.AddLog("\n");

    return 0;
}

int openfile(std::vector<std::string>& vect)
{
    fstream my_file;
    string file = vect[1];

    my_file.open(file, ios::in);
    if (!my_file)
    {
        console.AddLog("No such file %s!\n", file.c_str());
        return 1;
    }

    else
    {
        string str;
        while (getline(my_file, str))
        {
            console.AddLog("%s\n", str.c_str());
        }
    }
    console.AddLog("\n");
    my_file.close();

    return 0;
}

int ttime(std::vector<std::string>& vect)
{
    auto givemetime = chrono::system_clock::to_time_t(chrono::system_clock::now());
    console.AddLog("%s", ctime(&givemetime));
    return 0;
}

int rm(std::vector<std::string>& vect)
{
    return remove(vect[1].c_str());
}

int whoami(std::vector<std::string>& vect)
{
    TCHAR username[UNLEN + 1];
    DWORD username_len = UNLEN + 1;
    GetUserName((TCHAR*)username, &username_len);;

    char new_username[UNLEN];

    wcstombs(new_username, username, wcslen(username) + 1);

    console.AddLog("%s\n", new_username);

    return 0;
}

int writefile(std::vector<std::string>& vect)
{
    auto mode = ios::in;
    string file = vect[1];
    string content = vect[2];

    if (console.CheckFile(file.c_str()) != 0)
    {
        fstream new_file(file, mode);
    }

    fstream myfile(file, mode);
    myfile << content;
    myfile.close();

    return 0;
}

int yes(std::vector<std::string>& vect)
{
    /*while (true)
    {
        console.AddLog("yes\n");
    }*/

    for (int i = 0; i < 100000; i++)
    {
        console.AddLog("y\n");
    }

    return 0;
}

int calc(std::vector<std::string>& vect)
{
    string op = vect[1];
    float num1 = stof(vect[2]);
    float num2 = stof(vect[3]);

    try
    {
        if (!strcmp(op.c_str(), "+"))
        {
            console.AddLog("Result: %f\n", num1 + num2);
            return 0;
        }

        else if (!strcmp(op.c_str(), "-"))
        {
            console.AddLog("Result: %f\n", num1 - num2);
            return 0;
        }

        else if (!strcmp(op.c_str(), "*"))
        {
            console.AddLog("Result: %f\n", num1 * num2);
            return 0;
        }

        if (!strcmp(op.c_str(), "/"))
        {
            if (num2 == 0)
            {
                console.AddLog("Cannot divide with 0!\n");
                return 1;
            }

            else
            {
                console.AddLog("Result: %f\n", num1 / num2);
                return 0;
            }
        }

        else
        {
            console.AddLog("Invalid operator %s!\n", op.c_str());
            return 1;
        }

        return 0;
    }

    catch (const std::exception& e)
    {
        console.AddLog("Catched exception. Exception result: %s", e.what());
        return 1;
    }
}

/* Credits to StjepanBM1 */
int geocalc(std::vector<std::string>& vect)
{
    string EXT = "EXT";
    string SUR = "SUR";
    string TRA = "TRA";
    string REC = "REC";
    string SQU = "SQU";

    string ext_or_sur = vect[1];

    try
    {
        if (ext_or_sur == SUR)
        {
            string TRA = "TRA";
            string REC = "REC";
            string SQU = "SQU";

            string rec_or_squ = vect[2];

            if (rec_or_squ == REC)
            {
                double x = stod(vect[3]);
                double y = stod(vect[4]);

                console.AddLog("Result: %f\n", povrsDvijustr(x, y));
            }

            else if (rec_or_squ == SQU)
            {
                double x = stod(vect[3]);

                console.AddLog("Result: %f\n", povrsKvdjustr(x));
            }

            else if (rec_or_squ == TRA)
            {
                double x = stod(vect[3]);
                double Vx = stod(vect[4]);

                console.AddLog("Result: %f\n", povrsTrokuta(x, Vx));
            }

            else
            {
                console.AddLog("Unknown input '%s'\n", rec_or_squ.c_str());
                return 1;
            }
        }

        else if (ext_or_sur == EXT)
        {
            string TRA = "TRA";
            string REC = "REC";
            string SQU = "SQU";

            string tra_or_rec_or_squ = vect[1];

            if (tra_or_rec_or_squ == TRA)
            {
                double x = stod(vect[3]);
                double y = stod(vect[4]);
                double z = stod(vect[5]);

                console.AddLog("Result: %f\n", opsgTrijustr(x, y, z));
            }

            else if (tra_or_rec_or_squ == REC)
            {
                double x = stod(vect[3]);
                double y = stod(vect[4]);

                console.AddLog("Result: %f\n", opsgDvijustr(x, y));
            }

            else if (tra_or_rec_or_squ == SQU)
            {
                int sqe = 4;

                double x = stod(vect[3]);

                console.AddLog("Result: %f\n", opsgKvdjustr(x, sqe));
            }

            else
            {
                console.AddLog("Unknown input '%s'\n", tra_or_rec_or_squ.c_str());
                return 1;
            }
        }

        else
        {
            console.AddLog("Unknown input '%s'\n", ext_or_sur.c_str());
            return 1;
        }

        return 0;
    }

    catch (const std::exception& e)
    {
        console.AddLog("Catched exception! Result: '%s'\n", e.what());
        return 1;
    }
}