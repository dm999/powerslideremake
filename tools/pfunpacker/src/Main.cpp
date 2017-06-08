
#include <cstdio>
#include <string>
#include <vector>

#if defined(__WIN32__) || defined(__WIN32) || defined (_MSC_VER)
    #include <direct.h>
#else
    #include <sys/types.h>
    #include <sys/stat.h>
#endif

#include "CmdLine.h"

#include "tools/Conversions.h"
#include "loaders/PFItem.h"

namespace {
    std::string fileParam;
    std::string pathParam;

    std::vector<PF::PackedFileItem> fileSystem;
}

typedef unsigned int DWORD;
typedef unsigned char BYTE;

bool parseCommandLine(int argc, char **argv)
{
    bool res = false;
    try {

        TCLAP::CmdLine cmd("", ' ', "1.0", false);

        TCLAP::ValueArg<std::string> fileArg("", "file", "PF file to unpack", true, "data.pf", "string");
        cmd.add(fileArg);

        TCLAP::ValueArg<std::string> pathArg("", "dst", "Destination to unpack", true, ".", "string");
        cmd.add(pathArg);

        cmd.parse(argc, argv);

        fileParam = fileArg.getValue();
        pathParam = pathArg.getValue();

        res = true;
    }
    catch (TCLAP::ArgException& e)
    {
        std::cerr << "Exception: " << e.error() << " for arg " << e.argId() << std::endl;
    }
    catch (std::exception& e)
    {
        std::cerr << "Exception: " << e.what() << std::endl;
    }

    return res;
}

std::string readString(FILE * f, DWORD& FilePos)
{
    std::string ret = "";
    char ch = '!';
    char buffer[1024];
    DWORD charpos = 0;
    while (ch)
    {
        fread(&ch, 1, 1, f);
        buffer[charpos] = ch;
        charpos++;
        FilePos++;
    }
    ret = buffer;
    return ret;
}

void CreateDir(const std::string& Dir)
{
#if defined(__WIN32__) || defined(__WIN32) || defined (_MSC_VER)
    _mkdir(Dir.c_str());
#else
     mkdir(Dir.c_str(), S_IRWXU);
#endif
}

void ExtractPF(FILE *f, const std::string& Dir, DWORD index)
{
    if (index < fileSystem.size())
    {
        const PF::PackedFileItem& Tmp = fileSystem[index];

        if (Tmp.FileFolder != -1)                         //Folder Pass
        {
            if (Tmp.Name == ".")
            {
                if (Tmp.Next != -1 || Tmp.FileFolder != -1)
                    ExtractPF(f, Dir + "/", Tmp.Next);
            }
            else
            {
                if (Tmp.Next == -1)
                {
                    CreateDir(Dir + Tmp.Name);
                    ExtractPF(f, Dir + Tmp.Name, Tmp.FileFolder);
                }
                else
                {
                    CreateDir(Dir + Tmp.Name);
                    ExtractPF(f, Dir + Tmp.Name, Tmp.FileFolder);
                    ExtractPF(f, Dir, Tmp.Next);
                }
            }
        }
        else                                            //File Pass
        {
            FILE *ff;
            std::string FileName = Dir + "/";
            FileName += Tmp.Name;
            ff = fopen(FileName.c_str(), "wb");
            if (ff)
            {
                printf("File unpacking: [%.2fMB] %s\n", (float)Tmp.Length / (1024.0f * 1024.0f), Tmp.Name.c_str());
                std::vector<BYTE> buffer(Tmp.Length);
                fseek(f, Tmp.Offset, SEEK_SET);
                fread(&buffer[0], Tmp.Length, 1, f);
                fwrite(&buffer[0], Tmp.Length, 1, ff);
            }
            else
                printf("File unpacking: Unable to create file!!!\n");
            fclose(ff);


            if (Tmp.Next != -1 || Tmp.FileFolder != -1)
                ExtractPF(f, Dir, Tmp.Next);
        }
    }
}

int main(int argc, char* argv[])
{
    if (parseCommandLine(argc, argv))
    {
        FILE * f = fopen(fileParam.c_str(), "rb");
        if (f)
        {
            //read file system
            {
                DWORD Size, From;
                DWORD ElemCount;

                fseek(f, 0, SEEK_END);
                Size = ftell(f);
                fseek(f, -4, SEEK_CUR);
                fread(&From, 4, 1, f);
                fseek(f, From, 0);
                fread(&ElemCount, 4, 1, f);

                DWORD FilePos = From + 4;



                while (1)
                {
                    if (FilePos >= Size - 4) break;

                    DWORD Next, F_F, Offset, Length, Smthing;

                    std::string itemName = readString(f, FilePos);

                    fread(&Next, 4, 1, f);
                    fread(&F_F, 4, 1, f);
                    FilePos += 8;
                    if (F_F == 0xffffffff)//file
                    {
                        FilePos += 10;
                        fread(&Offset, 4, 1, f);
                        fread(&Length, 4, 1, f);
                        fread(&Smthing, 2, 1, f);
                        fileSystem.push_back(PF::PackedFileItem(itemName, Next, Offset, Length));
                    }
                    else                // folder
                    {
                        fileSystem.push_back(PF::PackedFileItem(itemName, Next, F_F));
                    }
                }

            }

            printf("File system size = %u\n", fileSystem.size());

            CreateDir(pathParam);
            ExtractPF(f, pathParam, 0);

            fclose(f);
        }
        else
            printf("Unable to open file [%s]\n", fileParam.c_str());

    }
    return 0;
}