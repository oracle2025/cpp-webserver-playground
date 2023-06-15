#include "backward.hpp"

void hello(){
    throw(5);
}
int main(){

    using namespace backward;
    //StackTrace st; st.load_here(32);
    //Printer p; p.print(st);
    backward::SignalHandling sh;

    hello();

}