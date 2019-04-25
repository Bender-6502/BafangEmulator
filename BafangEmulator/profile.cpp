#include "profile.h"
#include "trace.h"
#include <fstream>


namespace core
{
  profile::profile(const std::string& path)
    : exists_(false)
    , filename_(path)
  {
    std::ifstream f(filename_);
    if (f.is_open())
    {
      exists_ = true;
      std::string section;
      while (!f.eof())
      {
        std::string line;
        std::getline(f, line);

        if (line.empty())
        {
          continue;
        }
        else if (line[0] == '[' && line[line.length() - 1] == ']')
        {
          section = line.substr(1, line.length() - 2);
        }
        else
        {
          bool valid = false;
          auto equal = line.find('=');
          if (equal != std::string::npos)
          {
            std::string key = line.substr(0, equal);
            std::string val = line.substr(equal + 1);

            if (!section.empty())
            {
              if (!key.empty() && !val.empty())
              {
                data_[section][key] = val;
                valid = true;
              }
            }
          }

          if (!valid)
          {
            throw std::runtime_error("parse profile failure (" + line + ")");
          }
        }
      }
      TRACE_MESSAGE("profile \"%s\" read", filename_.c_str());
    }
  }


  void profile::save()
  {
    save_as(filename_);
  }


  void profile::save_as(const std::string& path)
  {
    std::ofstream f(path);
    if (f.is_open())
    {
      exists_ = true;
      filename_ = path;
      for (auto& section: data_)
      {
        f << "[" << section.first << "]" << std::endl;
        for (auto& item : section.second)
        {
          f << item.first << "=" << item.second << std::endl;
        }
      }
      TRACE_MESSAGE("profile \"%s\" written", filename_.c_str());
    }
    else
    {
      throw std::runtime_error("save profile failure");
    }
  }


  void profile::add(const std::string& section, const std::string& key, const std::string& value)
  {
    data_[section][key] = value;
  }


  std::string profile::find(const std::string& section, const std::string& key, const std::string& default_value)
  {
    auto sec = data_.find(section);
    if (sec != data_.end())
    {
      auto find = sec->second.find(key);
      if (find != sec->second.end())
        return find->second;
    }

    if (!exists())
      add(section, key, default_value);

    return default_value;
  }


  std::string profile::find(const std::string& section, const std::string& key, const std::string& default_value) const
  {
    auto sec = data_.find(section);
    if (sec != data_.end())
    {
      auto find = sec->second.find(key);
      if (find != sec->second.end())
        return find->second;
    }

    return default_value;
  }


  bool profile::compare(const profile & rhs) const
  {
    return data_ == rhs.data_;
  }
}
