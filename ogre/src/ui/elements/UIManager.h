#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <vector>
#include "../../includes/CommonIncludes.h"

template <typename T>
class UIManager
{
public:

    UIManager(){}
    ~UIManager(){}

    T* add()
    {
        mUI.push_back(CommonIncludes::shared_ptr<T>(new T()));
        return mUI[mUI.size() - 1].get();
    }

    void mousePressed(const Ogre::Vector2& pos)
    {
        for(size_t q = 0; q < mUI.size(); ++q)
        {
            mUI[q]->mousePressed(pos);
        }
    }

    void mouseReleased(const Ogre::Vector2& pos)
    {
        for(size_t q = 0; q < mUI.size(); ++q)
        {
            mUI[q]->mouseReleased(pos);
        }
    }

    void mouseMoved(const Ogre::Vector2& pos)
    {
        for(size_t q = 0; q < mUI.size(); ++q)
        {
            mUI[q]->mouseMoved(pos);
        }
    }

    void destroy(CustomTrayManager* trayMgr)
    {
        for(size_t q = 0; q < mUI.size(); ++q)
        {
            mUI[q]->destroy(trayMgr);
        }
    }

    void hide()
    {
        for(size_t q = 0; q < mUI.size(); ++q)
        {
            mUI[q]->hide();
        }
    }

private:

    std::vector<CommonIncludes::shared_ptr<T>> mUI; //to process automatically
};

#endif