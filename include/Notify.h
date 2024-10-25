#ifndef NOTIFY_H
#define NOTIFY_H
#include <sys/inotify.h>

class Notify
{
private:
    int startNotify();
    int startWatch(int fd);
    void handleFileChange(inotify_event *event, int wd);
public:
    Notify();
    int init();
};

#endif