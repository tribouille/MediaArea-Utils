/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "output_csv_video.h"
#include <iostream>

OutputCSVVideo::OutputCSVVideo() : file(NULL)
{
}

OutputCSVVideo::~OutputCSVVideo()
{
    if (file)
    {
        file->Close();
        delete file;
    }
}

int OutputCSVVideo::write_header()
{
    file->Write(__T("/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.\n"));
    file->Write(__T(" *\n"));
    file->Write(__T(" *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can\n"));
    file->Write(__T(" *  be found in the License.html file in the root of the source tree.\n"));
    file->Write(__T(" */\n"));
    file->Write(__T("\n"));
    file->Write(__T("#include <map>\n"));
    file->Write(__T("#include <vector>\n"));
    file->Write(__T("#include <string>\n"));
    file->Write(__T("\n"));
    file->Write(__T("/* This file is generated, do not modify it without the script. */"));
    file->Write(__T("\n"));
    return 0;
}


int OutputCSVVideo::write_function_top()
{
    file->Write(__T("int get_generated_values_from_csv(std::map<std::string, std::map<std::string, std::vector<std::string> > >& values)\n"));
    file->Write(__T("{\n"));
    return 0;
}

int OutputCSVVideo::write_fields_function(const std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, bool> >& values)
{
    std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, bool> >::const_iterator it = values.begin();

    for (; it != values.end(); ++it)
    {
        file->Write(__T("        {\n"));
        file->Write(__T("            std::vector<std::string> tmp_vec;\n"));
        std::map<ZenLib::Ztring, bool>::const_iterator sub_it = it->second.begin();
        for (; sub_it != it->second.end(); ++sub_it)
        {
            ZenLib::Ztring tmp = __T("            tmp_vec.push_back(\"") + sub_it->first + __T("\");\n");
            file->Write(tmp);
        }
        ZenLib::Ztring value = __T("            tmp[\"") + it->first + __T("\"] = tmp_vec;\n");
        file->Write(value);
        file->Write(__T("        }\n"));
    }
    return 0;
}

int OutputCSVVideo::write_type_function()
{
    std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, bool> > >::const_iterator it = values.begin();

    for (; it != values.end(); ++it)
    {
        file->Write(__T("    {\n"));
        file->Write(__T("        std::map<std::string, std::vector<std::string> > tmp;\n"));
        write_fields_function(it->second);
        ZenLib::Ztring value = __T("        values[\"") + it->first + __T("\"] = tmp;\n");
        file->Write(value);
        file->Write(__T("    }\n"));
    }
    return 0;
}

int OutputCSVVideo::write_function_end()
{
    file->Write(__T("    return 0;\n"));
    file->Write(__T("}\n"));
    return 0;
}

int OutputCSVVideo::write()
{
    if (write_header() < 0)
        return -1;

    if (write_function_top() < 0)
        return -1;

    if (write_type_function() < 0)
        return -1;

    if (write_function_end() < 0)
        return -1;
    return 0;
}

int OutputCSVVideo::open_file(const ZenLib::Ztring& filename)
{
    file = new ZenLib::File(filename, ZenLib::File::Access_Write);
    file->Create(filename);
    return 0;
}

int OutputCSVVideo::register_values(const ZenLib::Ztring& type, const ZenLib::Ztring& field, const std::map<ZenLib::Ztring, bool>& v)
{
    std::map<ZenLib::Ztring, bool>::const_iterator it = v.begin();
    for (; it != v.end(); ++it)
        values[type][field][it->first] = it->second;
}
