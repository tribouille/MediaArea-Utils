/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include <ZenLib/File.h>
#include <ZenLib/Ztring.h>
#include <ZenLib/ZtringList.h>
#include <ZenLib/ZtringListList.h>
#include "input_csv_video.h"
#include <iostream>

InputCSVVideo::InputCSVVideo()
{
}

InputCSVVideo::~InputCSVVideo()
{
    std::map<ZenLib::Ztring, std::vector<InputCSVVideo_Field*> >::iterator it = fields.begin();
    for (; it != fields.end(); ++it)
    {
        for (size_t i = 0; i < it->second.size(); ++i)
            delete it->second[i];
        it->second.clear();
    }
    fields.clear();
}

int InputCSVVideo::usage()
{
    std::cerr << "usage: tool --csv-stats-dir=path --out=output_filename " << std::endl;
    std::cerr << "--in-general=csv_general --in-video=csv_video --in-audio=csv_audio --in-text=csv_text --in-other=csv_other --in-image=csv_image --in-menu=csv_menu" << std::endl;
    return -1;
}

int InputCSVVideo::parse(int argc, char* argv[])
{
    if (argc == 1)
        return usage();

    std::vector<ZenLib::Ztring> args;
    for (int i = 0; i < argc; ++i)
        args.push_back(ZenLib::Ztring().From_UTF8(argv[i]));

    for (size_t i = 1; i < args.size(); ++i)
    {
#define COMPARE(arg, s) \
        arg.compare(0, s.length(), s)

#define PARAMETER(arg, val, callback)                                \
        else if (COMPARE(arg, ZenLib::Ztring(__T(val))) == 0)  \
        {                                                       \
            if (callback(arg) < 0)                          \
                return -1;                                      \
        }                                                       \

        if (0);
        PARAMETER(args[i], "--csv-stats-dir=", parse_in_csv_dir)
        PARAMETER(args[i], "--in-general=",    parse_in_General)
        PARAMETER(args[i], "--in-video=",      parse_in_Video)
        PARAMETER(args[i], "--in-audio=",      parse_in_Audio)
        PARAMETER(args[i], "--in-text=",       parse_in_Text)
        PARAMETER(args[i], "--in-other=",      parse_in_Other)
        PARAMETER(args[i], "--in-image=",      parse_in_Image)
        PARAMETER(args[i], "--in-menu=",       parse_in_Menu)
        PARAMETER(args[i], "--out=",           parse_out_file)
        else
        {
            std::wcerr << __T("Unknown argument: ") << args[i] << std::endl;
            return usage();
        }
#undef COMPARE
    }

    return 0;
}

int InputCSVVideo::parse_csv_fields(const ZenLib::Ztring& type, const ZenLib::Ztring& filename)
{
    ZenLib::File file(filename);
    ZenLib::int64u size = file.Size_Get();
    ZenLib::int8u *buffer = new ZenLib::int8u[size];
    size_t size_read = file.Read(buffer, size);

    if (size != size_read)
    {
        delete buffer;
        std::cerr << "cannot read properly the file" << std::endl;
        return -1;
    }

    ZenLib::Ztring content = ZenLib::Ztring((const char*)buffer, size_read);
    delete buffer;

    ZenLib::ZtringListList list;
    list.clear();
    list.Separator_Set(0, __T("\n"));
    list.Separator_Set(1, __T(";"));
    list.Write(content);
    if (!list.size())
        return -1;

    for (size_t i = 0; i < list.size(); ++i)
    {
        if (list[i].size() < 3)
        {
            std::cerr << list[i][0].To_UTF8() << ": " << list[i].size()<< std::endl;
            std::cerr << "file: " << filename.To_UTF8() << "is corrupted" << std::endl;
            return -1;
        }

        InputCSVVideo_Field *field = new InputCSVVideo_Field;
        field->field  = list[i][0];
        field->filter = list[i][1];
        field->method = list[i][2];
        if (list[i].size() == 4)
            field->list   = list[i][3];
        fields[type].push_back(field);
    }

    return 0;
}

#define PARSE_TYPE(name) \
    int InputCSVVideo::parse_in_##name(const ZenLib::Ztring& arg)       \
    {                                                                   \
        size_t pos = 0;                                                 \
                                                                        \
        pos = arg.find(__T("="));                                       \
        if (parse_csv_fields(__T(#name), arg.substr(pos + 1)) < 0)      \
            return -1;                                                  \
        return 0;                                                       \
    }

PARSE_TYPE(General)
PARSE_TYPE(Video)
PARSE_TYPE(Audio)
PARSE_TYPE(Text)
PARSE_TYPE(Other)
PARSE_TYPE(Image)
PARSE_TYPE(Menu)

#undef PARSE_TYPE

int InputCSVVideo::parse_in_csv_dir(const ZenLib::Ztring& arg)
{
    size_t pos = 0;

    pos = arg.find(__T("="));
    ZenLib::Ztring in_directory = arg.substr(pos + 1);
    ZenLib::Ztring separator(__T("/"));

#ifdef _WIN32
    separator = ZenLib::Ztring(__T("\\"));
#endif // _WIN32

    in_directory += separator;
    stats_filenames[__T("General")] = in_directory + __T("0.csv");
    stats_filenames[__T("Video")] = in_directory + __T("1.csv");
    stats_filenames[__T("Audio")] = in_directory + __T("2.csv");
    stats_filenames[__T("Text")] = in_directory + __T("3.csv");
    stats_filenames[__T("Other")] = in_directory + __T("4.csv");
    stats_filenames[__T("Image")] = in_directory + __T("5.csv");
    stats_filenames[__T("Menu")] = in_directory + __T("6.csv");

    return 0;
}

int InputCSVVideo::parse_out_file(const ZenLib::Ztring& arg)
{
    size_t pos = 0;

    pos = arg.find(__T("="));
    out_filename = arg.substr(pos + 1);
    return 0;
}
