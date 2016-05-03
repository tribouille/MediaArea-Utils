/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

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

bool ContentCSVVideo::field_is_wanted(const ZenLib::Ztring& field)
{
    // if fields wanted is empty, means all fields
    if (!fields_wanted.size())
        return true;

    for (size_t i = 0; i < fields_wanted.size(); ++i)
        if (fields_wanted[i] == field)
            return true;
    return false;
}

int ContentCSVVideo::create_map_for_doubles(const ZenLib::Ztring& type)
{
    size_t lines, columns;
    if (get_lines_columns(lines, columns))
        return -1;

    for (size_t i = 0; i < columns; ++i)
    {
        if (i >= list[0].size() || !list[0][i].length() || !field_is_wanted(list[0][i]))
            continue;

        std::map<ZenLib::Ztring, bool> internal_values;
        // j == 0 is the field name
        for (size_t j = 1; j < lines; ++j)
        {
            if (j >= list.size() || i >= list[j].size() || !list[j][i].length())
                continue;
            internal_values[list[j][i]] = true;
        }
        output.register_values(type, list[0][i], internal_values);
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

int ContentCSVVideo::parse(const ZenLib::Ztring& filename, const ZenLib::Ztring& type)
{
    if (get_content(filename) < 0)
        return 1;

    if (get_list() < 0)
        return 1;

    if (create_map_for_doubles(type) < 0)
        return 1;
}

void ContentCSVVideo::set_fields_wanted(const std::vector<ZenLib::Ztring>& fields)
{
    fields_wanted = fields;
}
