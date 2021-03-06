#ifndef SCRIPT_H
#define SCRIPT_H

const int QUEUE_SIZE = 100;

template<class T>

class Script
{
public:
    Script();
    void SetName(char* name_in);
    void AddSequential(T *command);
    void MergeQueue();
    void NextCommand();
    T* GetCurrentCommand();
    T **commands, **buffer, **tmp;
    int commands_size, buffer_size;
    char *name;
    bool is_new_current_command;
};

#endif // SCRIPT_H
