#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

class KernelSemaphore
{
private:
    int semid;

public:
    KernelSemaphore(int value, key_t a_key) // get a key from ftok
    {
        struct sembug sb = {0, value, 0};
        semid            = semget(a_key, 1, IPC_CREAT | IPC_EXCL | 0666);
        if(semid < 0)
            {
                semid = semget(a_key, 1, IPC_CREAT | 0666);
            }
        else
            {
                semop(semid, &sb, 1);
            }
    }
    ~KernelSemaphore()
    {
        semctl(semid, 0, IPC_RMID);
    }
    void P()
    {
        struct sembug sb = {0, -1, 0};
        semop(semid, &sb, 1);
    }
    void V()
    {
        struct sembug sb = {0, 1, 0};
        semop(semid, &sb, 1);
    }
}
