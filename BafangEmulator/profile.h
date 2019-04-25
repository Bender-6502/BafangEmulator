#pragma once
#include <map>
#include <string>
#include <sstream>


namespace core
{
  class profile
  {
  public:

    /**
     * @brief Constructs a new profile from a saved profile
     *
     * @param[in] path The profile path
     */
    profile(const std::string& path);

    profile() = default;
    profile(profile&&) = default;
    profile(const profile&) = default;
    profile& operator=(profile&&) = default;
    profile& operator=(const profile&) = default;
   ~profile() = default;

    /**
     * @brief Saves the profile
     */
    void save();

    /**
    * @brief Saves the profile
    *
    * @param[in] path The profile path
    */
    void save_as(const std::string& path);

    /**
     * @brief Add or update a key/value pair
     *
     * @param[in] section The profile section
     * @param[in] key The key
     * @param[in] value The new value
     */
    void add(const std::string& section, const std::string& key, const std::string& value);

    /**
     * @brief Add or update a key/value pair
     *
     * @param[in] section The profile section
     * @param[in] key The key
     * @param[in] value The new value
     */
    template<class T>
    void add(const std::string& section, const std::string& key, const T& value)
    {
      add(section, key, std::to_string(value));
    }

    /**
     * @brief Find a key/value pair from within a section
     *
     * @param[in] section The profile section
     * @param[in] key The key
     * @param[in] value The default value to return if the key is missing
     */
    std::string find(const std::string& section, const std::string& key, const std::string& default_value);
    std::string find(const std::string& section, const std::string& key, const std::string& default_value) const;

    /**
     * @brief Find a key/value pair from within a section
     *
     * @param[in] section The profile section
     * @param[in] key The key
     * @param[in] value The default value to return if the key is missing
     */
    template<class T>
    T find(const std::string& section, const std::string& key, const T& default_value)
    {
      std::istringstream ss(find(section, key, std::to_string(default_value)));
      T t = T();
      ss >> t;
      return t;
    }

    template<class T>
    T find(const std::string& section, const std::string& key, const T& default_value) const
    {
      std::istringstream ss(find(section, key, std::to_string(default_value)));
      T t = T();
      ss >> t;
      return t;
    }

    /**
     * @brief Compare two profiles
     *
     * @param[in] rhs The profile to compare with
     */
    bool compare(const profile& rhs) const;

    /**
     * @brief Returns true if profile exists on disk
     */
    bool exists() const
    {
      return exists_;
    }

  private:

    bool exists_;
    std::string filename_;
    std::map<std::string, std::map<std::string, std::string>> data_;
  };
}

template<class T, class U>
inline bool operator==(const std::map<T, U>& lhs, const std::map<T, U>& rhs)
{
  if (lhs.size() != rhs.size())
  {
    return false;
  }

  for (auto& item_lhs : lhs)
  {
    auto item_rhs = rhs.find(item_lhs.first);
    if (item_rhs == rhs.end())
    {
      return false;
    }

    if (item_lhs.second != item_rhs->second)
    {
      return false;
    }
  }
  return true;
}


template<class T, class U>
inline bool operator!=(const std::map<T, U>& lhs, const std::map<T, U>& rhs)
{
  return !(lhs == rhs);
}


inline bool operator==(const core::profile& lhs, const core::profile& rhs)
{
  return lhs.compare(rhs);
}


inline bool operator!=(const core::profile& lhs, const core::profile& rhs)
{
  return !(lhs == rhs);
}
