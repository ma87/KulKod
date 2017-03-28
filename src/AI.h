#ifndef __AI__H
#define __AI__H

#include <dlfcn.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <string.h>

class AI
{
  public:
   //AI();
   virtual ~AI() {}
   virtual std::string getName() = 0;
   virtual std::string getType() = 0;
};

typedef AI* create_t();
typedef void destroy_t(AI*);

class AIFactory
{
  public:
    AIFactory(char * dllName);
    ~AIFactory();
    AI * create();
    void destroy(AI * ai);
    char * getFactoryName();
  
  private:
    void* m_factory;
    create_t * m_create;
    destroy_t * m_destroy;
    bool m_isLoaded;
    char * m_factoryName;
};

class AIManager
{
  public:
    AIManager();
    ~AIManager();
    AIFactory * loadFactory(char * dllName);
    AI * getAI(char * dllName);
    
  private:
    std::vector<AIFactory> m_factories;
};

#endif