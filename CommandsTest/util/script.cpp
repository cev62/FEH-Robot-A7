#include "script.h"

template<class T>
Script<T>::Script(char* name_in)
{
    commands = new T*[QUEUE_SIZE];
    buffer = new T*[QUEUE_SIZE];
    tmp = new T*[QUEUE_SIZE];
    commands_size = 0;
    buffer_size = 0;
    name = name_in;
}

template<class T>
void Script<T>::AddSequential(T *command)
{
    buffer[buffer_size] = command;
    buffer_size++;
}

template<class T>
void Script<T>::MergeQueue()
{
    for(int i = 0; i < commands_size; i++)
    {
        tmp[i] = commands[i];
    }
    for(int j = 0; j < buffer_size; j++)
    {
        commands[j] = buffer[j];
    }
    for(int k = buffer_size; k < commands_size + buffer_size; k++)
    {
        commands[k] = tmp[k - buffer_size];
    }
    commands_size = commands_size + buffer_size;
    buffer_size = 0;
}

template<class T>
T* Script<T>::GetCurrentCommand()
{
    return commands[0];
}

template<class T>
void Script<T>::NextCommand()
{
    for(int i = 0; i < commands_size - 1; i++ )
    {
        tmp[i] = commands[i + 1];
    }
    commands_size = commands_size - 1;
    for (int j = 0; j < commands_size; j++)
    {
        commands[j] = tmp[j];
    }
}
