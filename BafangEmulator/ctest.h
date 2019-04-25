/**
* @file ctest.h
* @author Steve Evans
* @date 9 Oct 2017
* @brief A Simple UnitTest Framework, yes just 2 files :)
*
* This is based on GoogleTest's framework.
*
* Please note, the commandline has not been implemented as of yet
* so we cannot suppress individual tests.
*/
#pragma once
#include <sstream>
#include <string>
#include <ostream>
#include <map>


namespace testing
{
  class ctest_publisher;
  class ctest
  {
  public:

    typedef ctest* ptr;

    ctest( const char* caseName );
    virtual ~ctest( );

    /**
     * Sets the test case as failed
     */
    void fail( bool value );

    /**
     * Returns the failed state
     * true == failed, false == success
     */
    bool failed( ) const;

    /**
     * Returns the case name
     */
    const char* caseName( ) const;

    /**
     * Run all the added tests
     */
	static void run(int iterations = 1);
  static void init(int* argc, char** argv);

  protected:

    /**
     * Add a new test to the chain of tests
     */
    void add( ctest::ptr test );

    virtual void operator()( ctest_publisher& ) = 0;

  private:

    static ctest::ptr root_;
    static ctest::ptr last_;
    static int tests_;

    const char* caseName_;
    ctest::ptr next_;
    bool failed_;
  };

  
  class ctest_publisher
  {
  public:

    ctest_publisher( );
   ~ctest_publisher( );

    /**
     * Notify the publisher the assert was successful
     */
    void success( ctest::ptr test );

    /**
     * Return the number of successful asserts
     */
    int successes( ) const;

    /**
     * Notify the publisher the assert was not successful
     */
    void failure( const std::string& message, ctest::ptr test );

    /**
     * Return the number of failed asserts
     */
    int failures( ) const;

    /**
     * Notify the publisher the test case has failed
     */
    void caseFailure( );

    /**
     * Return the number of failed test cases
     */
    int caseFailures( ) const;

    /**
     * Publish a message to the defined outputs
     * Windows: Visual Studio Output, and Stdout
     * Linux:   Stdout
     */
    void publish( const std::string& message );

    /**
     * Make a printable string
     */
    template<class T>
    static std::string make_string(const T& data)
    {
      std::ostringstream ss;
      ss << data;
      return make_string(ss.str());
    }

    static std::string make_string(const std::string& data);

  private:

    int success_;
    int failures_;
    int caseFailures_;
  };


  enum class colorize_t
  {
    white,
    black,
    red,
    green,
    yellow,
    blue,
    magenta,
    cyan,
  };


  class colorize
  {
  public:

    colorize( colorize_t color );
    colorize( const std::string& str );
   ~colorize( ) = default;

    std::string to_string( ) const;
    std::string to_cmd( ) const;
    colorize_t color( ) const;

  private:

    static std::map<colorize_t, std::string> mapped_from_;
    static std::map<std::string, colorize_t> mapped_to_;
#ifdef _WIN32
    static std::map<colorize_t, uint16_t> mapped_cmd_;
#else
    static std::map<colorize_t, std::string> mapped_cmd_;
#endif
    colorize_t color_;
  };

  // Support for gtest interface
  inline void InitGoogleTest(int* argc, char** argv) { ctest::init(argc, argv); }
}

inline std::ostream& operator<<( std::ostream& str, std::nullptr_t )
{
  return str << "nullptr";
}

inline std::ostream& operator<<( std::ostream& str, const testing::colorize& rhs )
{
  return str << rhs.to_string( );
}

#define TEST( case_name ) \
    struct ctest_##case_name : public testing::ctest \
    { \
      ctest_##case_name() : testing::ctest( #case_name ) \
      {} \
        \
      virtual ~ctest_##case_name() \
      {} \
        \
      virtual void operator()( testing::ctest_publisher& results ) override; \
    } \
    ctest_##case_name##_Instance; \
    void ctest_##case_name::operator()( testing::ctest_publisher& results )

#define EXPECT_COMPARISON( expression, actual, expected ) \
    if ( expression ) \
    { \
      results.success(this); \
    } else { \
      results.failure(TESTER_STR("[  " << testing::colorize( testing::colorize_t::red ) << \
          "FAILED" << testing::colorize( testing::colorize_t::white ) << \
          "  ]" << std::endl << \
          __FILE__ << "(" << __LINE__ << ")" << std::endl << \
          "   actual: " << results.make_string(actual) << std::endl << \
          " expected: " << results.make_string(expected) << std::endl), this); \
    }

#define ASSERT_COMPARISON( expression, actual, expected ) \
    if ( expression ) \
    { \
      results.success(this); \
    } else { \
      results.failure(TESTER_STR("[  " << testing::colorize( testing::colorize_t::red ) << \
          "FAILED" << testing::colorize( testing::colorize_t::white ) << \
          "  ]" << std::endl << \
          __FILE__ << "(" << __LINE__ << ")" << std::endl << \
          "   actual: " << results.make_string(actual) << std::endl << \
          " expected: " << results.make_string(expected) << std::endl), this); \
      return; \
    }

