#include "SimpleApplication.h"

int main()
{
        selene::SimpleApplication application("SELENE Device");
        if(application.initialize(640, 480))
                application.run();

        std::cout<<"\n\nPress enter to exit";
        getchar();

        return 0;
}
