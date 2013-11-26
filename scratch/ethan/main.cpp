/* Include my headers */

#include "main.hpp"

void flush_all(void)
{
    std::cout.flush();
    std::clog.flush();
    std::cerr.flush();
}


int main(int argc, char **argv)
{
    int retval = 0;

    try
    {
        retval = ede::main(argc, argv);
    }
    catch(const err::system_exit &e)
    {
        if (e.code() != 0)
        {
            std::cerr << "Abnormal exit for following reason: " << e.what() << std::endl;
        }
        retval = e.code();
    }
    catch(const err::exception &e)
    {
        std::cerr << "There was an unexpected exit due to the following exception: ";
        std::cerr << e.what() << std::endl;
        retval = 1;
    }
    catch(const std::exception &e)
    {
        std::cerr << "There was an unexpected exit due to the following standard exception: ";
        std::cerr << e.what() << std::endl;
        retval = 1;
    }
    catch(...)
    {
        std::cerr << "An uncatchable exception was thrown. Abnormal exit." << std::endl;
        retval = 2;
    }

    flush_all();
    return retval;

}

