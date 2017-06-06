#ifndef LOADERLISTENER_H
#define LOADERLISTENER_H

class LoaderListener
{
public:

    virtual void loadState(float percent, const std::string& info) = 0;
};


#endif