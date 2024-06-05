#include "time.h"


void convertTime(time_t total_seconds) {
    int hours = total_seconds / 3600;
    int remaining_seconds = total_seconds % 3600;
    int minutes = remaining_seconds / 60;
    int seconds = remaining_seconds % 60;

    std::cout << "时间： " << hours << "时" << minutes << "分" << seconds << "秒" << " ";
}



std::string getCurrentTimestamp(time_t x) {
    std::stringstream ss;
    struct tm tm;
    localtime_r(&x, &tm); 
    ss << std::put_time(&tm, "%Y年%m月%d日 %H:%M:%S"); 
    return ss.str();
}


std::string gethms(time_t x) {
    std::stringstream ss;
    struct tm tm;


    bool is_negative = (x < 0);


    x = std::abs(x);


    localtime_r(&x, &tm);


    if (is_negative) {
        ss << "-";
    }


    ss << std::put_time(&tm, "%H:%M:%S");

    return ss.str();
}



void printDatetime(time_t rawtime) {
    struct tm timeinfo;
   

    localtime_r(&rawtime, &timeinfo); 
    
    int year = timeinfo.tm_year + 1900; 
    int month = timeinfo.tm_mon + 1; 
    int day = timeinfo.tm_mday; 
    int hour = timeinfo.tm_hour; 
    int minute = timeinfo.tm_min; 
    int second = timeinfo.tm_sec;

    std::cout << year << "year" << month << "mouth" << day << "day "
        << hour << ":" << minute << ":" << second << std::endl;
}



