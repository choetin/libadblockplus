#ifndef ADBLOCK_PLUS_FILE_READER_H
#define ADBLOCK_PLUS_FILE_READER_H

#include <istream>
#include <string>

namespace AdblockPlus
{
  class FileReader
  {
  public:
    virtual ~FileReader();
    virtual std::auto_ptr<std::istream> Read(const std::string& path) const = 0;
  };
}

#endif
