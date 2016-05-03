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

    ContentCSVVideo content(output);

    content.set_fields_wanted(input.get_fields_wanted());

    const std::map<ZenLib::Ztring, std::vector<ZenLib::Ztring> >& in = input.get_inputs();
    std::map<ZenLib::Ztring, std::vector<ZenLib::Ztring> >::const_iterator it = in.begin();
    for (; it != in.end(); ++it)
    {
        for (size_t i = 0; i < it->second.size(); ++i)
            if (content.parse(it->second[i], it->first) < 0)
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
