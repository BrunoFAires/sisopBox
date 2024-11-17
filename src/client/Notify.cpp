#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <limits.h>
#include <unistd.h>
#include <string.h>
#include "Notify.h"
#include "Service.h"

#define MAX_EVENTS 1024
#define LEN_NAME 255
#define EVENT_SIZE (sizeof(struct inotify_event))
#define BUF_LEN (MAX_EVENTS * (EVENT_SIZE + LEN_NAME))
#define DIR_NAME "sync_dir"

Notify::Notify(Client *client) : client(client)
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

bool isSyncFile(string filename)
{
    const std::string suffix = ".sync";
    // Verifica se o tamanho da string é suficiente para conter o sufixo
    if (filename.size() < suffix.size())
    {
        return false;
    }
    // Compara o final da string com o sufixo
    return filename.compare(filename.size() - suffix.size(), suffix.size(), suffix) == 0;
}

void Notify::handleFileChange(inotify_event *event, int wd)
{
    // TODO tratar o lock dos arquivos que são criados ao editar(.swp). Envia-lo somente quando o lock for liberado.
    // Devemos impossbilitar a edição de um arquivo por clientes diferentes?
    string filename(event->name); // Convert to std::st
    if (event->mask & IN_DELETE)
    {
        string dir = "dir/" + client->getUsername() + "/" + filename;
        remove(dir.c_str());
        cout << "removido: " << dir << endl;
    }

    if (isSyncFile(filename))
        return;

    if (event->mask & IN_MOVED_TO)
    {

        sendFile(client->getSocketId(), DIR_NAME, filename);
        printf("The file %s was moved from folder with WD %d\n", event->name, event->wd);
    }

    if (event->mask & IN_MOVED_FROM)
    {
        string dir = "dir/" + client->getUsername() + "/" + filename;
        remove(dir.c_str());
        cout << "removido: " << dir << endl;
        printf("The file %s was removed to folder with WD %d\n", event->name, event->wd);
    }

    if (event->mask & IN_CLOSE_WRITE)
    {
        sendFile(client->getSocketId(), DIR_NAME, filename);

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