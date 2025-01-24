#include <stdio.h>
#include "AssistRuntime.h"


namespace NAssist
{
    
    int Main(int  /*argc*/,char**  /*argv*/)
    {
        //init the AssistCore
        AssistRuntime instance ;
        instance.init();
        instance.run();

        instance.shutdown();
        return 0;
    }
} // namespace NAssist

int main(int argc,char** argv)
{
    return NAssist::Main(argc,argv);
}





