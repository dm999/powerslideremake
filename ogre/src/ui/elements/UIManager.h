#ifndef UIMANAGER_H
#define UIMANAGER_H

#include <vector>
#include <map>
#include <string>
#include "../../includes/CommonIncludes.h"


/**
 * UIManager - collection of UI elements for automatization
 * assumed no need to cleanup (deleted together with whole mode)
*/
template <typename T>
class UIManager
{
public:

    UIManager(){}
    ~UIManager(){}

    T* add()
    {
        mUI.push_back(std::make_shared<T>());
        return mUI[mUI.size() - 1].get();
    }

    T* add(const std::string& group)
    {
        T* ret = add();

        if(!group.empty())
        {
            typename std::map<std::string, std::vector<T*>>::iterator i = mGroupMapper.find(group);
            if(i != mGroupMapper.end())
            {
                i->second.push_back(ret);
            }
            else
            {
                std::vector<T*> vec;
                vec.push_back(ret);
                mGroupMapper.insert(std::make_pair(group, vec));
            }
        }

        return ret;
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

    void show(const std::string& group)
    {
        if(!group.empty())
        {
            typename std::map<std::string, std::vector<T*>>::iterator i = mGroupMapper.find(group);
            if(i != mGroupMapper.end())
            {
                for(size_t q = 0; q < i->second.size(); ++q)
                {
                    i->second[q]->show();
                }
            }
        }
    }

private:

    std::vector<CommonIncludes::shared_ptr<T>> mUI;
    std::map<std::string, std::vector<T*>> mGroupMapper;
};

#endif