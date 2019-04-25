#include "ctest.h"
#include <vector>
#include <chrono>

#ifdef _WIN32
  #include <Windows.h>
#endif

namespace testing
{
  ctest::ctest( const char* caseName )
    : caseName_( caseName )
    , next_( nullptr )
    , failed_( false )
  {
    add( this );
  }


  ctest::~ctest( )
  {}


  void ctest::fail( bool value )
  {
    failed_ = value;
  }


  bool ctest::failed( ) const
  {
    return failed_;
  }


  const char* ctest::caseName( ) const
  {
    return caseName_;
  }


  void ctest::add( ctest::ptr test )
  {
    if ( !root_ )
      root_ = test;
    else
      last_->next_ = test;

    last_ = test;
    tests_++;
  }


  void ctest::run( int iterations /*= 1*/)
  {
    using namespace std::chrono;
    steady_clock::time_point t1 = steady_clock::now();
    
    ctest_publisher results;

    results.publish( TESTER_STR( std::string( 80, '-' ) << std::endl <<
                                 "ctest v1.0.0" << std::endl ) );

	  int iteration = 0;
	  for (; iteration < iterations; iteration++)
	  {
      results.publish(TESTER_STR(std::string(80, '-') << std::endl));

      for (ctest::ptr test = root_; test != nullptr; test = test->next_)
		  {
			  auto& t = *test;
			  t(results);

			  if (t.failed())
			  {
				  results.publish(TESTER_STR("[  " << testing::colorize( testing::colorize_t::red ) <<
					  "FAILED" << testing::colorize( testing::colorize_t::white) <<
					  "  ] " << iteration + 1 << " | " << t.caseName_ << std::endl));
				  results.caseFailure();
			  }
			  else
			  {
				  results.publish(TESTER_STR("[  " << testing::colorize( testing::colorize_t::green ) <<
					  "PASSED" << testing::colorize( testing::colorize_t::white) <<
					  "  ] " << iteration + 1 << " | " << t.caseName_ << std::endl));
			  }
		  }

      if (results.caseFailures())
        break;
	  }

    results.publish( TESTER_STR( std::string( 80, '=' ) << std::endl <<
                                 "iterations: " << iteration << std::endl <<
                                 "test cases: " << tests_ << 
                                 " | " << testing::colorize( testing::colorize_t::red ) << results.caseFailures( ) <<
                                 " failed" << testing::colorize( testing::colorize_t::white ) << std::endl <<
                                 "assertions: " << ( results.successes( ) + results.failures( ) ) << 
                                 " | " << testing::colorize( testing::colorize_t::green ) << results.successes( ) <<
                                 " passed" << testing::colorize( testing::colorize_t::white ) <<
                                 " | " << testing::colorize( testing::colorize_t::red ) << results.failures( ) <<
                                 " failed" << testing::colorize( testing::colorize_t::white ) << std::endl ) );

    steady_clock::time_point t2 = steady_clock::now();
    results.publish(TESTER_STR("elapsed:    " << duration_cast<duration<double>>(t2 - t1).count( ) << 
                               " seconds" << std::endl << std::endl ) );


  }


  void ctest::init(int* argc, char** argv)
  {
    // Not support as of yet
  }


  ctest_publisher::ctest_publisher( )
    : success_( 0 )
    , failures_( 0 )
    , caseFailures_( 0 )
  {}


  ctest_publisher::~ctest_publisher( )
  {}


  void ctest_publisher::success( ctest::ptr test )
  {
    success_++;
  }


  int ctest_publisher::successes(  ) const
  {
    return success_;
  }


  void ctest_publisher::failure( const std::string& message, ctest::ptr test )
  {
    failures_++;

    if ( test )
      test->fail( true );

    publish( message );
  }


  int ctest_publisher::failures(  ) const
  {
    return failures_;
  }


  void ctest_publisher::caseFailure( )
  {
    caseFailures_++;
  }


  int ctest_publisher::caseFailures( ) const
  {
    return caseFailures_;
  }


