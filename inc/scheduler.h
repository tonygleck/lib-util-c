#ifndef SCHEDULER_H
#define SCHEDULER_H

typedef struct SCHEDULER_TAG* SCHEDULER_HANDLE;

extern SCHEDULER_HANDLE scheduler_create();
extern void scheduler_create(SCHEDULER_HANDLE handle);

#endif // SCHEDULER
