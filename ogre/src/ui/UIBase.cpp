
#include "UIBase.h"

#include "../customs/CustomTrayManager.h"

Ogre::PanelOverlayElement* UIBase::createEmptyPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top)
{
    Ogre::PanelOverlayElement* res = NULL;

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 

    res = (Ogre::PanelOverlayElement*)om.createOverlayElement("Panel", name);
    res->setMetricsMode(Ogre::GMM_PIXELS);
    res->setWidth(width);
    res->setHeight(height);
    res->setTop(top);
    res->setLeft(left);
    res->setHorizontalAlignment(Ogre::GHA_LEFT);
    res->setVerticalAlignment(Ogre::GVA_TOP);
    res->show();

    mCreatedElementsVisible.push_back(false);
    mCreatedElements.push_back(res);

    return res;
}

Ogre::PanelOverlayElement* UIBase::createEmptyPanel(const Ogre::String& name, const Ogre::Vector4& pos)
{
    return createEmptyPanel(name, pos.z - pos.x, pos.w - pos.y, pos.x, pos.y);
}

Ogre::PanelOverlayElement* UIBase::createPanel(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top, const Ogre::String& material)
{
    Ogre::PanelOverlayElement* res = NULL;

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 

    res = (Ogre::PanelOverlayElement*)om.createOverlayElement("Panel", name);
    res->setMetricsMode(Ogre::GMM_PIXELS);
    res->setWidth(width);
    res->setHeight(height);
    res->setMaterialName(material);
    res->setTop(top);
    res->setLeft(left);
    res->setHorizontalAlignment(Ogre::GHA_LEFT);
    res->setVerticalAlignment(Ogre::GVA_TOP);
    res->show();

    mCreatedElementsVisible.push_back(false);
    mCreatedElements.push_back(res);

    return res;
}

Ogre::PanelOverlayElement* UIBase::createPanel(const Ogre::String& name, const Ogre::Vector4& pos, const Ogre::String& material)
{
    return createPanel(name, pos.z - pos.x, pos.w - pos.y, pos.x, pos.y, material);
}

Ogre::TextAreaOverlayElement* UIBase::createTextArea(const Ogre::String& name, Ogre::Real width, Ogre::Real height, Ogre::Real left, Ogre::Real top)
{
    Ogre::TextAreaOverlayElement* res = NULL;

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton(); 

    res = (Ogre::TextAreaOverlayElement*)om.createOverlayElement("TextArea", name);
    res->setMetricsMode(Ogre::GMM_PIXELS);
    res->setWidth(width);
    res->setHeight(height);
    res->setTop(top);
    res->setLeft(left);
    res->setHorizontalAlignment(Ogre::GHA_LEFT);
    res->setVerticalAlignment(Ogre::GVA_TOP);
    res->show();

    mCreatedElementsVisible.push_back(false);
    mCreatedElements.push_back(res);

    return res;
}

Ogre::TextAreaOverlayElement* UIBase::createTextArea(const Ogre::String& name, const Ogre::Vector4& pos)
{
    return createTextArea(name, pos.z - pos.x, pos.w - pos.y, pos.x, pos.y);
}

OgreBites::Label* UIBase::createLabel(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width)
{
    OgreBites::Label* res = trayMgr->createLabel(trayLoc, name, caption, width);

    mCreatedWidgetsVisible.push_back(false);
    mCreatedWidgets.push_back(res);

    return res;
}

OgreBites::Button* UIBase::createButton(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width)
{
    OgreBites::Button* res = trayMgr->createButton(trayLoc, name, caption, width);

    mCreatedWidgetsVisible.push_back(false);
    mCreatedWidgets.push_back(res);

    return res;
}

OgreBites::TextBox* UIBase::createTextBox(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width, Ogre::Real height)
{
    OgreBites::TextBox* res = trayMgr->createTextBox(trayLoc, name, caption, width, height);

    mCreatedWidgetsVisible.push_back(false);
    mCreatedWidgets.push_back(res);

    return res;
}

OgreBites::SelectMenu* UIBase::createSelectMenu(CustomTrayManager* trayMgr, OgreBites::TrayLocation trayLoc, const Ogre::String& name, const Ogre::String& caption, Ogre::Real width, unsigned int maxItemsShown, const Ogre::StringVector& items)
{
    OgreBites::SelectMenu* res = trayMgr->createThickSelectMenu(trayLoc, name, caption, width, maxItemsShown, items);

    mCreatedWidgetsVisible.push_back(false);
    mCreatedWidgets.push_back(res);

    return res;
}

void UIBase::destroy(CustomTrayManager* trayMgr)
{
    trayMgr->setListener(NULL);

    Ogre::OverlayManager& om = Ogre::OverlayManager::getSingleton();


    for(size_t q = 0; q < mCreatedElements.size(); ++q)
        om.destroyOverlayElement(mCreatedElements[q]);

    for(size_t q = 0; q < mCreatedWidgets.size(); ++q)
        trayMgr->destroyWidget(mCreatedWidgets[q]);

    mCreatedElementsVisible.clear();
    mCreatedElements.clear();

    mCreatedWidgetsVisible.clear();
    mCreatedWidgets.clear();
}

void UIBase::setVisible(bool isVisible)
{
    if(isVisible)
    {
        for(size_t q = 0; q < mCreatedElements.size(); ++q)
        {
            if(mCreatedElementsVisible[q])
                mCreatedElements[q]->show();
        }

        for(size_t q = 0; q < mCreatedWidgets.size(); ++q)
        {
            if(mCreatedWidgetsVisible[q])
                mCreatedWidgets[q]->show();
        }
    }
    else
    {
        for(size_t q = 0; q < mCreatedElements.size(); ++q)
        {
            mCreatedElementsVisible[q] = mCreatedElements[q]->isVisible();
            mCreatedElements[q]->hide();
        }

        for(size_t q = 0; q < mCreatedWidgets.size(); ++q)
        {
            mCreatedWidgetsVisible[q] = mCreatedWidgets[q]->isVisible();
            mCreatedWidgets[q]->hide();
        }
    }
}