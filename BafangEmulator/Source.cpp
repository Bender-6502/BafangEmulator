#include "trace.h"
#include "serial_handler.h"
#include "exceptions.h"
#include "profile.h"
#include "getopt.h"

#include <future>
#include <vector>


void usage()
{
  printf("Usage: BafangEmulator -p PORT -g PATH -c PATH\r\n\r\n"
         "Bafang controller emulator, currently only supporting the configuration tool.\r\n\r\n"
         "  -p, --port <ARG>    comms port to connect to, typically COM1...\n"
         "      --port2 <ARG>   second comms port to connect to, typically COM1...\n"
         "      --port3 <ARG>   third comms port to connect to, typically COM1...\n"
         "      --port4 <ARG>   fourth comms port to connect to, typically COM1...\n"
         "  -g, --general <ARG> path for the general profile\n"
         "  -c, --config <ARG>  path of the config profile\n"
         "  -h, --help          display this help and exit\n"
         "  -V, --version       output version information and exit\r\n\r\n");
}

void version()
{
  printf("BafangEmulator 1.0.0\r\n");
}

int main(int argc, char* argv[])
{
  TRACE_INIT(trace::flag_console | trace::flag_file);
  TRACE_FILENAME("BafangEmulator.txt");
  TRACE_MESSAGE("Application start");

  std::vector<std::string> ports;
  std::string general, config;
  option long_options[] =
  {
    { "port",      required_argument, 0, 'p' },
    { "port2",     required_argument, 0,  2  },
    { "port3",     required_argument, 0,  3  },
    { "port4",     required_argument, 0,  4  },
    { "general",   required_argument, 0, 'g' },
    { "config",    required_argument, 0, 'c' },
    { "help",      no_argument,       0, 'h' },
    { "version",   no_argument,       0, 'V' },
    { 0, 0, 0, 0 },
  };

  /* Handle the arguments */
  int c = 0, option_index = 0;
  while ((c = getopt_long(argc, argv, "p:g:c:hV", long_options, &option_index)) >= 0)
  {
    switch (c)
    {
    case 'p':  ports.push_back(optarg); break;
    case  2 :  ports.push_back(optarg); break;
    case  3 :  ports.push_back(optarg); break;
    case  4 :  ports.push_back(optarg); break;
    case 'g':  general = optarg; break;
    case 'c':  config  = optarg; break;
    case 'V':  version();        return 0;
    case 'h':
    case '\0':
    case ':':
    case '?':  usage();          return 0;
    }
  }

  if (!ports.empty() && !general.empty() && !config.empty())
  {
    try
    {
      core::profile g(general);
      core::profile c(config);
      std::vector<std::future<void>> workers;

      // Establish workers for each serial port
      for (const auto& port : ports)
      {
        auto f = std::async([&](std::string prt)
        {
          try
          {
            core::serial_handler s(prt, g, c);
            s.poll();
          }
          catch (...)
          {
            core::exception_handler();
          }
        }, port);

        workers.push_back(std::move(f));
      }

      // Wait for workers to complete
      for (const auto& worker : workers)
      {
        worker.wait();
      }
    }
    catch (...)
    {
      core::exception_handler();
    }
  }
  else
  {
    usage();
    return 1;
  }
}
