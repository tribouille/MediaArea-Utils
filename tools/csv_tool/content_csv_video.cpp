/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "input_csv_video.h"
#include "output_csv_video.h"
#include "content_csv_video.h"
#include <iostream>

ContentCSVVideo::ContentCSVVideo(OutputCSVVideo& out) : output(out)
{
}

ContentCSVVideo::~ContentCSVVideo()
{
}

int ContentCSVVideo::get_content(const ZenLib::Ztring& filename)
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

    content = ZenLib::Ztring((const char*)buffer, size_read);
    delete buffer;
    return 0;
}

int ContentCSVVideo::get_lines_columns(size_t& lines, size_t& columns)
{
    lines = list.size();
    columns = 0;
    for (size_t i = 0; i < lines; ++i)
    {
        // CSV: nb columns is equal to each line
        if (columns < list[i].size())
            columns = list[i].size();
    }
    return 0;
}

bool ContentCSVVideo::field_is_wanted(const ZenLib::Ztring& method, const ZenLib::Ztring& field, ZenLib::Ztring& filter)
{
    // if fields wanted is empty
    if (!fields_wanted.size())
        return false;

    for (size_t i = 0; i < fields_wanted.size(); ++i)
        if (fields_wanted[i]->method == method && fields_wanted[i]->field == field)
        {
            filter = fields_wanted[i]->filter;
            return true;
        }
    return false;
}

int ContentCSVVideo::create_map_for_doubles()
{
    size_t lines, columns;
    if (get_lines_columns(lines, columns))
        return -1;

    for (size_t i = 0; i < columns; ++i)
    {
        ZenLib::Ztring filter;
        if (i >= list[0].size() || !list[0][i].length() || !field_is_wanted(__T("FromStats"), list[0][i], filter))
            continue;

        std::map<ZenLib::Ztring, ZenLib::Ztring> internal_values;
        // j == 0 is the field name
        for (size_t j = 1; j < lines; ++j)
        {
            if (j >= list.size() || i >= list[j].size() || !list[j][i].length())
                continue;
            internal_values[list[j][i]] = filter;
        }
        output.register_values(list[0][i], internal_values);
    }
    return 0;
}

int ContentCSVVideo::get_list()
{
    list.clear();
    list.Separator_Set(0, __T("\n"));
    list.Separator_Set(1, __T(";"));
    list.Write(content);
}

int ContentCSVVideo::parse_stats(const ZenLib::Ztring& filename)
{
    if (!fields_wanted.size())
        return 0;

    if (get_content(filename) < 0)
        return 1;

    if (get_list() < 0)
        return 1;

    if (create_map_for_doubles() < 0)
        return 1;
    return 0;
}

int ContentCSVVideo::parse_list()
{
    if (!fields_wanted.size())
        return 0;

    for (size_t i = 0; i < fields_wanted.size(); ++i)
    {
        if (fields_wanted[i]->method == __T("FromList") && fields_wanted[i]->list.length())
        {
            ZenLib::ZtringList tmp;
            tmp.Separator_Set(0, __T(" / "));
            tmp.Write(fields_wanted[i]->list);
            std::map<ZenLib::Ztring, ZenLib::Ztring> internal_values;
            for (size_t j = 0; j < tmp.size(); ++j)
                internal_values[tmp[j]] = fields_wanted[i]->filter;
            output.register_values(fields_wanted[i]->field, internal_values);
        }
    }
    return 0;
}

int ContentCSVVideo::parse_source()
{
    if (!fields_wanted.size())
        return 0;

    return 0;
}

void ContentCSVVideo::set_fields_wanted(const std::vector<InputCSVVideo_Field*>& fields)
{
    fields_wanted = fields;
}
