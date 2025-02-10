
#include "Runtime/AssistRuntime.h"
#include "GUI/AssistRuntimeWindow.h"

#include <QApplication>
namespace NAssist
{
    
    int Main(int  argc,char**  argv)
    {
        //init the AssistCore
        //AssistRuntime instance;
        
        
        //init the gui
        QApplication app(argc, argv);
        AssistRuntimeWindow win;
        
        
        win.show();
        auto exit_code = app.exec();
        
        return exit_code;
    }
} // namespace NAssist

int main(int argc,char** argv)
{
    
    return NAssist::Main(argc,argv);
}





