/*  Copyright (c) MediaArea.net SARL. All Rights Reserved.
 *
 *  Use of this source code is governed by a GPLv3+/MPLv2+ license that can
 *  be found in the License.html file in the root of the source tree.
 */

#ifndef INPUT_CSV_VIDEO_H
#define INPUT_CSV_VIDEO_H

#include <ZenLib/Ztring.h>
#include <map>
#include <vector>

struct InputCSVVideo_Field
{
    ZenLib::Ztring field;
    ZenLib::Ztring filter;
    ZenLib::Ztring method;
    ZenLib::Ztring list;
};

class InputCSVVideo
{
public:
    InputCSVVideo();
    ~InputCSVVideo();

    int parse(int argc, char* argv[]);

    ZenLib::Ztring get_output() const { return out_filename; }
    ZenLib::Ztring get_stats_filename(const ZenLib::Ztring& type) { if (stats_filenames.find(type) != stats_filenames.end()) return stats_filenames.find(type)->second; return ZenLib::Ztring(); }
    const std::map<ZenLib::Ztring, std::vector<InputCSVVideo_Field*> >& get_fields_wanted() const { return fields; }

private:
    ZenLib::Ztring out_filename;
    std::map<ZenLib::Ztring, ZenLib::Ztring>                     stats_filenames;
    std::map<ZenLib::Ztring, std::vector<InputCSVVideo_Field*> > fields;

    int usage();
    int parse_in_csv_dir(const ZenLib::Ztring& arg);
    int parse_in_General(const ZenLib::Ztring& arg);
    int parse_in_Video(const ZenLib::Ztring& arg);
    int parse_in_Audio(const ZenLib::Ztring& arg);
    int parse_in_Text(const ZenLib::Ztring& arg);
    int parse_in_Other(const ZenLib::Ztring& arg);
    int parse_in_Image(const ZenLib::Ztring& arg);
    int parse_in_Menu(const ZenLib::Ztring& arg);
    int parse_out_file(const ZenLib::Ztring& arg);

    int parse_csv_fields(const ZenLib::Ztring& type, const ZenLib::Ztring& filename);

    InputCSVVideo(const InputCSVVideo&);
    InputCSVVideo& operator=(const InputCSVVideo&);
};

#endif // !INPUT_CSV_VIDEO_H
