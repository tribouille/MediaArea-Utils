/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "output_csv_video.h"
#include <iostream>
#include <stdlib.h>
#include "ZenLib/ZtringList.h"

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

bool OutputCSVVideo::next_is_bigger(const ZenLib::Ztring& val, std::vector<ZenLib::Ztring>& res, std::vector<ZenLib::Ztring>::iterator& it)
{
    ZenLib::ZtringList tmp_1;
    tmp_1.Separator_Set(0, __T(" / "));
    tmp_1.Write(val);
    ZenLib::ZtringList tmp_2;
    tmp_2.Separator_Set(0, __T(" / "));
    tmp_2.Write(*it);

    for (size_t i = 0; i < tmp_1.size(); ++i)
    {
        if (i == tmp_2.size())
            return false;

        std::string utf8_1 = tmp_1[i].To_UTF8();
        std::string utf8_2 = tmp_2[i].To_UTF8();
        char* end_1 = NULL;
        char* end_2 = NULL;
        double d_1 = strtod(utf8_1.c_str(), &end_1);
        double d_2 = strtod(utf8_2.c_str(), &end_2);
        if (d_2 > d_1)
            return true;
    }
    return false;
}

void OutputCSVVideo::fill_values_ordered(const std::map<ZenLib::Ztring, ZenLib::Ztring>& values, std::vector<ZenLib::Ztring>& res)
{
    bool are_numerical = true;
    std::map<ZenLib::Ztring, ZenLib::Ztring>::const_iterator it = values.begin();
    for (; it != values.end(); ++it)
    {
        const ZenLib::Ztring& str = it->first;
        for (size_t i = 0; i < str.size(); ++i)
        {
            if ((str[i] < '0' || str[i] > '9') && str[i] != '.')
            {
                if (i + 3 < str.size() && str[i] == ' ' && str[i + 1] == '/' && str[i + 2] == ' ')
                    i += 2;
                else
                {
                    are_numerical = false;
                    break;
                }
            }
        }
    }

    it = values.begin();
    if (!are_numerical)
    {
        for (; it != values.end(); ++it)
            res.push_back(it->first);
        return;
    }
    for (; it != values.end(); ++it)
    {
        std::vector<ZenLib::Ztring>::iterator vec_it = res.begin();

        for (; vec_it != res.end(); ++vec_it)
        {
            if (next_is_bigger(it->first, res, vec_it))
            {
                res.insert(vec_it, it->first);
                break;
            }
        }

        if (vec_it == res.end())
            res.push_back(it->first);
    }
}

int OutputCSVVideo::write_fields_function(const std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, ZenLib::Ztring> >& values)
{
    std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, ZenLib::Ztring> >::const_iterator it = values.begin();

    for (; it != values.end(); ++it)
    {
        file->Write(__T("        {\n"));
        file->Write(__T("            std::vector<std::string> tmp_vec;\n"));
        std::vector<ZenLib::Ztring> res;
        fill_values_ordered(it->second, res);
        for (size_t i = 0; i < res.size(); ++i)
        {
            ZenLib::Ztring tmp = __T("            tmp_vec.push_back(\"") + res[i] + __T("\");\n");
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
    std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, ZenLib::Ztring> > >::const_iterator it = values.begin();

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

int OutputCSVVideo::register_values(const ZenLib::Ztring& field, const std::map<ZenLib::Ztring, ZenLib::Ztring>& v)
{
    std::map<ZenLib::Ztring, ZenLib::Ztring>::const_iterator it = v.begin();
    for (; it != v.end(); ++it)
        values[current_type][field][it->first] = it->second;
}
