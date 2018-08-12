#ifndef PFITEM_H
#define PFITEM_H

namespace PF
{
    struct PackedFileItem
    {
        typedef unsigned int DWORD;

        std::string Name;
        DWORD Next;
        DWORD FileFolder;
        DWORD Offset;
        DWORD Length;

        //Folder
        PackedFileItem(const std::string& N,DWORD Num,DWORD FF)
        {
            Name=N;

            if(Name == ".")
            {
                Next = FF+1;
                FileFolder = FF;
            }
            else
            {
                Next = Num;
                FileFolder = FF;
            }
        }

        //File
        PackedFileItem(const std::string& N,DWORD Num,DWORD O,DWORD L)
        {
            Name = N;
            Next = Num;
            FileFolder = static_cast<DWORD>(-1);
            Offset = O;
            Length = L;
        }
    };

}//PF namespace

#endif