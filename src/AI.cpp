#include "AI.h"

using std::cout;
using std::cerr;

AIFactory::AIFactory(char * filename)
{
  m_isLoaded = true;
  m_factoryName = filename;

  m_factory = dlopen(filename, RTLD_LAZY);
  if (!m_factory)
  {
      cerr << "Cannot load library: " << dlerror() << '\n';
      m_isLoaded = false;
  }

  // reset errors
  dlerror();

  // load the symbols
  m_create = (create_t*) dlsym(m_factory, "create");
  const char* dlsym_error = dlerror();
  if (dlsym_error) {
      cerr << "Cannot load symbol create: " << dlsym_error << '\n';
      m_isLoaded = false;
  }

  m_destroy = (destroy_t*) dlsym(m_factory, "destroy");
  dlsym_error = dlerror();
  if (dlsym_error) {
      cerr << "Cannot load symbol destroy: " << dlsym_error << '\n';
      m_isLoaded = false;
  }
}

AI* AIFactory::create()
{
  if (m_isLoaded)
  {
    return m_create();
  }
  else
  {
    return NULL;
  }
}

void AIFactory::destroy(AI * ai)
{
  m_destroy(ai);
}

char * AIFactory::getFactoryName()
{
  return m_factoryName;
}

AIFactory::~AIFactory()
{
  dlclose(m_factory);
}

AIManager::AIManager()
 : m_factories()
{

}

AIManager::~AIManager()
{

}

AI * AIManager::getAI(char * dllName)
{
  AIFactory  * factory = loadFactory(dllName);
  if (factory != NULL)
  {
    return factory->create();
  }
  else
  {
    return NULL;
  }
}

AIFactory * AIManager::loadFactory(char * dllName)
{
  bool isFactoryAlreadyCreated = false;
  AIFactory * factory;
  for(auto it = std::begin(m_factories) ; it != std::end(m_factories) ; ++it)
  {
    if (!strcmp((*it)->getFactoryName(), dllName))
    {
      std::cout << "found factory " << dllName << std::endl;
      return (*it);
    }
  }

  if (!isFactoryAlreadyCreated)
  {
    std::cout << "create factory " << dllName << std::endl;
    AIFactory * factory = new AIFactory(dllName);
    m_factories.push_back(factory);

    return factory;
  }

  return NULL; //TODO handle case AIFactory returns null
}
