/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include <ZenLib/ZtringList.h>
#include "input_csv_video.h"
#include <iostream>

InputCSVVideo::InputCSVVideo()
{
}

InputCSVVideo::~InputCSVVideo()
{
}

int InputCSVVideo::usage()
{
    std::cerr << "usage: tool --in-video=csv_filename --out=output_filename --fields=fields_separated_by_comma" << std::endl;
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
        PARAMETER(args[i], "--csv-dir=",    parse_in_csv_dir)
        PARAMETER(args[i], "--in-general=", parse_in_General)
        PARAMETER(args[i], "--in-video=",   parse_in_Video)
        PARAMETER(args[i], "--in-audio=",   parse_in_Audio)
        PARAMETER(args[i], "--in-text=",    parse_in_Text)
        PARAMETER(args[i], "--in-other=",   parse_in_Other)
        PARAMETER(args[i], "--in-image=",   parse_in_Image)
        PARAMETER(args[i], "--in-menu=",    parse_in_Menu)
        PARAMETER(args[i], "--out=",        parse_out_file)
        PARAMETER(args[i], "--fields=",     parse_fields)
        else
        {
            std::wcerr << __T("Unknown argument: ") << args[i] << std::endl;
            return usage();
        }
#undef COMPARE
    }

    return 0;
}

#define PARSE_TYPE(name) \
    int InputCSVVideo::parse_in_##name(const ZenLib::Ztring& arg)      \
    {                                                                  \
        size_t pos = 0;                                                \
                                                                       \
        pos = arg.find(__T("="));                                      \
        in_filenames[#name].push_back(arg.substr(pos + 1));            \
        return 0;                                                      \
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
    in_filenames[__T("General")].push_back(in_directory + __T("0.csv"));
    in_filenames[__T("Video")].push_back(in_directory + __T("1.csv"));
    in_filenames[__T("Audio")].push_back(in_directory + __T("2.csv"));
    in_filenames[__T("Text")].push_back(in_directory + __T("3.csv"));
    in_filenames[__T("Other")].push_back(in_directory + __T("4.csv"));
    in_filenames[__T("Image")].push_back(in_directory + __T("5.csv"));
    in_filenames[__T("Menu")].push_back(in_directory + __T("6.csv"));

    return 0;
}

int InputCSVVideo::parse_out_file(const ZenLib::Ztring& arg)
{
    size_t pos = 0;

    pos = arg.find(__T("="));
    out_filename = arg.substr(pos + 1);
    return 0;
}

int InputCSVVideo::parse_fields(const ZenLib::Ztring& arg)
{
    size_t pos = 0;

    pos = arg.find(__T("="));
    ZenLib::Ztring fields_str = arg.substr(pos + 1);

    ZenLib::ZtringList list;
    list.Separator_Set(0, __T(","));
    list.Write(fields_str);
    if (!list.size())
        return -1;

    for (size_t i = 0; i < list.size(); ++i)
        fields.push_back(list[i]);

    return 0;
}
