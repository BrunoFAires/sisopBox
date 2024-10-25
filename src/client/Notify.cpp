#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include "Notify.h"

#define MAX_EVENTS 1024
#define LEN_NAME 255
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))
#define DIR_NAME "sync_dir"

Notify::Notify()
{
}

int Notify::init()
{
    int length, i = 0, fd, wd;
    char buffer[BUF_LEN];

    fd = startNotify();
    wd = startWatch(fd);

    while (1)
    {
        i = 0;
        length = read(fd, buffer, BUF_LEN);

        while (i < length)
        {
            struct inotify_event *event = (struct inotify_event *)&buffer[i];
            if (event->len)
            {
                handleFileChange(event, wd);
                i += EVENT_SIZE + event->len;
            }
        }
    }

    inotify_rm_watch(fd, wd);
    close(fd);

    return fd;
}

void Notify::handleFileChange(inotify_event *event, int wd)
{
    // TODO tratar o lock dos arquivos que são criados ao editar(.swp). Envia-lo somente quando o lock for liberado.
    // Devemos impossbilitar a edição de um arquivo por clientes diferentes?
    if (event->mask & IN_DELETE)
    {
        printf("The file %s was deleted with WD %d\n", event->name, event->wd);
    }
    
    if (event->mask & IN_MOVED_TO)
    {
        printf("The file %s was removed from folder with WD %d\n", event->name, event->wd);
    }

    if (event->mask & IN_MOVED_FROM)
    {
        printf("The file %s was moved to folder with WD %d\n", event->name, event->wd);
    }

    if (event->mask & IN_CLOSE_WRITE)
    {
        printf("The file %s was modified with WD %d\n", event->name, event->wd);
    }
}

int Notify::startNotify()
{
    int fd = inotify_init();
    if (fd < 0)
    {
        perror("Couldn't initialize inotify");
        return -1;
    }

    return fd;
}

int Notify::startWatch(int fd)
{
    int wd = inotify_add_watch(fd, DIR_NAME, IN_MOVED_TO | IN_MOVED_FROM | IN_CLOSE_WRITE | IN_DELETE);
    if (wd == -1)
    {
        printf("Couldn't add watch to %s: %s\n", DIR_NAME, strerror(errno));
        return -1;
    }

    return wd;
}