#define EXPECT_CONDITION( expression, condition ) \
    if ( expression ) \
    { \
      results.success( this ); \
    } else { \
      results.failure( TESTER_STR( "[  " << testing::colorize( testing::colorize_t::red ) << \
                                   "FAILED" << testing::colorize( testing::colorize_t::white ) << \
                                   "  ]" << std::endl << \
                                   __FILE__ << "(" <<__LINE__<< ")" << std::endl << \
                                   "condition: " << #condition << std::endl ), this ); \
    }

#define ASSERT_CONDITION( expression, condition ) \
    if ( expression ) \
    { \
      results.success( this ); \
    } else { \
      results.failure( TESTER_STR( "[  " << testing::colorize( testing::colorize_t::red ) << \
                                   "FAILED" << testing::colorize( testing::colorize_t::white ) << \
                                   "  ]" << std::endl << \
                                   __FILE__ << "(" <<__LINE__<< ")" << std::endl << \
                                   "condition: " << #condition << std::endl ), this ); \
      return; \
    }

#define EXPECT_NOTHROW( expression ) \
    try \
    { \
      expression; \
      results.success( this ); \
    } catch ( std::exception& e ) { \
      results.failure( TESTER_STR( "[  " << testing::colorize( testing::colorize_t::red ) << \
                                   "FAILED" << testing::colorize( testing::colorize_t::white ) << \
                                   "  ]" << std::endl << \
                                   __FILE__ << "(" << __LINE__ << ")" << std::endl << \
                                   "exception: " << e.what( ) << std::endl ), this ); \
    }

#define EXPECT_THROW( expression ) \
    try \
    { \
      expression; \
      results.failure( TESTER_STR( "[  " << testing::colorize( testing::colorize_t::red ) << \
                                   "FAILED" << testing::colorize( testing::colorize_t::white ) << \
                                   "  ]" << std::endl << \
                                   __FILE__ << "(" << __LINE__ << ")" << std::endl << \
                                   "exception expected" << std::endl ), this ); \
    } catch ( std::exception& e ) { \
      results.success( this ); \
    }

#define ASSERT_NOTHROW( expression ) \
    try \
    { \
      expression; \
      results.success( this ); \
    } catch ( std::exception& e ) { \
      results.failure( TESTER_STR( "[  " << testing::colorize( testing::colorize_t::red ) << \
                                   "FAILED" << testing::colorize( testing::colorize_t::white ) << \
                                   "  ]" << std::endl << \
                                   __FILE__ << "(" << __LINE__ << ")" << std::endl << \
                                   "exception: " << e.what( ) << std::endl ), this ); \
      return; \
    }

#define ASSERT_THROW( expression ) \
    try \
    { \
      expression; \
      results.failure( TESTER_STR( "[  " << testing::colorize( testing::colorize_t::red ) << \
                                   "FAILED" << testing::colorize( testing::colorize_t::white ) << \
                                   "  ]" << std::endl << \
                                   __FILE__ << "(" << __LINE__ << ")" << std::endl << \
                                   "exception expected" << std::endl ), this ); \
      return; \
    } catch ( std::exception& e ) { \
      results.success( this ); \
    }

#define EXPECT_EQ( actual, expected )        EXPECT_COMPARISON( ( actual ) == ( expected ), actual, expected )
#define EXPECT_NE( actual, expected )        EXPECT_COMPARISON( ( actual ) != ( expected ), actual, expected )
#define EXPECT_LT( actual, expected )        EXPECT_COMPARISON( ( actual ) <  ( expected ), actual, expected )
#define EXPECT_LE( actual, expected )        EXPECT_COMPARISON( ( actual ) <= ( expected ), actual, expected )
#define EXPECT_GT( actual, expected )        EXPECT_COMPARISON( ( actual ) >  ( expected ), actual, expected )
#define EXPECT_GE( actual, expected )        EXPECT_COMPARISON( ( actual ) >= ( expected ), actual, expected )
#define EXPECT_TRUE( condition )             EXPECT_CONDITION( condition, condition )
#define EXPECT_FALSE( condition )            EXPECT_CONDITION( !( condition ), condition )
#define ASSERT_EQ( actual, expected )        ASSERT_COMPARISON( ( actual ) == ( expected ), actual, expected )
#define ASSERT_NE( actual, expected )        ASSERT_COMPARISON( ( actual ) != ( expected ), actual, expected )
#define ASSERT_LT( actual, expected )        ASSERT_COMPARISON( ( actual ) <  ( expected ), actual, expected )
#define ASSERT_LE( actual, expected )        ASSERT_COMPARISON( ( actual ) <= ( expected ), actual, expected )
#define ASSERT_GT( actual, expected )        ASSERT_COMPARISON( ( actual ) >  ( expected ), actual, expected )
#define ASSERT_GE( actual, expected )        ASSERT_COMPARISON( ( actual ) >= ( expected ), actual, expected )
#define ASSERT_TRUE( condition )             ASSERT_CONDITION( condition, condition )
#define ASSERT_FALSE( condition )            ASSERT_CONDITION( !( condition ), condition )

#define TESTER_STR( message )                static_cast<std::ostringstream&>( std::ostringstream() << message ).str( )

#define INIT_TESTS( ... )                    testing::ctest::init( __VA_ARGS__ )
#define RUN_ALL_TESTS( ... )                 testing::ctest::run( __VA_ARGS__ )
