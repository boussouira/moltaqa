#include "listmanager.h"
#include "utils.h"

ListManager::ListManager(QObject *parent) :
    QObject(parent)
{
}

ListManager::~ListManager()
{
}

void ListManager::reloadModels()
{
    clear();
    loadModels();
}
