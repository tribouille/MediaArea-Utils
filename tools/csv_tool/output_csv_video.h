/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef OUTPUT_CSV_VIDEO_H
#define OUTPUT_CSV_VIDEO_H

#include "ZenLib/File.h"
#include <ZenLib/Ztring.h>
#include <map>
#include <vector>

class OutputCSVVideo
{
public:
    OutputCSVVideo();
    ~OutputCSVVideo();

    int open_file(const ZenLib::Ztring& filename);
    int write();
    int register_values(const ZenLib::Ztring& field, const std::map<ZenLib::Ztring, ZenLib::Ztring>& values);

    void set_current_type(const ZenLib::Ztring& type) { current_type = type;}

private:
    ZenLib::File *file;
    std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, ZenLib::Ztring> > > values;

    int write_header();
    int write_function_top();
    int write_type_function();
    int write_fields_function(const std::map<ZenLib::Ztring, std::map<ZenLib::Ztring, ZenLib::Ztring> >& values);
    int write_function_end();
    void fill_values_ordered(const std::map<ZenLib::Ztring, ZenLib::Ztring>& values, std::vector<ZenLib::Ztring>& res);
    bool next_is_bigger(const ZenLib::Ztring& val, std::vector<ZenLib::Ztring>& res, std::vector<ZenLib::Ztring>::iterator& it);

    ZenLib::Ztring current_type;

    OutputCSVVideo(const OutputCSVVideo&);
    OutputCSVVideo& operator=(const OutputCSVVideo&);
};

#endif // !OUTPUT_CSV_VIDEO_H