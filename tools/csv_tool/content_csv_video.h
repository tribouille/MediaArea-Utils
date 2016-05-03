/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef CONTENT_CSV_VIDEO_H
#define CONTENT_CSV_VIDEO_H

#include "ZenLib/File.h"
#include <ZenLib/Ztring.h>
#include "ZenLib/ZtringListList.h"
#include <map>

class OutputCSVVideo;

class ContentCSVVideo
{
public:
    ContentCSVVideo(OutputCSVVideo& out);
    ~ContentCSVVideo();

    int  parse(const ZenLib::Ztring& filename, const ZenLib::Ztring& type);
    void set_fields_wanted(const std::vector<ZenLib::Ztring>& fields);

private:
    OutputCSVVideo& output;
    ZenLib::Ztring content;
    ZenLib::ZtringListList list;
    std::vector<ZenLib::Ztring> fields_wanted;

    int  get_content(const ZenLib::Ztring& filename);
    int  get_list();
    int  create_map_for_doubles(const ZenLib::Ztring& type);
    bool field_is_wanted(const ZenLib::Ztring& field);
    int  get_lines_columns(size_t& lines, size_t& columns);

    ContentCSVVideo(const ContentCSVVideo&);
    ContentCSVVideo& operator=(const ContentCSVVideo&);
};

#endif // !CONTENT_CSV_VIDEO_H
