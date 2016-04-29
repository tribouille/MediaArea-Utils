/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#include "ZenLib/ZtringListList.h"
#include "ZenLib/ZtringList.h"
#include "ZenLib/Ztring.h"
#include "ZenLib/File.h"
#include <iostream>
#include <map>

static int get_lines_columns(const ZenLib::ZtringListList& list, size_t& lines, size_t& columns)
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

static int write_header(ZenLib::File& file)
{
    file.Write(__T("/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.\n"));
    file.Write(__T(" *\n"));
    file.Write(__T(" *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can\n"));
    file.Write(__T(" *  be found in the License.html file in the root of the source tree.\n"));
    file.Write(__T(" */\n"));
    file.Write(__T("\n"));
    file.Write(__T("#include <map>\n"));
    file.Write(__T("#include <vector>\n"));
    file.Write(__T("#include <string>\n"));
    file.Write(__T("\n"));
    file.Write(__T("/* This file is generated, do not modify it without the script. */"));
    file.Write(__T("\n"));
    return 0;
}

static int write_function_top(ZenLib::File& file)
{
    file.Write(__T("int get_generated_values_from_csv(std::map<std::string, std::vector<std::string> >& values)\n"));
    file.Write(__T("{\n"));
    return 0;
}

static bool field_is_wanted(const ZenLib::Ztring& field, const std::vector<ZenLib::Ztring>& fields)
{
    for (size_t i = 0; i < fields.size(); ++i)
        if (fields[i] == field)
            return true;
    return false;
}

static int create_map_for_doubles(const ZenLib::ZtringListList& list,
                                  const std::vector<ZenLib::Ztring>& fields,
                                  std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, bool> >& values)
{
    size_t lines, columns;
    if (get_lines_columns(list, lines, columns))
        return -1;

    for (size_t i = 0; i < columns; ++i)
    {
        if (i >= list[0].size() || !list[0][i].length() || !field_is_wanted(list[0][i], fields))
            continue;

        std::map<ZenLib::Ztring, bool> internal_values;
        // j == 0 is the field name
        for (size_t j = 1; j < lines; ++j)
        {
            if (j >= list.size() || i >= list[j].size() || !list[j][i].length())
                continue;
            internal_values[list[j][i]] = true;
        }
        values[list[0][i]] = internal_values;
    }
    return 0;
}

static int write_function(const std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, bool> >& values,
                          ZenLib::File& file)
{
    std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, bool> >::const_iterator it = values.begin();

    for (; it != values.end(); ++it)
    {
        file.Write(__T("    {\n"));
        file.Write(__T("        std::vector<std::string> tmp;\n"));
        std::map<ZenLib::Ztring, bool>::const_iterator sub_it = it->second.begin();
        for (; sub_it != it->second.end(); ++sub_it)
        {
            ZenLib::Ztring tmp = __T("        tmp.push_back(\"") + sub_it->first + __T("\");\n");
            file.Write(tmp);
        }
        ZenLib::Ztring value = __T("        values[\"") + it->first + __T("\"] = tmp;\n");
        file.Write(value);
        file.Write(__T("    }\n"));
    }
    return 0;
}

static int write_function_end(ZenLib::File& file)
{
    file.Write(__T("    return 0;\n"));
    file.Write(__T("}\n"));
    return 0;
}

static int write_list(const ZenLib::Ztring& filename,
                      const std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, bool> >& values)
{
    ZenLib::File file(filename, ZenLib::File::Access_Write);
    file.Create(filename);

    if (write_header(file) < 0)
        return -1;

    if (write_function_top(file) < 0)
        return -1;

    if (write_function(values, file) < 0)
        return -1;

    if (write_function_end(file) < 0)
        return -1;
    return 0;
}

static int get_list(const ZenLib::Ztring& content, ZenLib::ZtringListList& list)
{
    list.Separator_Set(0, __T("\n"));
    list.Separator_Set(1, __T(";"));
    list.Write(content);
}

static int get_content(const ZenLib::Ztring& filename, ZenLib::Ztring& content)
{
    ZenLib::File file(filename);
    ZenLib::int64u size = file.Size_Get();
    ZenLib::int8u *buffer = new ZenLib::int8u[size];
    size_t size_read = file.Read(buffer, size);

    if (size != size_read)
    {
        std::cerr << "cannot read properly the file" << std::endl;
        return -1;
    }

    content = ZenLib::Ztring((const char*)buffer, size_read);
    return 0;
}

static int parse_fields(const ZenLib::Ztring& arg, std::vector<ZenLib::Ztring>& fields)
{
    ZenLib::ZtringList list;
    list.Separator_Set(0, __T(","));
    list.Write(arg);
    if (!list.size())
        return -1;

    for (size_t i = 0; i < list.size(); ++i)
    {
        std::wcout << list[i] << std::endl;
        fields.push_back(list[i]);
    }

    return 0;
}

static int usage()
{
    std::cerr << "usage: tool input_filename output_filename fields_separated_by_comma" << std::endl;
    return -1;
}

static int get_args(int argc, char* argv[], ZenLib::Ztring& in_filename,
                    ZenLib::Ztring& out_filename, std::vector<ZenLib::Ztring>& fields)
{
    if (argc != 4)
        return usage();

    std::vector<ZenLib::Ztring> args;
    for (int i = 0; i < argc; ++i)
        args.push_back(ZenLib::Ztring().From_UTF8(argv[i]));

    in_filename = args[1];
    out_filename = args[2];
    if (parse_fields(args[3], fields))
        return -1;

    return 0;
}

//---------------------------------------------------------------------------
int main(int argc, char* argv[])
{
    ZenLib::Ztring in_filename;
    ZenLib::Ztring out_filename;
    std::vector<ZenLib::Ztring> fields;
    if (get_args(argc, argv, in_filename, out_filename, fields) < 0)
        return 1;

    ZenLib::Ztring content;
    if (get_content(in_filename, content) < 0)
        return 1;

    ZenLib::ZtringListList list;
    if (get_list(content, list) < 0)
        return 1;

    std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, bool> > values;
    if (create_map_for_doubles(list, fields, values) < 0)
        return 1;

    if (write_list(out_filename, values) < 0)
    {
        std::cerr << "An error occured when writing list" << std::endl;
        return 1;
    }

    return 0;
}