  void ctest_publisher::publish( const std::string& message )
  {
    std::string vs_string;

    int index = 0;
    char c;
    bool col = false;
    std::string text, color;

    while ( c = message[ index++ ] )
    {
      switch ( c )
      {
        case '<':
        {
          printf( text.c_str( ) );
          vs_string += text;
          text.clear( );
          col = true;
          color += c;
        }
        break;

        case '>':
        {
          color += c;
          printf( colorize( color ).to_cmd( ).c_str( ) );
          color.clear( );
          col = false;
        }
        break;

        default:
        {
          if ( col )
            color += c;
          else
            text += c;
        }
      }
    }
    if ( !text.empty( ) )
    {
      printf( text.c_str( ) );
      vs_string += text;
    }

#ifdef _WIN32
    OutputDebugStringA( vs_string.c_str( ) );
#endif
  }


  std::string ctest_publisher::make_string(const std::string& data)
  {
    std::string temp;

    for (auto& c : data)
    {
      if (isprint((int)c))
        temp += c;
      else
        temp += '.';
    }

    return temp;
  }


  colorize::colorize( colorize_t color )
    : color_( color )
  {}


  colorize::colorize( const std::string& str )
    : color_( colorize_t::white )
  {
    auto color = mapped_to_.find( str );
    if ( color != mapped_to_.end( ) )
      color_ = color->second;
  }


  std::string colorize::to_string( ) const
  {
    auto color = mapped_from_.find( color_ );
    if ( color != mapped_from_.end( ) )
      return color->second;
    else
      return "";
  }


  std::string colorize::to_cmd( ) const
  {
    auto color = mapped_cmd_.find( color_ );
    if ( color != mapped_cmd_.end( ) )
#ifdef _WIN32
      SetConsoleTextAttribute( GetStdHandle( STD_OUTPUT_HANDLE ), color->second );
#else
      return color->second;
#endif    
    return "";
  }


  colorize_t colorize::color( ) const
  {
    return color_;
  }


  std::map<colorize_t, std::string> colorize::mapped_from_ =
  {
    { colorize_t::white   , "<white>"   },
    { colorize_t::black   , "<black>"   },
    { colorize_t::red     , "<red>"     },
    { colorize_t::green   , "<green>"   },
    { colorize_t::yellow  , "<yellow>"  },
    { colorize_t::blue    , "<blue>"    },
    { colorize_t::magenta , "<magenta>" },
    { colorize_t::cyan    , "<cyan>"    },
  };


  std::map<std::string, colorize_t> colorize::mapped_to_ =
  {
    { "<white>"   , colorize_t::white   },
    { "<black>"   , colorize_t::black   },
    { "<red>"     , colorize_t::red     },
    { "<green>"   , colorize_t::green   },
    { "<yellow>"  , colorize_t::yellow  },
    { "<blue>"    , colorize_t::blue    },
    { "<magenta>" , colorize_t::magenta },
    { "<cyan>"    , colorize_t::cyan    },
  };

#ifdef _WIN32
  std::map<colorize_t, uint16_t> colorize::mapped_cmd_ =
  {
    { colorize_t::white   , 0b0111 },
    { colorize_t::black   , 0b0000 },
    { colorize_t::red     , 0b1100 },
    { colorize_t::green   , 0b1010 },
    { colorize_t::yellow  , 0b1110 },
    { colorize_t::blue    , 0b1001 },
    { colorize_t::magenta , 0b1101 },
    { colorize_t::cyan    , 0b1011 },
  };
#else
  std::map<colorize_t, std::string> colorize::mapped_cmd_ =
  {
    { colorize_t::white   , "\x1b[0m"   },
    { colorize_t::black   , "\x1b[0m"   },
    { colorize_t::red     , "\x1b[31m"  },
    { colorize_t::green   , "\x1b[32m"  },
    { colorize_t::yellow  , "\x1b[33m"  },
    { colorize_t::blue    , "\x1b[34m"  },
    { colorize_t::magenta , "\x1b[35m"  },
    { colorize_t::cyan    , "\x1b[36m"  },
  };
#endif

  ctest::ptr ctest::root_ = nullptr;
  ctest::ptr ctest::last_ = nullptr;
  int ctest::tests_ = 0;
}
