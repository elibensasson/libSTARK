#include "specsPrint.hpp"
#include <iostream>
#include <iomanip>
#include<algorithm>

namespace libstark{

    using std::vector;
    using std::string;
    using std::pair;
    using std::max;

    specsPrinter::specsPrinter(const string title):title_(title),data_(0){};
    void specsPrinter::addLine(const string name, const string val){
        data_.push_back(pair<string,string>(name,val));
    }

    void specsPrinter::print()const{
        using std::cout;
        using std::endl;
        using std::setw;
        using std::setfill;

        int namesLen=0, valsLen = 0;
        for (const auto& p : data_){
            namesLen = max(namesLen,int(p.first.length()));
            valsLen = max(valsLen,int(p.second.length()));
        }

        const int len = max(int(title_.length()), namesLen + valsLen + 3);
        valsLen = len - namesLen - 3;

        cout<<setfill('-')<<setw(4+len)<<"-"<<setfill(' ')<<endl;
        cout<<"| "<<std::left<<setw(len)<<title_<<" |"<<std::endl;
        cout<<setfill('-')<<setw(4+len)<<"-"<<setfill(' ')<<endl;
        for(const auto& p : data_){
            cout<<"| "<<setw(namesLen)<<p.first<<" = "<<setw(valsLen)<<p.second<<" |"<<std::endl;
        }
        cout<<setfill('-')<<setw(4+len)<<"-"<<setfill(' ')<<endl;
    }

} // namespace libstark
