/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "input_csv_video.h"
#include "output_csv_video.h"
#include "content_csv_video.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/File.h"
#include <iostream>
#include <map>

//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    InputCSVVideo  input;
    OutputCSVVideo output;

    if (input.parse(argc, argv) < 0)
        return 1;

    // std::map<ZenLib::Ztring, std::vector<InputCSVVideo_Field*> >::const_iterator it = fields.begin();
    // for (; it != fields.end(); ++it)
    // {
    //     std::wcout << it->first << __T(": ")<< std::endl;
    //     for (size_t i = 0; i < it->second.size(); ++i)
    //     {
    //         std::wcout << __T("\t") << it->second[i]->field <<  __T("\t")  << it->second[i]->filter <<  __T("\t")  << it->second[i]->method <<  __T("\t")  << it->second[i]->list;
    //         std::wcout << std::endl;
    //     }
    // }

    ZenLib::Ztring existing_types[] =
    {
        __T("General"),
        __T("Video"),
        __T("Audio"),
        __T("Text"),
        __T("Other"),
        __T("Image"),
        __T("Menu")
    };

    const std::map<ZenLib::Ztring, std::vector<InputCSVVideo_Field*> >& types = input.get_fields_wanted();
    for (size_t i = 0; i < (sizeof(existing_types) / sizeof(*existing_types)); ++i)
    {
        std::map<ZenLib::Ztring, std::vector<InputCSVVideo_Field*> >::const_iterator current_type = types.find(existing_types[i]);
        if (current_type == types.end())
            continue;
        output.set_current_type(existing_types[i]);
        ContentCSVVideo content(output);

        if (!current_type->second.size())
            continue;

        content.set_fields_wanted(current_type->second);

        const ZenLib::Ztring& stats_filename = input.get_stats_filename(existing_types[i]);
        if (stats_filename.length() && content.parse_stats(stats_filename) < 0)
            return 1;
        if (content.parse_list() < 0)
            return 1;
        if (content.parse_source() < 0)
            return 1;
    }

    if (output.open_file(input.get_output()) < 0)
    {
        std::wcerr << __T("Cannot open the output file ") << input.get_output() << std::endl;
        return 1;
    }

    if (output.write() < 0)
    {
        std::cerr << "An error occured when writing list" << std::endl;
        return 1;
    }

    return 0;
}
