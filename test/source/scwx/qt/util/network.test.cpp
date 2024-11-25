#include <scwx/qt/util/network.hpp>

#include <gtest/gtest.h>



namespace scwx
{
namespace qt
{
namespace util
{

const std::vector<std::pair<const std::string, const std::string>> testUrls = {
   {" https://example.com/ ", "https://example.com/"},
   {"\thttps://example.com/\t", "https://example.com/"},
   {"\nhttps://example.com/\n", "https://example.com/"},
   {"\rhttps://example.com/\r", "https://example.com/"},
   {"\r\nhttps://example.com/\r\n", "https://example.com/"},
   {"    https://example.com/   ", "https://example.com/"},
   {"    \nhttps://example.com/  \n ", "https://example.com/"},
};

TEST(network, NormalizeUrl)
{
   for (auto& pair : testUrls)
   {
      const std::string& preNormalized = pair.first;
      const std::string& expNormalized = pair.second;

      std::string normalized = network::NormalizeUrl(preNormalized);
      EXPECT_EQ(normalized, expNormalized);
   }

}


} // namespace util
} // namespace qt
} // namespace scwx
